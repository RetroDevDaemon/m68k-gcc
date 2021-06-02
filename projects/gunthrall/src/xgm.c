#include "../inc/sgdk/config.h"
#include "../inc/sgdk/types.h"

#include "xgm.h"

#include "../inc/sgdk/z80_ctrl.h"
#include "../inc/sgdk/smp_null.h"
#include "../inc/sgdk/sys.h"
#include "../inc/sgdk/mapper.h"

//// just to get xgmstop resource
#include "../inc/sgdk/vdp.h"
#include "../inc/sgdk/bmp.h"
#include "../inc/sgdk/vdp_tile.h"
#include "../inc/sgdk/libres.h"


// allow to access it without "public" share
volatile u16 VBlankProcess;
u16 currentDriver;
u16 driverFlags;

// specific for the XGM driver
static u16 xgmTempo;
static u16 xgmTempoDef;
// can be nice to alter it from external
s16 xgmTempoCnt = 0;

// Z80 cpu load calculation for XGM driver
static u16 xgmIdleTab[32];
static u16 xgmWaitTab[32];
static u16 xgmTabInd;
static u16 xgmIdleMean;
static u16 xgmWaitMean;


// Z80_DRIVER_XGM
// XGM driver
///////////////////////////////////////////////////////////////

u8 XGM_isPlaying()
{
    vu8 *pb;
    u8 ret;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    // point to Z80 status
    pb = (u8 *) Z80_DRV_STATUS;

    Z80_requestBus(TRUE);
    // play status
    ret = *pb & (1 << 6);
    if (!busTaken) Z80_releaseBus();

    return ret;
}

void XGM_startPlay(const u8 *song)
{
    u8 ids[0x100-4];
    u32 addr;
    u16 i;
    vu8 *pb;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    // prepare sample id table
    for(i = 0; i < 0x3F; i++)
    {
        // sample address in sample bank data
        addr = song[(i * 4) + 0] << 8;
        addr |= ((u32) song[(i * 4) + 1]) << 16;

        // silent sample ? use null sample address
        if (addr == 0xFFFF00) addr = (u32) smp_null;
        // adjust sample address (make it absolute)
        else addr += ((u32) song) + 0x100;

        // write adjusted addr
        ids[(i * 4) + 0] = addr >> 8;
        ids[(i * 4) + 1] = addr >> 16;
        // and recopy len
        ids[(i * 4) + 2] = song[(i * 4) + 2];
        ids[(i * 4) + 3] = song[(i * 4) + 3];
    }

    // upload sample id table (first entry is silent sample, we don't transfer it)
    Z80_upload(0x1C00 + 4, ids, 0x100 - 4, FALSE);

    // get song address and bypass sample id table
    addr = ((u32) song) + 0x100;
    // bypass sample data (use the sample data size)
    addr += song[0xFC] << 8;
    addr += ((u32) song[0xFD]) << 16;
    // and bypass the music data size field
    addr += 4;

    // request Z80 BUS
    Z80_requestBus(TRUE);

    // point to Z80 XGM address parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x00);
    // set XGM music data address
    pb[0x00] = addr >> 0;
    pb[0x01] = addr >> 8;
    pb[0x02] = addr >> 16;
    pb[0x03] = addr >> 24;

    // point to Z80 command
    pb = (u8 *) Z80_DRV_COMMAND;
    // set play XGM command
    *pb |= (1 << 6);

    // point to PENDING_FRM parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0F);
    // clear pending frame
    *pb = 0;

    if (!busTaken) Z80_releaseBus();
}

void XGM_startPlay_FAR(const u8 *song, u32 size)
{
    XGM_startPlay(FAR_SAFE(song, size));
}

void XGM_stopPlay()
{
    vu8 *pb;
    u32 addr;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);

    // special xgm sequence to stop any sound
    addr = (u32) stop_xgm;

    // point to Z80 XGM address parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x00);

    // set XGM music data address
    pb[0x00] = addr >> 0;
    pb[0x01] = addr >> 8;
    pb[0x02] = addr >> 16;
    pb[0x03] = addr >> 24;

    // point to Z80 command
    pb = (u8 *) Z80_DRV_COMMAND;
    // set play XGM command
    *pb |= (1 << 6);

    // point to PENDING_FRM parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0F);
    // clear pending frame
    *pb = 0;

    if (!busTaken) Z80_releaseBus();
}

