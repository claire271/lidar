package lidar;

import gnu.io.*;
import java.io.*;

public class Comm implements Runnable {
  private InputStream in;
  private OutputStream out;

  private Thread readThread;
  private int inBuf[] = new int[Lidar.IW];

  private RawFrameInterface receiver = null;
  
  public Comm() {}

  public void connect(String portName) throws Exception {
    CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
    if ( portIdentifier.isCurrentlyOwned() ) {
      System.out.println("Error: Port is currently in use");
    }
    else {
      CommPort commPort = portIdentifier.open(this.getClass().getName(),0);
      
      if(commPort instanceof SerialPort) {
        SerialPort serialPort = (SerialPort) commPort;
        serialPort.setSerialPortParams(115200,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
        
        in = serialPort.getInputStream();
        out = serialPort.getOutputStream();

        readThread = new Thread(this);
        readThread.start();
      }
      else {
        System.out.println("Error: Only serial ports are handled.");
      }
    }    
  }

  public void setReceiver(RawFrameInterface receiver) {
    this.receiver = receiver;
  }

  public void run() {
    int index = 0;
    for(;;) {
      int input = -1;
      try {
        input = in.read();
      }
      catch(IOException ex) {
        ex.printStackTrace();
      }
      if(input == -1) continue;
      if(input == 255 || index == inBuf.length) {
        if(receiver != null) {
          receiver.processFrame(inBuf);
        }
        index = 0;
      }
      else {
        inBuf[index++] = input;
      }
    }
  }
}
