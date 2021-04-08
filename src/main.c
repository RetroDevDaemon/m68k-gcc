
#include "bentgen.h"

u32 frameCounter;
u16 vdpstat;
u32 hcount;

        
void main()
{
    WriteVDPRegister((u32)WRITE|REG(0)|0);
    WriteVDPRegister((u32)WRITE|REG(0x7)|1);
    WriteVDPRegister((u32)WRITE|REG(0xf)|2);
    
    SetVDPPlaneAddress(0, 0x2000);
    SetVDPPlaneAddress(1, 0x4000);
    SetVDPPlaneAddress(2, 0x6000);
    SetVDPPlaneAddress(3, 0x7000);
    
    SetVDPAddress(0x2000);
    
    while(1)
    { 
        WaitVBlank();
        frameCounter++;
        WaitHBlank();
        VDPStatus_u16(vdpstat);
        hcount++;
        
    }
}
