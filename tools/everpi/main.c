#include <stdio.h>
#include <unistd.h> // sleep
#include <fcntl.h>  
#include <stdlib.h> // malloc

#define ED_FIFO_RAM 0x1810000
#define CMD_MEM_RD 0x19
#define CMD_MEM_WR 0x1a
#define CMD_FIFO_WR 0x23
#define CMD_OS_RESET 0x29
#define SOFT 1
#define HARD 2
#define OFF 0
typedef int bool;
#define true 1;
#define false 0;
#define TRUE true;
#define FALSE false;

#define MODE_WRITE_ROMFILE 0
#define MODE_MEMSET 1
#define MODE_MEMSET16 2
#define MODE_MEMSET32 3
#define MODE_MEMREAD 4
#define MODE_DUMPROM 5

char* rom_filename;

typedef unsigned char u8;
char buf[64];
int packet_length;
int ed_port;
char ptstr[] = "/dev/ttyACM0";
FILE* romfile;
char* filebuffer;
int filelen;


int txCmd(u8 cmd)
{
  char cb[4];
  cb[0] = '+';
  cb[1] = cb[0] ^ 0xff;
  cb[2] = cmd;
  cb[3] = cb[2] ^ 0xff;
  size_t r = write(ed_port, &cb[0], 4);
  return r;
}

void clear_buffer()
{
  for(int i = 0; i < 64; i++) buf[i] = 0;
}

int ed_reset(unsigned char mode)
{
  clear_buffer();
  txCmd(CMD_OS_RESET);
  buf[0] = mode;
  size_t r = write(ed_port, &buf[0], 1);
  return r;
}

int dump_rom(char* outfile)
{
  clear_buffer();
  char* hdat = (char*)malloc(0x200 * sizeof(char));
  int br = 0;
  for(int g = 0; g < 0x200; g += 32)
  {
    br = ed_read_mem(g, 32);
    if(br != 32){
      printf("read rom header failed\n");
      return -1;
    }
    for(int p = 0; p < 32; p++) 
      hdat[g + p] = buf[p];
  }
  int size = (hdat[0x1a4] << 24) |\
             (hdat[0x1a5] << 16) |\
             (hdat[0x1a6] << 8) |\
              hdat[0x1a7];
  char* rdat = (char*)malloc(size * sizeof(char));
  br = 0;
  for(int g = 0; g < size; g += 32)
  {
    br = ed_read_mem(g, 32);
    if (br != 32) {
      printf("read failed\n");
      return -1;
    }
    for(int p = 0; p < 32; p++) 
      rdat[g + p] = buf[p];
    
  }
  FILE* f = fopen(outfile, "wb");
  fwrite(rdat, 1, size, f);
  //if(pf != 0x80000) printf("failed write rom\n");
  fclose(f);
  printf("%s (%d bytes) written ok\n", outfile, size);
  printf("WARNING: Addresses 00h - FFh will not be read correctly!\n");
}

int ed_read_mem(int addr, int length)
{
  //txCMD // 4 bytes
  //  tx 8 0 // execute
  //  tx data
  clear_buffer();
  if(txCmd(CMD_MEM_RD) != 4) return -1;
  
  // addr32
  if(addr > 0xffffff) buf[4] = (unsigned char)(addr >> 24);
  if(addr > 0xffff) buf[5] = (unsigned char)(addr >> 16);//0x81;
  if(addr > 0xff) buf[6] = (unsigned char)(addr >> 8);//0;
  buf[7] = (unsigned char)(addr & 0xff);//0;
  int bw = write(ed_port, &buf[4], 4);
  if(bw != 4) return -1;

  // Size32
  buf[8] = (u8)(length >> 24);
  buf[9] = (u8)(length >> 16);
  buf[10] = (u8)(length >> 8);
  buf[11] = (u8)(length & 0xff);
  bw = write(ed_port, &buf[8], 4);
  if(bw != 4) return -1;
  
  buf[0] = 0;
  bw = write(ed_port, &buf[0], 1);
  int rb = read(ed_port, &buf, length);
  return rb;
}

