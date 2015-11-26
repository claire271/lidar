#include <stdio.h>
#include <unistd.h>
#include <bcm2835.h>
#include "camera.h"
#include "graphics.h"
#include <ncurses.h>
#include <sys/time.h>

#define MAIN_TEXTURE_WIDTH 512
#define MAIN_TEXTURE_HEIGHT 512

//should the camera convert frame data from yuv to argb automatically?
#define DO_ARGB_CONVERSION true
//how many detail levels (1 = just the capture res, > 1 goes down by half each level, 4 max)
#define NUM_LEVELS 4

#define FPS 30

#define PIN RPI_GPIO_P1_11

void revokeRoot();

int main(int argc, const char **argv)
{
  bool laser_state = false;

  //Init the laser output
  if(!bcm2835_init())
    return 1;
  revokeRoot();

  bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);

  //Init ncurses
  initscr();      /* initialize the curses library */
  keypad(stdscr, TRUE);  /* enable keyboard mapping */
  nonl();         /* tell curses not to do NL->CR/NL on output */
  cbreak();       /* take input chars one at a time, no wait for \n */
  clear();
  nodelay(stdscr, TRUE);

  //init graphics and the camera
  InitGraphics();
  CCamera* cam = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,FPS,NUM_LEVELS,DO_ARGB_CONVERSION);

  //create 4 textures of decreasing size
  GfxTexture textures[4];
  for(int texidx = 0; texidx < NUM_LEVELS; texidx++)
    textures[texidx].Create(MAIN_TEXTURE_WIDTH >> texidx, MAIN_TEXTURE_HEIGHT >> texidx);

  struct timeval time;
  gettimeofday(&time, NULL);
  unsigned long old_time = time.tv_sec * 1000000 + time.tv_usec;
  unsigned long new_time;
  int pos = 0;

  printf("Running frame loop\n");
  for(int i = 0; i < 3000; i++) {
    //pick a level to read based on current frame (flicking through them every 'FPS' frames)
    //int texidx = (i / FPS)%NUM_LEVELS;
    int texidx = 0;

    //lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
    const void* frame_data; int frame_sz;
    if(cam->BeginReadFrame(texidx,frame_data,frame_sz)) {
      //if doing argb conversion the frame data will be exactly the right size so just set directly
      textures[texidx].SetPixels(frame_data);

      cam->EndReadFrame(texidx);
      bcm2835_gpio_write(PIN, laser_state ? HIGH : LOW);
      laser_state = !laser_state;
    }

    //begin frame, draw the texture then end frame (the bit of maths just fits the image to the screen while maintaining aspect ratio)
    BeginFrame();
    float aspect_ratio = float(MAIN_TEXTURE_WIDTH)/float(MAIN_TEXTURE_HEIGHT);
    float screen_aspect_ratio = 1280.f/720.f;
    DrawTextureRect(&textures[texidx],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
    EndFrame();

    gettimeofday(&time, NULL);
    new_time = time.tv_sec * 1000000 + time.tv_usec;
    //mvprintw(0,0,"uS/Frame: %i\n",new_time - old_time);
    int max = (new_time - old_time) / 2000;
    old_time = new_time;

    int j = 0;
    for(;j < max;j++) {
      mvprintw(LINES - j,pos,"#");
    }
    for(;j < LINES;j++) {
      mvprintw(LINES - j,pos," ");
    }
    pos++;
    pos %= COLS;
    for(j = 0;j < LINES;j++) {
      mvprintw(LINES - j,pos,"|");
    }
    mvprintw(0,0,"CURRENT mS/fame: %i",max);
    refresh();
  }

  StopCamera();
}

void revokeRoot()
{
  if (getuid () + geteuid () == 0)      // Really running as root
    return ;

  if (geteuid () == 0)                  // Running setuid root
    seteuid (getuid ()) ;               // Change effective uid to the uid of the caller
}
