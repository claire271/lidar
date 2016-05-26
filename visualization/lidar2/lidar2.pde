import processing.serial.*;

//Constants
final int VD = 633;
final int H = 100;

final int iwidth = 640;
final int iheight = 480;

final int scale = 2;
final int GB = 63;

Serial port;

int prev = 0x00;

void setup() {
  // Open the port you are using at the rate you want:
  port = new Serial(this, "/dev/ttyUSB0", 115200);
  size(800,600);
  frameRate(9);
  
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
  
  prev = 0x00;
  for(int j = 0;j < iwidth * 2 + 10;j++) {
    while(port.available() == 0); //print("w");
    //print("t");
    int input = port.read();
    if(prev == 255 && input == 255) {
      //print("o");
      break;
    }
    
    if(!(prev == 255 && input == 254) && j%2 > 0) {
      float value = input + prev / 256;
      float dpos = VD * H / value;
      float xpos = dpos * (j/2 - iwidth / 2) / VD;
      
      xpos /= scale;
      dpos /= scale;
      
      if(xpos < -width/2) xpos = -width/2;
      if(xpos > width/2-1) xpos = width/2-1;
      
      if(dpos < 0) dpos = 0;
      if(dpos > height-1) dpos = height-1;
      
      pixels[((int)xpos + width/2) + (height - 1 - (int)dpos) * width] = color(0, 255, 0);
      //pixels[j/2 + (iheight / 2 + input) * width] = color(0, 255, 0);
    }
    prev = input;
  }
  
  //print(".");
  updatePixels();
}