int ed_write_fifo(char* data, unsigned int length)
{
  clear_buffer();
  txCmd(CMD_FIFO_WR);
  //addr16
  buf[5] = length & 0xff;
  buf[4] = (length & 0xff00) >> 8;
  write(ed_port, &buf[4], 2);
  for(int i = 0; i < length; i++)
    {
      buf[i] = *data++;
    }
  write(ed_port, &buf[0], length);
}

int ed_write_mem(int addr, char* data, unsigned int length)
{
  int i = 0;
  clear_buffer();
  txCmd(CMD_MEM_WR);
  // addr32
  buf[0] = (unsigned char)(addr >> 24);
  buf[1] = (unsigned char)(addr >> 16);//0x81;
  buf[2] = (unsigned char)(addr >> 8);//0;
  buf[3] = (unsigned char)(addr & 0xff);//0;
  // Size32
  buf[4] = (u8)(length >> 24);
  buf[5] = (u8)(length >> 16);
  buf[6] = (u8)(length >> 8);
  buf[7] = (u8)(length & 0xff);
  // execute byte
  buf[8] = '0'; 
  write(ed_port, &buf[0], 9);
  for(; i < length; i++)
    {
      buf[i] = *data++;
    }
  size_t r = write(ed_port, &buf, i);
  return r;
}


int get_error(int* fd)
{
  if(*fd == -1)
    {
      close(*fd);
      *fd = open(&ptstr, O_RDWR);
      if(*fd == -1)
      {
        close(*fd);
        printf("Error opening %s.\n", &ptstr);
        return 1;
      }
    }
  return 0;
}

void soft_reset();

void hard_reset()
{
  int bw;
  clear_buffer();
  bw = ed_reset(HARD);
  if (bw != 5) printf("hard reset error.\n");
  bw = ed_reset(OFF);
  //if (bw != 5) { printf("reset off error.\n"); soft_reset(); }
 
}

void soft_reset()
{
  int bw;
  clear_buffer();
  bw = ed_reset(SOFT);
  if (bw != 5) printf("soft reset error.\n");
  bw = ed_reset(OFF);
  //if (bw != 5) printf("reset off error.\n");
 
}
/*
void setup_port()
{
  tcgetattr(ed_port, &tty);
  tty.c_cflag &= ~PARENB; // parity off
  tty.c_cflag &= ~CSTOPB; // 1 stop bit
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS; // disable rts/cts
  //tty.c_cflag |= CREAD | CLOCAL; // read on, local ctrl
  tty.c_lflag &= ~ICANON; // non-canon, don't wait for newline
  // echo set if needed here
  tty.c_lflag &= ~(ECHO | ECHOE | ECHONL);
  tty.c_lflag &= ~ISIG; // disable signal interrupt characters
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // software flow ctl off
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR;
  tty.c_cc[VTIME] = 3; // 300ms timeout (taken from source)
  tty.c_cc[VMIN] = 0;
  //cfsetispeed(&tty, B19200);
  //cfsetospeed(&tty, B19200);
  tcsetattr(ed_port, TCSANOW, &tty);
}
*/

int ed_load_rom()
{
  int bw;
  clear_buffer();

  ed_reset(SOFT);
  printf("writing to cart\n");
  for(int fs = 0; fs < filelen; fs += 32)
    {
      bw = ed_write_mem(fs, &filebuffer[fs], 32);
      if(bw != 32) printf("WRITE ERROR\n");
      bw = ed_read_mem(fs, 32);
      while(bw != 32) {
        printf("READ ERROR\n");
        bw = ed_read_mem(fs, 32);
      }
      for(int j = 0; j < 32; j++){
        if(buf[j] != filebuffer[fs+j]) { printf("verify failed\n"); return -1; }
      }
      if(fs % (1024*64) == 0) { printf("."); fflush(0); }
    }

  ed_reset(OFF);

  u8 inp;
  read(ed_port, &inp, 1);
  printf("\nresponse code: %c\n", inp);
  if(inp != 'r') return -1;
  return 0;
}


