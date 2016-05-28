import processing.serial.*;

//Constants
final int VD = 633;
final int H = 100;

final int iwidth = 640;
final int iheight = 480;

final int scale = 5;
final int GB = 63;

Serial port;       

void setup() {
  // Open the port you are using at the rate you want:
  port = new Serial(this, "/dev/ttyUSB0", 115200);
  size(800,600);
  frameRate(10);
  
  loadPixels();
}

void draw() {
  for(int i = 0;i < width * height;i++) {
    pixels[i] = color(0, 0, 0);
  }
  
  for(int i = 0;i < height;i += 100 / scale) {
    for(int j = 0;j < width;j++) {
      pixels[j + (height - 1 - i) * width] = color(GB, 0, i == 0 ? GB : 0);
    }
  }
  for(int i = 0;i < width/2;i += 100 / scale) {
    for(int j = 0;j < height;j++) {
      pixels[(i + width/2) + j * width] = color(GB, 0, i == 0 ? GB : 0);
      pixels[(-i + width/2) + j * width] = color(GB, 0, i == 0 ? GB : 0);
    }
  }
  
  int i = 0;
  for(;;) {
    int input = port.read();
    if(input == -1) continue;
    if(input == 255) break;
    if(input != 254) {
      float value = input;
      float dpos = VD * H / value;
      float xpos = dpos * (i - iwidth / 2) / VD;
      
      xpos /= scale;
      dpos /= scale;
      
      if(xpos < -width/2) xpos = -width/2;
      if(xpos > width/2-1) xpos = width/2-1;
      
      if(dpos < 0) dpos = 0;
      if(dpos > height-1) dpos = height-1;
      
      pixels[((int)xpos + width/2) + (height - 1 - (int)dpos) * width] = color(0, 255, 0);
    }
    i++;
  }
  updatePixels();
}