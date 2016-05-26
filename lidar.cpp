#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <bcm2835.h>
#include "camera.h"
#include "graphics.h"
#include <ncurses.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include "config.h"

//should the camera convert frame data from yuv to argb automatically?
#define DO_ARGB_CONVERSION true
//how many detail levels (1 = just the capture res, > 1 goes down by half each level, 4 max)
#define NUM_LEVELS 1

//raw FPS of the camera
#define FPS 30
#define laser_freq FPS / 3
#define laser_duty_cycle .4

//laser control pin
#define PIN RPI_GPIO_P1_11

//distance to virtual camera plane (px)
#define VD 633

//physical height from camera to laser (mm)
#define H 100

//output serial port
#define PORT "/dev/ttyAMA0" 

//buffer for serial output
unsigned char buf[2];

void revokeRoot();
void cleanup();
void catch_SIGINT(int sig);
void* laserloop(void *arg);

bool need_cleanup = false;

GLubyte data_buf[CAMERA_WIDTH * CAMERA_HEIGHT * 4];
GLubyte max_value[CAMERA_WIDTH];
GLshort max_index[CAMERA_WIDTH];

short xpos[CAMERA_WIDTH];
short dpos[CAMERA_WIDTH];
int pcount;

GLubyte out_tex_buf[CAMERA_WIDTH * CAMERA_HEIGHT * 4];

unsigned short binToHex(unsigned char value);
unsigned char hexToBin(unsigned short value);

//FILE* out_fd;

int main(int argc, const char **argv)
{
  //Init the laser output
  if(!bcm2835_init())
    return 1;
  revokeRoot();

  signal(SIGINT, catch_SIGINT);

  //Setting up threads
  pthread_t laserthread;
  pthread_create(&laserthread, NULL, &laserloop, NULL);

  //Setting up serial output
  struct termios tio;
  int tty_fd;

  memset(&tio,0,sizeof(tio));
  tio.c_iflag=0;
  tio.c_oflag=0;
  tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
  tio.c_lflag=0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=5;
  
  tty_fd=open(PORT, O_RDWR | O_NONBLOCK);      
  cfsetospeed(&tio,B115200);            // 115200 baud
  cfsetispeed(&tio,B115200);            // 115200 baud
  
  tcsetattr(tty_fd,TCSANOW,&tio);

  char c = ' ';

  //Setting up file output
  //out_fd = fopen("out.txt","a+");

  //init graphics and the camera
  InitGraphics();
  CCamera* cam = StartCamera(CAMERA_WIDTH, CAMERA_HEIGHT,FPS,NUM_LEVELS,DO_ARGB_CONVERSION);

  GfxTexture textures[4];
  for(int i = 0;i < 4;i++) {
    textures[i].Create(CAMERA_WIDTH,CAMERA_HEIGHT);
  }

  for(int i = 0;i < CAMERA_HEIGHT;i++) {
    for(int j = 0;j < CAMERA_WIDTH;j++) {
      out_tex_buf[((i * CAMERA_WIDTH) + j) * 4 + 0] = 0;
      out_tex_buf[((i * CAMERA_WIDTH) + j) * 4 + 1] = 0;
      out_tex_buf[((i * CAMERA_WIDTH) + j) * 4 + 2] = 0;
      out_tex_buf[((i * CAMERA_WIDTH) + j) * 4 + 3] = 0;
    }
  }

  struct timeval time;
  gettimeofday(&time, NULL);
  unsigned long old_time = time.tv_sec * 1000000 + time.tv_usec;
  unsigned long new_time;
  int pos = 0;
  float uspf = 33;

  //Init ncurses
  initscr();      /* initialize the curses library */
  keypad(stdscr, TRUE);  /* enable keyboard mapping */
  nonl();         /* tell curses not to do NL->CR/NL on output */
  cbreak();       /* take input chars one at a time, no wait for \n */
  clear();
  nodelay(stdscr, TRUE);

  unsigned char cur_frame = 0;

  printf("Running frame loop\n");
  for(;!need_cleanup;) {

    int down_sample = 0;

    //spin until we have a camera frame
    const void* frame_data; int frame_sz;
    while(!cam->BeginReadFrame(0,frame_data,frame_sz)) {};

    textures[cur_frame].SetPixels(frame_data);
    cur_frame++;
    cur_frame %= 3;
    
    cam->EndReadFrame(down_sample);

    if(cur_frame == 0) {
      //begin frame, draw the texture then end frame
      BeginFrame();
      DrawTextureRect(textures,(GLvoid*)data_buf);

      //Finding the max position and value
      for(int j = 0;j < CAMERA_WIDTH;j++) {
        max_value[j] = 0;
        max_index[j] = -1;
      }
      for(int i = 0;i < CAMERA_HEIGHT / 2;i++) {
        for(int j = 0;j < CAMERA_WIDTH;j++) {
          GLubyte value = data_buf[((i * CAMERA_WIDTH) + j) * 4];
          if(value > max_value[j]) {
            max_value[j] = value;
            max_index[j] = CAMERA_HEIGHT - i - 1;
          }
        }
      }

      //Test print out
      mvprintw(1,0,"0x%X 0x%X 0x%X 0x%X",((unsigned char*)(data_buf + 500))[0],
               ((unsigned char*)(data_buf + 500))[1],
               ((unsigned char*)(data_buf + 500))[2],
               ((unsigned char*)(data_buf + 500))[3]);

      //Displaying the found value if it is above a threshold
      //Also calculating actual positions
//if (read(tty_fd,&c,1)>0)  write(tty_fd,&c,1);
//dprintf(tty_fd,"1");
      pcount = 0;
      for(int j = 0;j < CAMERA_WIDTH;j++) {
        if(max_value[j] > 14) {
          out_tex_buf[((max_index[j] * CAMERA_WIDTH) + j) * 4 + 1] = 255;

          int output = max_index[j] - CAMERA_HEIGHT / 2;
          if(output > 254) output = 254;

          buf[0] = 0;
          buf[1] = output;
          write(tty_fd,buf,2);
          
          //Calculating actual position
          //dpos[pcount] = VD * H * 1.0f / (max_index[j] - CAMERA_HEIGHT / 2);
          //xpos[pcount] = dpos[pcount] * (j - CAMERA_WIDTH / 2) / VD;
          //pcount++;

          //Writing out position to serial port
          //dprintf(tty_fd,"%i %i ",(int)xpos[pcount - 1],(int)dpos[pcount - 1]);

          //write(tty_fd,buf,2);
          
          //fprintf(out_fd,"%i ",max_index[j]);
        }
        else {
          //fprintf(out_fd,"-1 ");
          buf[0] = 255;
          buf[1] = 254;
          write(tty_fd,buf,2);
        }
      }
      textures[3].SetPixels(out_tex_buf);
      for(int j = 0;j < CAMERA_WIDTH;j++) {
          out_tex_buf[((max_index[j] * CAMERA_WIDTH) + j) * 4 + 1] = 0;
      }
      //fprintf(out_fd,"\n");
      //dprintf(tty_fd,"\n");
      buf[0] = 255;
      buf[1] = 255;
      write(tty_fd,buf,2);

      EndFrame();
    }
     
    gettimeofday(&time, NULL);
    new_time = time.tv_sec * 1000000 + time.tv_usec;
    //mvprintw(0,0,"uS/Frame: %i\n",new_time - old_time);
    int max = (new_time - old_time) / 1000;
    old_time = new_time;

    mvprintw(0,0,"CURRENT fps: %.2f",1000.0 / (uspf = (uspf*.99 + max*.01)));
    refresh();
  }

  endwin();
  StopCamera();
  //fclose(out_fd);
  close(tty_fd);
  return 0;
}