void usage()
{
  printf("everpi\n\
  A Debian/Pi tool for Mega EverDrive development\n\n\
  USAGE:\n\
  $ everpi rom [romfile]\n\
   or\n\
  $ everpi memset|memset16|memset32 [address] [value]\n\
   or\n\
  $ everpi memread [address]\n\
   or\n\
  $ everpi dump [filename]\n\n\
\n\
  EXAMPLES:\n\
  'everpi rom rom.md' will load and launch a local romfile.\n\
  'everpi memset16 0x1512 0x0101' will set ROM memory $01512-13 to $0101.\n\
  'everpi memread 0x405' will print the four bytes at 0x405-0x408.\n\n\
  NOTES:\n\
   1. memset* and memread must be ran from the EverDrive interrupt menu!\n\
   2. memset*/memread do not work on SRAM ($ff0000+)\n\
   3. memread always reads 32 bits (4 bytes). memset and memset16 will only\n\
      use the last 8-16 bits of the operand.\n\
   4. 'ls /dev/*ACM*' will list USB modem devices. (Use port 0)\n\
   \nHave fun!\n\n");
}

int load_and_run_file()
{
  // kw_args[1] = file name
    romfile = fopen(rom_filename, "rb");
    fseek(romfile, 0, SEEK_END);
    filelen = ftell(romfile);
    printf("fsize %d\n",filelen);
    rewind(romfile);
    filebuffer = (char*)malloc(filelen * sizeof(char));
    fread(filebuffer, filelen, 1, romfile);
    fclose(romfile);
    if(ed_load_rom() == -1) {
      close(ed_port);
      return 1;
    }
    // cmd_tx FIFO_WR
    // len16
    // data
    char test_msg[3] = "*t";
    ed_write_fifo(&test_msg[0], 2);
    read(ed_port, &buf, 1);
    printf("response from *t: %c\n", buf[0]);
    
    // *g
    test_msg[1] = 'g';
    ed_write_fifo(&test_msg[0], 2);
    // 0x80000
    char leng[4] = { 0x0, 0x08, 0x00, 0x00 };
    ed_write_fifo(&leng[0], 4);
    // 6 chars: 'USB:/ '
    char stlen[2] = { 0x0, 0x6 };
    char addr[6] = "USB:/";
    ed_write_fifo(&stlen[0], 2);
    ed_write_fifo(&addr[0], 6);
    
    printf("Launching game...\n"); 
    return 0;
}

void read_memory(int memset_addr)
{
  int i = 0;
  i = ed_read_mem(memset_addr, 4);
  printf(" 0x%x%x%x%x\t|\n %d %d %d %d\t|\n %c %c %c %c \t\t(%d bytes read)\n", \
    buf[0], buf[1], buf[2], buf[3],\
    buf[0], buf[1], buf[2], buf[3],\
    buf[0], buf[1], buf[2], buf[3], i);
  if(i != 4) printf("probable fail, try again...\n");
}