void XGM_pausePlay()
{
    vu8 *pb;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);

    // point to Z80 command
    pb = (u8 *) Z80_DRV_COMMAND;
    // set pause XGM command
    *pb |= (1 << 4);

    if (!busTaken) Z80_releaseBus();
}

void XGM_resumePlay()
{
    vu8 *pb;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);

    // point to Z80 command
    pb = (u8 *) Z80_DRV_COMMAND;
    // set resume XGM command
    *pb |= (1 << 5);

    // point to PENDING_FRM parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0F);
    // clear pending frame
    *pb = 0;

    if (!busTaken) Z80_releaseBus();
}

u8 XGM_isPlayingPCM(const u16 channel_mask)
{
    vu8 *pb;
    u8 ret;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);

    // point to Z80 status
    pb = (u8 *) Z80_DRV_STATUS;
    // play status
    ret = *pb & (channel_mask << Z80_DRV_STAT_PLAYING_SFT);

    if (!busTaken) Z80_releaseBus();

    return ret;
}

void XGM_setPCM(const u8 id, const u8 *sample, const u32 len)
{
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);
    XGM_setPCMFast(id, sample, len);
    if (!busTaken) Z80_releaseBus();
}

void XGM_setPCMFast(const u8 id, const u8 *sample, const u32 len)
{
    vu8 *pb;

    // point to sample id table
    pb = (u8 *) (0xA01C00 + (id * 4));

    // write sample addr
    pb[0x00] = ((u32) sample) >> 8;
    pb[0x01] = ((u32) sample) >> 16;
    pb[0x02] = len >> 8;
    pb[0x03] = len >> 16;
}

void XGM_startPlayPCM(const u8 id, const u8 priority, const u16 channel)
{
    vu8 *pb;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);

    // point to Z80 PCM parameters
    pb = (u8 *) (Z80_DRV_PARAMS + 0x04 + (channel * 2));

    // set PCM priority and id
    pb[0x00] = priority & 0xF;
    pb[0x01] = id;

    // point to Z80 command
    pb = (u8 *) Z80_DRV_COMMAND;
    // set play PCM channel command
    *pb |= (Z80_DRV_COM_PLAY << channel);

    if (!busTaken) Z80_releaseBus();
}

void XGM_stopPlayPCM(const u16 channel)
{
    vu8 *pb;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);

    // point to Z80 PCM parameters
    pb = (u8 *) (Z80_DRV_PARAMS + 0x04 + (channel * 2));

    // use silent PCM (id = 0) with maximum priority
    pb[0x00] = 0xF;
    pb[0x01] = 0;

    // point to Z80 command
    pb = (u8 *) Z80_DRV_COMMAND;
    // set play PCM channel command
    *pb |= (Z80_DRV_COM_PLAY << channel);

    if (!busTaken) Z80_releaseBus();
}

void XGM_setLoopNumber(s8 value)
{
    vu8 *pb;
    bool busTaken = Z80_isBusTaken();

    // load the appropriate driver if not already done
    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);

    Z80_requestBus(TRUE);

    // point to Z80 PCM parameters
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0C);

    // set loop argument (+1 as internally 0 = infinite)
    *pb = value + 1;

    if (!busTaken) Z80_releaseBus();
}

void XGM_set68KBUSProtection(u8 value)
{
    vu8 *pb;

    // nothing to do (driver should be loaded here)
    if (currentDriver != Z80_DRIVER_XGM)
        return;

    bool busTaken = Z80_getAndRequestBus(TRUE);

    // point to Z80 PROTECT parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0D);
    *pb = value;

    // release bus
    if (!busTaken) Z80_releaseBus();
}


u16 XGM_getManualSync()
{
    return driverFlags & DRIVER_FLAG_MANUALSYNC_XGM;
}

void XGM_setManualSync(u16 value)
{
    // nothing to do
    if (currentDriver != Z80_DRIVER_XGM)
        return;

    if (value)
    {
        driverFlags |= DRIVER_FLAG_MANUALSYNC_XGM;
        // remove VInt XGM process
        VBlankProcess &= ~PROCESS_XGM_TASK;
    }
    else
    {
        driverFlags &= ~DRIVER_FLAG_MANUALSYNC_XGM;
        // set VInt XGM process
        VBlankProcess |= PROCESS_XGM_TASK;
    }
}

