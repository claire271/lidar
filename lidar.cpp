#include <stdio.h>
#include <unistd.h>
#include <bcm2835.h>
#include "camera.h"
#include "graphics.h"
#include <ncurses.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

//should the camera convert frame data from yuv to argb automatically?
#define DO_ARGB_CONVERSION true
//how many detail levels (1 = just the capture res, > 1 goes down by half each level, 4 max)
#define NUM_LEVELS 1

#define FPS 30 
#define laser_freq FPS / 3
#define laser_duty_cycle .4

#define PIN RPI_GPIO_P1_11

void revokeRoot();
void cleanup();
void catch_SIGINT(int sig);
void* laserloop(void *arg);

bool need_cleanup = false;

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

  //init graphics and the camera
  InitGraphics();
  CCamera* cam = StartCamera(CAMERA_WIDTH, CAMERA_HEIGHT,FPS,NUM_LEVELS,DO_ARGB_CONVERSION);

  GfxTexture textures[3];
  for(int i = 0;i < 3;i++) {
    textures[i].Create(CAMERA_WIDTH,CAMERA_HEIGHT);
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
      //begin frame, draw the texture then end frame (the bit of maths just fits the image to the screen while maintaining aspect ratio)
      BeginFrame();
      float aspect_ratio = float(CAMERA_WIDTH)/float(CAMERA_HEIGHT);
      float screen_aspect_ratio = 1280.f/720.f;
      DrawTextureRect(textures,-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
      EndFrame();
    }
     
    gettimeofday(&time, NULL);
    new_time = time.tv_sec * 1000000 + time.tv_usec;
    //mvprintw(0,0,"uS/Frame: %i\n",new_time - old_time);
    int max = (new_time - old_time) / 1000;
    old_time = new_time;

    mvprintw(0,0,"CURRENT mS/fame: %f",uspf = (uspf*.5 + max*.5));
    refresh();
  }

  endwin();
  StopCamera();
  return 0;
}

void* laserloop(void *arg) {
  //Setting up timer stuff
  struct timeval tv;
  gettimeofday(&tv,NULL);
  unsigned long old_time = 1000000 * tv.tv_sec + tv.tv_usec;

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
