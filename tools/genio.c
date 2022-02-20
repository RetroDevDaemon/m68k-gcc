#include <stdio.h>
#include <unistd.h> // sleep
#include <fcntl.h>  
#include <string.h>
#include <stdlib.h> // malloc
#include <termios.h>
#include <errno.h>

int get_error(int* fd);
int main(int num_args, char** kw_args);
void setup_port(int port);


struct termios tty;
int out_port;
int in_port;
char outportstr[] = "/dev/cu.usbserial-1330\000";
//char inportstr[] =  "/dev/tty.usbserial-1330\000";

int get_error_outport(int* fd)
{
  while(*fd == -1)
    {
      close(*fd);
      *fd = open((const char*)&outportstr, O_WRONLY);
      if(*fd == -1)
      {
        close(*fd);
        printf("Error: %s (%d). ", strerror(errno), errno);
        if(errno == 9) printf("Try: chmod +777 %s\n", (const char*)&outportstr);
        printf("\nError opening %s. Quitting...\n", (const char*)&outportstr);
        return 1;
      }
      //sleep(3);
    }
  return 0;
}


void setup_port(int port)
{
  tcgetattr(port, &tty);
  tty.c_cflag &= ~PARENB; // parity off
  tty.c_cflag &= ~CSTOPB; // 1 stop bit
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS; // disable rts/cts
  tty.c_cflag |= CREAD | CLOCAL; // read on, local ctrl
  tty.c_lflag &= ~ICANON; // non-canon, don't wait for newline
  // echo set if needed here
  tty.c_lflag &= ~(ECHO | ECHOE | ECHONL);
  tty.c_lflag &= ~ISIG; // disable signal interrupt characters
  //tty.c_iflag &= ~(IXON | IXOFF | IXANY); // software flow ctl off
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR;
  //tty.c_cc[VTIME] = 3; // 300ms timeout (taken from source)
  //tty.c_cc[VMIN] = 0;
    cfsetispeed(&tty, B4800);
    cfsetospeed(&tty, B4800);

  tcsetattr(port, TCSANOW, &tty);
}

int main(int num_args, char** kw_args)
{
  /* Open Write Port */
  printf("trying to open() RDWR on port: %s\n", &outportstr);
  out_port = open((const char*)&outportstr, O_RDWR);
  close(out_port);
  out_port = open((const char*)&outportstr, O_RDWR);
  if(get_error_outport(&out_port)) {
    close(out_port);
    printf("error opening\n");
    return 1;
  }
  setup_port(out_port); 
  

    while(1)
    {
        char c = ' ';
        read(out_port, &c, 1);
        if(c != ' ')
            printf("%c\n");
    }

  close(out_port);
  printf("OK. closing.\n");
  
}