u16 XGM_getForceDelayDMA()
{
    return driverFlags & DRIVER_FLAG_DELAYDMA_XGM;
}

void XGM_setForceDelayDMA(u16 value)
{
    // nothing to do
    if (currentDriver != Z80_DRIVER_XGM)
        return;

    if (value)
        driverFlags |= DRIVER_FLAG_DELAYDMA_XGM;
    else
        driverFlags &= ~DRIVER_FLAG_DELAYDMA_XGM;
}

u16 XGM_getMusicTempo()
{
    return xgmTempo;
}

void XGM_setMusicTempo(u16 value)
{
    xgmTempo = value;
    if (IS_PALSYSTEM) xgmTempoDef = 50;
    else xgmTempoDef = 60;
}

u32 XGM_getElapsed()
{
    vu8 *pb;
    u8 *dst;
    u8 values[3];
    u32 result;

    // nothing to do (driver should be loaded here)
    if (currentDriver != Z80_DRIVER_XGM)
        return 0;

    // point to ELAPSED value
    pb = (u8 *) (Z80_DRV_PARAMS + 0x90);
    dst = values;

    bool busTaken = Z80_getAndRequestBus(TRUE);

    // copy quickly elapsed time
    *dst++ = *pb++;
    *dst++ = *pb++;
    *dst = *pb;

    if (!busTaken) Z80_releaseBus();

    result = (values[0] << 0) | (values[1] << 8) | ((u32) values[2] << 16);

    // fix possible 24 bit negative value (parsing first extra frame)
    if (result >= 0xFFFFF0) return 0;

    return result;
}

u32 XGM_getCPULoad()
{
    vu8 *pb;
    u16 idle;
    u16 wait;
    u16 ind;
    s16 load;

    // nothing to do (driver should be loaded here)
    if (currentDriver != Z80_DRIVER_XGM)
        return 0;

    bool busTaken = Z80_getAndRequestBus(TRUE);

    // point to Z80 'idle wait loop' value
    pb = (u8 *) (Z80_DRV_PARAMS + 0x7C);

    // get idle
    idle = pb[0] + (pb[1] << 8);
    // reset it and point on 'dma wait loop'
    *pb++ = 0;
    *pb++ = 0;

    // get dma wait
    wait = pb[0] + (pb[1] << 8);
    // and reset it
    *pb++ = 0;
    *pb = 0;

    if (!busTaken) Z80_releaseBus();

    ind = xgmTabInd;

    xgmIdleMean -= xgmIdleTab[ind];
    xgmIdleMean += idle;
    xgmIdleTab[ind] = idle;

    xgmWaitMean -= xgmWaitTab[ind];
    xgmWaitMean += wait;
    xgmWaitTab[ind] = wait;

    xgmTabInd = (ind + 1) & 0x1F;

    load = 105 - (xgmIdleMean >> 5);

    return load | ((u32) (xgmWaitMean >> 5) << 16);
}

void XGM_resetLoadCalculation()
{
    u16 i;
    u16 *s1;
    u16 *s2;

    s1 = xgmIdleTab;
    s2 = xgmWaitTab;
    i = 32;
    while(i--)
    {
        *s1++ = 0;
        *s2++ = 0;
    }

    xgmTabInd = 0;
    xgmIdleMean = 0;
    xgmWaitMean = 0;
}

void XGM_nextXFrame(u16 num)
{
    vu16 *pw_bus;
    vu16 *pw_reset;
    vu8 *pb;

    // nothing to do (driver should be loaded here)
    if (currentDriver != Z80_DRIVER_XGM)
        return;

    bool busTaken = Z80_isBusTaken();

    // point on bus req and reset ports
    pw_bus = (u16 *) Z80_HALT_PORT;
    pw_reset = (u16 *) Z80_RESET_PORT;
    // point to MODIFYING_F parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0E);

    while(TRUE)
    {
        // take bus and end reset (fast method)
        *pw_bus = 0x0100;
        *pw_reset = 0x0100;
        // wait for bus taken
        while (*pw_bus & 0x0100);

        // Z80 not accessing ?
        if (!*pb) break;

        // release bus
        *pw_bus = 0x0000;

        // wait a bit (about 80 cycles)
        asm volatile ("\t\tmovm.l %d0-%d3,-(%sp)\n");
        asm volatile ("\t\tmovm.l (%sp)+,%d0-%d3\n");
    }

    // point to PENDING_FRM parameter
    pb++;
    // increment frame to process
    *pb += num;

    // release bus
    if (!busTaken) *pw_bus = 0x0000;
}