void ed_memset(int memset_addr, int memset_val, u8 size)
{
    //ed_reset(OFF);
    printf("addr %d\n", memset_addr);
    printf("val%d %d | %c %c %c %c \n", \
      (size*8), memset_val, memset_val >> 24, memset_val >> 16, memset_val >> 8, memset_val & 0xff);
    char c;
    switch(size)
    {
      case(1):   
        c = memset_val;
        ed_write_mem(memset_addr, &c, 1);
        ed_read_mem(memset_addr, 1);
        if(buf[0] != c) printf("fail\n");
        else printf("mem set (1 byte) ok\n");
        break;
      case(2):
        for(int i = 1; i >= 0; i--)
        {
          c = (memset_val >> (8*i)) & 0xff;
          ed_write_mem(memset_addr + (1-i), &c, 1);
        }
        ed_read_mem(memset_addr, 2);
        if((buf[0] << 8 | buf[1]) != memset_val) printf("fail\n");
        else printf("mem set (2 bytes) ok\n");
        break;
      case(4):
        for(int i = 3; i >= 0; i--)
        {
          c = (memset_val >> (8*i)) & 0xff;
          ed_write_mem(memset_addr + (3-i), &c, 1);
        }
        ed_read_mem(memset_addr, 4);
        if((buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]) != memset_val) 
          printf("fail\n");
        else 
          printf("mem set (4 bytes) ok\n");
        break;
    }
    //ed_reset(SOFT);
  }

int main(int num_args, char** kw_args)
{

  if(num_args < 2) { usage(); return 1;}
  /* Parse arguments */
  int mode;
  int memset_addr;
  int memset_val;
  for(u8 b = 0; b < num_args; b++)
  {
    if(!strcmp(kw_args[1], "rom")) 
      mode = MODE_WRITE_ROMFILE;
    else if (!strcmp(kw_args[1], "memset")) 
      mode = MODE_MEMSET;
    else if (!strcmp(kw_args[1], "memset16")) 
      mode = MODE_MEMSET16;
    else if (!strcmp(kw_args[1], "memset32")) 
      mode = MODE_MEMSET32;
    else if (!strcmp(kw_args[1], "memread"))
      mode = MODE_MEMREAD;
    else if (!strcmp(kw_args[1], "dump"))
      mode = MODE_DUMPROM;
    else 
    { 
      usage();
      return 1; 
    }
  }
  /* Parse secondary arguments */
  if(mode == MODE_WRITE_ROMFILE || \
    (mode == MODE_DUMPROM)) 
    rom_filename = kw_args[2];
  else if ( (mode == MODE_MEMSET) || \
    (mode == MODE_MEMSET16) || \
    (mode == MODE_MEMSET32))
  {
    if(kw_args[2][1] != 'x') memset_addr = atoi(kw_args[2]);
    else memset_addr = strtol(kw_args[2], &kw_args[2] + sizeof(kw_args[2]), 16);
    //if(memset_addr >= 0xff0000) memset_addr += 0x10000; 
    if(kw_args[3][1] != 'x') memset_val = atoi(kw_args[3]);
    else memset_val = strtol(kw_args[3], &kw_args[3] + sizeof(kw_args[3]), 16);
  }
  else if ((mode == MODE_MEMREAD))
  {
    if(kw_args[2][1] != 'x') memset_addr = atoi(kw_args[2]);
    else memset_addr = strtol(kw_args[2], &kw_args[2] + sizeof(kw_args[2]), 16);
    printf("reading from addr %d\n", memset_addr);
    
  }
  else if ((mode == MODE_DUMPROM)){}
  else 
    return 1;

  /* Open Port */
  //u8 port = kw_args[2];  
  //ptstr[11] = port | 0x30;
  ed_port = open(&ptstr, O_RDWR);
  printf("open() on port: %s\n", &ptstr);
  if(get_error(&ed_port)) {
    close(ed_port);
    return 1;
  }
  /* Perform command */
  if(mode == MODE_WRITE_ROMFILE)
    load_and_run_file();
  else if (mode == MODE_MEMSET)
    ed_memset(memset_addr, memset_val, 1);
  else if (mode == MODE_MEMSET16)
    ed_memset(memset_addr, memset_val, 2);
  else if (mode == MODE_MEMSET32)
    ed_memset(memset_addr, memset_val, 4);
  else if (mode == MODE_MEMREAD) 
    read_memory(memset_addr);
  else if (mode == MODE_DUMPROM)
    dump_rom(rom_filename);
  close(ed_port);
  
}
