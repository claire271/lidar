import processing.serial.*;

//Constants
final int VD = 633;
final int h = 100;

final int iwidth = 640;
final int iheight = 480;

Serial port;       

void setup() {
  // Open the port you are using at the rate you want:
  port = new Serial(this, "/dev/ttyUSB0", 115200);
  size(640,480);
  frameRate(10);
  
  loadPixels();
}

void draw() {
  for(int i = 0;i < width * height;i++) {
    pixels[i] = color(0, 0, 0);
  }
  
  int i = 0;
  for(;;) {
    int input = port.read();
    if(input == -1) continue;
    if(input == 255) break;
    if(input != 254) {
      pixels[i + (iheight / 2 + input) * width] = color(0, 255, 0);
    }
    i++;
  }
  updatePixels();
}