// VInt processing for XGM driver
void XGM_doVBlankProcess()
{
    vu16 *pw_bus;
    vu16 *pw_reset;
    vu8 *pb;
    s16 cnt = xgmTempoCnt;
    u16 step = xgmTempoDef;
    u16 num = 0;

    while(cnt <= 0)
    {
        num++;
        cnt += step;
    }

    xgmTempoCnt = cnt - xgmTempo;

    // directly do the frame here as we want this code to be as fast as possible (to not waste vint time)
    // driver should be loaded here
    bool busTaken = Z80_isBusTaken();

    // point on bus req and reset ports
    pw_bus = (u16 *) Z80_HALT_PORT;
    pw_reset = (u16 *) Z80_RESET_PORT;
    // point to MODIFYING_F parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0E);

    while(TRUE)
    {
        // take bus and end reset (fast method)
        *pw_bus = 0x0100;
        *pw_reset = 0x0100;
        // wait for bus taken
        while (*pw_bus & 0x100);

        // Z80 not accessing ?
        if (!*pb) break;

        // release bus
        *pw_bus = 0x0000;

        // wait a bit (about 80 cycles)
        asm volatile ("\t\tmovm.l %d0-%d3,-(%sp)\n");
        asm volatile ("\t\tmovm.l (%sp)+,%d0-%d3\n");
    }

    // point to PENDING_FRM parameter
    pb = (u8 *) (Z80_DRV_PARAMS + 0x0F);
    // increment frame to process
    *pb += num;

    // release bus
    if (!busTaken) *pw_bus = 0x0000;
}

////////////////////////////////////////
//////////////////////// Z80_CTRL.C
///////////////////////////////////////

#include "config.h"
#include "types.h"

#include "z80_ctrl.h"

#include "ym2612.h"
#include "psg.h"
#include "memory.h"
#include "timer.h"
#include "sys.h"
#include "vdp.h"
#include "sound.h"
#include "xgm.h"

// Z80 drivers
#include "z80_drv0.h"
#include "z80_drv1.h"
#include "z80_drv2.h"
#include "z80_drv3.h"
#include "z80_xgm.h"

#include "tab_vol.h"
#include "smp_null.h"
#include "smp_null_pcm.h"


// we don't want to share it
extern vu16 VBlankProcess;

//u16 currentDriver;
u16 driverFlags;


// we don't want to share it
extern void XGM_resetLoadCalculation();


void Z80_init()
{
    // request Z80 bus
    Z80_requestBus(TRUE);
    // set bank to 0
    Z80_setBank(0);

    // no loaded driver
    currentDriver = -1;
    driverFlags = 0;

    // load null/dummy driver as it's important to have Z80 active (state is preserved)
    Z80_loadDriver(Z80_DRIVER_NULL, FALSE);
}


bool Z80_isBusTaken()
{
    vu16 *pw;

    pw = (u16 *) Z80_HALT_PORT;
    if (*pw & 0x0100) return FALSE;
    else return TRUE;
}

void Z80_requestBus(bool wait)
{
    vu16 *pw_bus;
    vu16 *pw_reset;

    // request bus (need to end reset)
    pw_bus = (u16 *) Z80_HALT_PORT;
    pw_reset = (u16 *) Z80_RESET_PORT;

    // take bus and end reset
    *pw_bus = 0x0100;
    *pw_reset = 0x0100;

    if (wait)
    {
        // wait for bus taken
        while (*pw_bus & 0x0100);
    }
}

bool Z80_getAndRequestBus(bool wait)
{
    vu16 *pw_bus;
    vu16 *pw_reset;

    pw_bus = (u16 *) Z80_HALT_PORT;

    // already requested ? just return TRUE
    if (!(*pw_bus & 0x0100)) return TRUE;

    pw_reset = (u16 *) Z80_RESET_PORT;

    // take bus and end reset
    *pw_bus = 0x0100;
    *pw_reset = 0x0100;

    if (wait)
    {
        // wait for bus taken
        while (*pw_bus & 0x0100);
    }

    return FALSE;
}

