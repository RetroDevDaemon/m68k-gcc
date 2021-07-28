#include <stdio.h>
#include <unistd.h> // sleep
#include <fcntl.h>  
#include <stdlib.h> // malloc
#include <termios.h>

#define ED_FIFO_RAM 0x1810000
#define CMD_MEM_RD 0x19
#define CMD_MEM_WR 0x1a
#define CMD_FIFO_WR 0x23
#define CMD_OS_RESET 0x29
#define SOFT 1
#define HARD 2
#define OFF 0

typedef unsigned char u8;
char buf[64];
int packet_length;
int ed_port;
//char msg_snd_cmd[] = "Sending command: hostReset(1)";
FILE* romfile;
char* filebuffer;
//char* filebuffer2;
int filelen;

struct termios tty;

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

int tx16(int val)
{
  buf[0] = (unsigned char)(val >> 8);//0;
  buf[1] = (unsigned char)(val & 0xff);//0;
  int bw = write(ed_port, &buf[0], 2);
  if(bw != 2) return 0;
  return 1;
}

int tx32(int addr)
{
  buf[0] = (unsigned char)(addr >> 24);
  buf[1] = (unsigned char)(addr >> 16);//0x81;
  buf[2] = (unsigned char)(addr >> 8);//0;
  buf[3] = (unsigned char)(addr & 0xff);//0;
  int bw = write(ed_port, &buf[0], 4);
  if(bw != 4) return 0;
  return 1;
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

int ed_read_mem(int addr, int length)
{
  //txCMD // 4 bytes
  //  tx 8 0 // execute
  //  tx data
  clear_buffer();
  if(txCmd(CMD_MEM_WR) != 4) return -1;
  // addr32
  if(!tx32(addr)) return -1;
  // Size32
  if(!tx32(length)) return -1;

  buf[12] = 0;
  int bw = write(ed_port, &buf[12], 1);
  int rb = read(ed_port, &buf, length);
  return rb;
}

int ed_write_fifo(char* data, unsigned int length)
{
  clear_buffer();
  txCmd(CMD_FIFO_WR);
  //addr16
  tx16(length);
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
  txCmd(0x1a);
  // addr32
  tx32(addr);
  // Size32
  tx32(length);

  // execute byte
  buf[12] = '0'; 
  write(ed_port, &buf[12], 1);
  for(i; i < length; i++)
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
      *fd = open("/dev/ttyACM0", O_RDWR);
      if(*fd == -1)
      {
        close(*fd);
        printf("Error opening ACM0.\n");
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
  if (bw != 5) printf("reset off error.\n");
 
}

#define true 1
#define false 0
typedef int bool;

int load_rom_file()
{
  int bw;
  clear_buffer();
  u8 inp;
  
  ed_reset(SOFT);
  bool retry = true;
  printf("writing to cart");
  while(retry){
    retry = false;
    for(int fs = 0; fs < filelen; fs += 32)
      {
        bw = ed_write_mem(fs, &filebuffer[fs], 32);
        if(bw != 32) printf("WRITE ERROR\n");
        bw = ed_read_mem(fs, 32);
        while(bw != 32) { //printf("retrying read..\n");
          bw = ed_read_mem(fs, 32);
        }
        for(int j = 0; j < 32; j++){
          // verify
          if(buf[j] != filebuffer[fs+j]) {
            //printf("verify fail at %d\n",(fs+j));
            retry = true;
            //j = 99;
            read(ed_port, &inp, 1);
          }
        }
        if(fs % (1024*64) == 0) { printf("."); fflush(0); }
      }
      
  }
    /*
  printf("\nverifying");
  for(int fs = 0; fs < filelen; fs += 32)
    {
      bw = ed_read_mem(fs, 32);
      while(bw != 32) { //printf("retrying read..\n");
	      bw = ed_read_mem(fs, 32);
      }
      for(int j = 0; j < 32; j++){
        // verify
        if(buf[j] != filebuffer[fs+j]) {
          printf("verify fail at %d\n",(fs+j));
          return -1;
        }
      }
      if(fs%(1024*64) == 0) { printf("."); fflush(0); }
    }
  */
  ed_reset(OFF);

  read(ed_port, &inp, 1);
  printf("\nresponse code: %c\n", inp);
  if(inp != 'r') return -1;
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
  tty.c_cc[VTIME] = 3; // 300ms timeout (taken from source)
  tty.c_cc[VMIN] = 0;
  //cfsetispeed(&tty, B19200);
  //cfsetospeed(&tty, B19200);
  tcsetattr(ed_port, TCSANOW, &tty);
}
/*
An important point to note is that VTIME means slightly different
 things depending on what VMIN is. When VMIN is 0, VTIME specifies a 
 time-out from the start of the read() call. But when VMIN is > 0, 
 VTIME specifies the time-out from the start of the first received character.

Let’s explore the different combinations:

VMIN = 0, VTIME = 0: No blocking, return immediately with what is available

VMIN > 0, VTIME = 0: This will make read() always wait for bytes 
(exactly how many is determined by VMIN), so read() could block indefinitely.

VMIN = 0, VTIME > 0: This is a blocking read of any number chars with 
a maximum timeout (given by VTIME). read() will block until either any amount of 
data is available, or the timeout occurs. This happens to be my favourite mode 
(and the one I use the most).

VMIN > 0, VTIME > 0: Block until either VMIN characters have been received, or 
VTIME after first character has elapsed. Note that the timeout for VTIME does not 
begin until the first character is received.
*/
int main(int num_args, char** kw_args)
{
  ed_port = open("/dev/ttyACM1", O_RDWR);
  printf("fopen() on port: %d\n", ed_port);
  if(get_error(&ed_port)) return 1;

  // Proper port setup:
  setup_port();
  
  if(num_args > 0)
    {
      for(int i = 0; i < num_args; i++) printf("%s\n",kw_args[i]);
    }
  if(num_args < 2) return -1;
  
  romfile = fopen(kw_args[1], "rb");
  fseek(romfile, 0, SEEK_END);
  filelen = ftell(romfile);
  printf("fsize %d\n",filelen);
  rewind(romfile);
  filebuffer = (char*)malloc(filelen * sizeof(char));
  fread(filebuffer, filelen, 1, romfile);
  fclose(romfile);

  
  if(load_rom_file() == -1) return 1;
  // FIFO STUFF:
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
  
  close(ed_port);
  
}
