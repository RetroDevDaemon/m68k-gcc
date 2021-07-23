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

typedef unsigned char u8;
char buf[64];
int packet_length;
int ed_port;
char msg_snd_cmd[] = "Sending command: hostReset(1)";
FILE* romfile;
char* filebuffer;
char* filebuffer2;
int filelen;

struct packet
{
  
} pack;


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

int ed_read_mem(int addr, int length)
{
  //txCMD // 4 bytes
  //  tx 8 0 // execute
  //  tx data
  clear_buffer();
  int i = 0;
  if(txCmd(0x19) != 4) return -1;
  
  // addr32
  buf[4] = (unsigned char)(addr >> 24);
  buf[5] = (unsigned char)(addr >> 16);//0x81;
  buf[6] = (unsigned char)(addr >> 8);//0;
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
  
  buf[12] = 0;
  bw = write(ed_port, &buf[12], 1);
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
  txCmd(0x1a);
  // addr32
  buf[4] = (unsigned char)(addr >> 24);
  buf[5] = (unsigned char)(addr >> 16);//0x81;
  buf[6] = (unsigned char)(addr >> 8);//0;
  buf[7] = (unsigned char)(addr & 0xff);//0;
  // Size32
  buf[8] = (u8)(length >> 24);
  buf[9] = (u8)(length >> 16);
  buf[10] = (u8)(length >> 8);
  buf[11] = (u8)(length & 0xff);
  // execute byte
  buf[12] = '0'; 
  write(ed_port, &buf[4], 9);
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

int load_rom_file()
{
  int bw;
  clear_buffer();

  ed_reset(SOFT);
  printf("writing to cart");
  for(int fs = 0; fs < filelen; fs += 32)
    {
      bw = ed_write_mem(fs, &filebuffer[fs], 32);
      if(bw != 32) printf("WRITE ERROR\n");
      if(fs % (1024*64) == 0) { printf("."); fflush(0); }
    }
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

  ed_reset(OFF);

  u8 inp;
  read(ed_port, &inp, 1);
  printf("\nresponse code: %c\n", inp);
  if(inp != 'r') return -1;
  return 0;
}

int main(int num_args, char** kw_args)
{
  char b;
  ed_port = open("/dev/ttyACM0", O_RDWR);
  printf("fopen() on port: %d\n", ed_port);
  if(get_error(&ed_port)) return 1;

  
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