void Z80_releaseBus()
{
    vu16 *pw;

    pw = (u16 *) Z80_HALT_PORT;
    *pw = 0x0000;
}


void Z80_startReset()
{
    vu16 *pw;

    pw = (u16 *) Z80_RESET_PORT;
    *pw = 0x0000;
}

void Z80_endReset()
{
    vu16 *pw;

    pw = (u16 *) Z80_RESET_PORT;
    *pw = 0x0100;
}


void Z80_setBank(const u16 bank)
{
    vu8 *pb;
    u16 i, value;

    pb = (u8 *) Z80_BANK_REGISTER;

    i = 9;
    value = bank;
    while (i--)
    {
        *pb = value;
        value >>= 1;
    }
}

u8 Z80_read(const u16 addr)
{
    return ((vu8*) Z80_RAM)[addr];
}

void Z80_write(const u16 addr, const u8 value)
{
    ((vu8*) Z80_RAM)[addr] = value;
}


void Z80_clear(const u16 to, const u16 size, const bool resetz80)
{
    Z80_requestBus(TRUE);

    const u8 zero = getZeroU8();
    vu8* dst = (u8*) (Z80_RAM + to);
    u16 len = size;

    while(len--) *dst++ = zero;

    if (resetz80) Z80_startReset();
    Z80_releaseBus();
    // wait bus released
    while(Z80_isBusTaken());
    if (resetz80) Z80_endReset();
}

void Z80_upload(const u16 to, const u8 *from, const u16 size, const bool resetz80)
{
    Z80_requestBus(TRUE);

    // copy data to Z80 RAM (need to use byte copy here)
    u8* src = (u8*) from;
    vu8* dst = (u8*) (Z80_RAM + to);
    u16 len = size;

    while(len--) *dst++ = *src++;

    if (resetz80) Z80_startReset();
    Z80_releaseBus();
    // wait bus released
    while(Z80_isBusTaken());
    if (resetz80) Z80_endReset();
}

void Z80_download(const u16 from, u8 *to, const u16 size)
{
    bool busTaken = Z80_getAndRequestBus(TRUE);

    // copy data from Z80 RAM (need to use byte copy here)
    vu8* src = (u8*) (Z80_RAM + from);
    u8* dst = (u8*) to;
    u16 len = size;

    while(len--) *dst++ = *src++;

    if (!busTaken)
        Z80_releaseBus();
}


u16 Z80_getLoadedDriver()
{
    return currentDriver;
}

void Z80_unloadDriver()
{
    // already unloaded
    if (currentDriver == Z80_DRIVER_NULL) return;

    // clear Z80 RAM
    Z80_clear(0, Z80_RAM_LEN, TRUE);

    currentDriver = Z80_DRIVER_NULL;

    // remove XGM task if present
    VBlankProcess &= ~PROCESS_XGM_TASK;
}

