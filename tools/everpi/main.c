#include <stdio.h>
#include <unistd.h> // sleep
#include <fcntl.h>  
#include <string.h>
#include <stdlib.h> // malloc
#include <termios.h>
#include <errno.h>

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
#define MODE_COPYFILE 6

char* rom_filename;

typedef unsigned char u8;
char buf[64];
int packet_length;
int ed_port;
char ptstr[] = "/dev/tty.usbserial-1330";
FILE* romfile;
char* filebuffer;
int filelen;

struct termios tty;

int ed_create_file(char rm[], int size);
int get_status();

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

void tx8(int arg)
{
  u8 buff[1];// = new u8[1];
  buff[0] = (u8)(arg);
  size_t t = write(ed_port, &buff[0], sizeof(buff));
  if (t != sizeof(buff))
  {
    printf("tx8 failed\n");
  }
}

void tx16(int arg)
{
  u8 buff[2];// = new u8[1];
  buff[0] = (u8)(arg>>8);
  buff[1] = (u8)(arg & 0xff);
  size_t t = write(ed_port, &buff[0], sizeof(buff));
  if (t != sizeof(buff))
  {
    printf("tx16 failed\n");
  }
}

void tx32(int arg)
{
  u8 buff[4];// = new u8[1];
  buff[0] = (u8)(arg>>24);
  buff[1] = (u8)(arg>>16);
  buff[2] = (u8)(arg>>8);
  buff[3] = (u8)(arg & 0xff);
  
  size_t t = write(ed_port, &buff[0], sizeof(buff));
  if (t != sizeof(buff))
  {
    printf("tx32 failed\n");
  }
}

int ed_reset(unsigned char mode);

int copy_file()
{
  romfile = fopen(rom_filename, "rb");
  fseek(romfile, 0, SEEK_END);
  filelen = ftell(romfile);
  printf("fsize %d\n",filelen);
  rewind(romfile);
  filebuffer = (char*)malloc(filelen * sizeof(char));
  fread(filebuffer, filelen, 1, romfile);
  fclose(romfile);

  if(ed_create_file("TEST.MD", 7) == 1)
    {
      printf("Failed - try again?\n");
      return -1;
    }
  txCmd(0xcc); // write file data
  tx32(filelen);
  int offset = 0;
  int len = filelen;
  int ctr = 10;
  printf("Copying as TEST.MD");
  while (len > 0)
  {
    // Progress dot
      ctr--;
      if (ctr==0){
        printf(".");
        fflush(0);
        ctr = 32;
      }
      
      read(ed_port, &buf, 1);
      if (buf[0] != 0) {
        printf("\nexception %d!\n", buf[0]);
        return -1;
      }

      int block = 1024;
      if (block > len) block = len;

      write(ed_port, &filebuffer[0] + offset, block);

      len -= block;
      offset += block;

  }
  get_status();
    
  txCmd(0xce); // close file
  get_status();

  printf("Done. \nRestarting everdrive.\n");

  ed_reset(SOFT);
  ed_reset(OFF);

  return 0;
}

int get_status()
{
// get status 
  txCmd(0x10); 
  size_t t = read(ed_port, &buf, 2);
  if(t != 2) {
    printf("get status failed\n");
    return 1;
  }
  //printf("state ok.");
  if (buf[1] != 0) printf("get state failed. you should quit.\n");
  return 0;
}

void txStr(char str[], int strlen)
{
  tx16(strlen);
  size_t t = write(ed_port, &str[0], strlen);
  if(t != strlen) printf("txstr failed\n");
}

int ed_create_file(char rm[], int strlen)
{
  txCmd(0xc9); // open file
  tx8(0x08|0x02); // write, FAT
  // then txstr filename
  txStr(rm, strlen);
  int r = get_status(); // make sure OK
  if (r == 1)
    return 1;
  else 
    return 0;
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

int get_error(int* fd)
{
  while(*fd == -1)
    {
      close(*fd);
      *fd = open((const char*)&ptstr, O_RDWR);
      if(*fd == -1)
      {
        close(*fd);
        printf("Error: %s (%d). ", strerror(errno), errno);
        if(errno == 9) printf("Try: chmod +777 %s\n", (const char*)&ptstr);
        printf("\nError opening %s. Quitting...\n", (const char*)&ptstr);
        return 1;
      }
      //sleep(3);
    }
  return 0;
}

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
  //tty.c_cc[VTIME] = 3; // 300ms timeout (taken from source)
  //tty.c_cc[VMIN] = 0;

  tcsetattr(ed_port, TCSANOW, &tty);
}



void usage()
{
  printf("everpi lite\n\
  A Linux tool for Mega EverDrive development\n\n\
  USAGE:\n\
  $ everpi copy [romfile]\n\
  \n\
  Copies local romfile to the Everdrive's SD card root.\n\
  Note: 'ls /dev/*ACM*' will list USB modem devices. (Use port 0)\n\
   Have fun!\n\n");
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
    if (!strcmp(kw_args[1], "copy"))
      mode = MODE_COPYFILE;
    else 
    { 
      usage();
      return 1; 
    }
  }
  /* Parse secondary arguments */
  if(mode == MODE_WRITE_ROMFILE || \
    (mode == MODE_DUMPROM) || \
    (mode == MODE_COPYFILE))
    rom_filename = kw_args[2];
  else 
    return 1;

  /* Open Port */
  ed_port = open((const char*)&ptstr, O_RDWR);
  setup_port(); // uses ed_port 
  printf("open() on port: %s\n", &ptstr);
  if(get_error(&ed_port)) {
    close(ed_port);
    return 1;
  }
  
  /* Perform command */
  if (mode == MODE_COPYFILE)
  {
    copy_file();
  }
  close(ed_port);
  
}