void* laserloop(void *arg) {
  //Setting up timer stuff
  struct timeval tv;
  gettimeofday(&tv,NULL);
  unsigned long old_time = 1000000 * tv.tv_sec + tv.tv_usec;
  unsigned long init_time = old_time;

  //Setting laser pin as output
  bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);

  bool laser_state = false;

  for(;!need_cleanup;) {
    laser_state = !laser_state;
    bcm2835_gpio_write(PIN, laser_state ? HIGH : LOW);

    //Timer loop code
    gettimeofday(&tv,NULL);
    unsigned long cur_time = 1000000 * tv.tv_sec + tv.tv_usec;
    unsigned long time_diff = 1000000L/(laser_freq) * (laser_state ? laser_duty_cycle : (1 - laser_duty_cycle));
    if(old_time + time_diff > cur_time) {
      usleep(old_time + time_diff - cur_time);
    }
    old_time += time_diff;
  }
  bcm2835_gpio_write(PIN, LOW);
  return 0;
}

void cleanup() {
}

void catch_SIGINT(int sig) {
  need_cleanup = true;
}

void revokeRoot()
{
  if (getuid () + geteuid () == 0)      // Really running as root
    return ;

  if (geteuid () == 0)                  // Running setuid root
    seteuid (getuid ()) ;               // Change effective uid to the uid of the caller
}

//In these two functions, the two ascii values are crammed into the short
unsigned short binToHex(unsigned char value) {
  unsigned char top = value >> 4;
  unsigned char bottom = value & 0x0F;
  unsigned short result;
  ((char*)(&result))[1] = ((top <= 9) ? (top + '0') : (top + 'A' - 0x0A));
  ((char*)(&result))[0] = ((bottom <= 9) ? (bottom + '0') : (bottom + 'A' - 0x0A));
  return result;
}

unsigned char hexToBin(unsigned short value) {
  unsigned char top = value >> 8;
  unsigned char bottom = value & 0xFF;
  return (((top <= '9') ? (top - '0') : (top - 'A' + 0x0A)) << 4) |
    ((bottom <= '9') ? (bottom - '0') : (bottom - 'A' + 0x0A));
}