void Z80_loadDriver(const u16 driver, const bool waitReady)
{
    const u8 *drv;
    u16 len;

    // already loaded
    if (currentDriver == driver) return;

    switch(driver)
    {
        case Z80_DRIVER_NULL:
            drv = z80_drv0;
            len = sizeof(z80_drv0);
            break;

        case Z80_DRIVER_PCM:
            drv = z80_drv1;
            len = sizeof(z80_drv1);
            break;

        case Z80_DRIVER_2ADPCM:
            drv = z80_drv2;
            len = sizeof(z80_drv2);
            break;

        case Z80_DRIVER_4PCM:
            drv = z80_drv3;
            len = sizeof(z80_drv3);
            break;

        case Z80_DRIVER_XGM:
            drv = z80_xgm;
            len = sizeof(z80_xgm);
            break;

        default:
            // no valid driver to load
            return;
    }

    // clear z80 memory
    Z80_clear(0, Z80_RAM_LEN, FALSE);
    // upload Z80 driver and reset Z80
    Z80_upload(0, drv, len, TRUE);

    // driver initialisation
    switch(driver)
    {
        vu8 *pb;
        u32 addr;

        case Z80_DRIVER_2ADPCM:
            // misc parameters initialisation
            Z80_requestBus(TRUE);
            // point to Z80 null sample parameters
            pb = (u8 *) (Z80_DRV_PARAMS + 0x20);

            addr = (u32) smp_null_pcm;
            // null sample address (128 bytes aligned)
            pb[0] = addr >> 7;
            pb[1] = addr >> 15;
            // null sample length (128 bytes aligned)
            pb[2] = sizeof(smp_null_pcm) >> 7;
            pb[3] = sizeof(smp_null_pcm) >> 15;
            Z80_releaseBus();
            break;

        case Z80_DRIVER_PCM:
            // misc parameters initialisation
            Z80_requestBus(TRUE);
            // point to Z80 null sample parameters
            pb = (u8 *) (Z80_DRV_PARAMS + 0x20);

            addr = (u32) smp_null;
            // null sample address (256 bytes aligned)
            pb[0] = addr >> 8;
            pb[1] = addr >> 16;
            // null sample length (256 bytes aligned)
            pb[2] = sizeof(smp_null) >> 8;
            pb[3] = sizeof(smp_null) >> 16;
            Z80_releaseBus();
            break;

        case Z80_DRIVER_4PCM:
            // load volume table
            Z80_upload(0x1000, tab_vol, 0x1000, 0);

            // misc parameters initialisation
            Z80_requestBus(TRUE);
            // point to Z80 null sample parameters
            pb = (u8 *) (Z80_DRV_PARAMS + 0x20);

            addr = (u32) smp_null;
            // null sample address (256 bytes aligned)
            pb[4] = addr >> 8;
            pb[5] = addr >> 16;
            // null sample length (256 bytes aligned)
            pb[6] = sizeof(smp_null) >> 8;
            pb[7] = sizeof(smp_null) >> 16;
            Z80_releaseBus();
            break;

        case Z80_DRIVER_XGM:
            // reset sound chips
            YM2612_reset();
            PSG_init();

            // misc parameters initialisation
            Z80_requestBus(TRUE);
            // point to Z80 sample id table (first entry = silent sample)
            pb = (u8 *) (0xA01C00);

            addr = (u32) smp_null;
            // null sample address (256 bytes aligned)
            pb[0] = addr >> 8;
            pb[1] = addr >> 16;
            // null sample length (256 bytes aligned)
            pb[2] = sizeof(smp_null) >> 8;
            pb[3] = sizeof(smp_null) >> 16;
            Z80_releaseBus();
            break;
    }

    // wait driver for being ready
    if (waitReady)
    {
        switch(driver)
        {
            // drivers supporting ready status
            case Z80_DRIVER_2ADPCM:
            case Z80_DRIVER_PCM:
            case Z80_DRIVER_4PCM:
            case Z80_DRIVER_XGM:
                Z80_releaseBus();
                // wait bus released
                while(Z80_isBusTaken());

                // just wait for it
                while(!Z80_isDriverReady())
                    waitMs(1);
                break;
        }
    }

    // new driver set
    currentDriver = driver;

    // post init stuff
    switch(driver)
    {
        // XGM driver
        case Z80_DRIVER_XGM:
            // using auto sync --> enable XGM task on VInt
            if (!(driverFlags & DRIVER_FLAG_MANUALSYNC_XGM))
                VBlankProcess |= PROCESS_XGM_TASK;
            // define default XGM tempo (always based on NTSC timing)
            XGM_setMusicTempo(60);
            // reset load calculation
            XGM_resetLoadCalculation();
            break;

        default:
            VBlankProcess &= ~PROCESS_XGM_TASK;
            break;
    }
}

void Z80_loadCustomDriver(const u8 *drv, u16 size)
{
    // clear z80 memory
    Z80_clear(0, Z80_RAM_LEN, FALSE);
    // upload Z80 driver and reset Z80
    Z80_upload(0, drv, size, TRUE);

    // custom driver set
    currentDriver = Z80_DRIVER_CUSTOM;

    // remove XGM task if present
    VBlankProcess &= ~PROCESS_XGM_TASK;
}

u16 Z80_isDriverReady()
{
    vu8 *pb;
    u8 ret;

    // point to Z80 status
    pb = (u8 *) Z80_DRV_STATUS;

    // bus already taken ? just check status
    if (Z80_isBusTaken())
        ret = *pb & Z80_DRV_STAT_READY;
    else
    {
        // take the bus, check status and release bus
        Z80_requestBus(TRUE);
        ret = *pb & Z80_DRV_STAT_READY;
        Z80_releaseBus();
    }

    return ret;
}