package lidar;

import gnu.io.*;

public class Comm {
  
  public Comm() {}

  public void connect(String portName) throws Exception {
    CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
    if ( portIdentifier.isCurrentlyOwned() ) {
      System.out.println("Error: Port is currently in use");
    }
    else {
      CommPort commPort = portIdentifier.open(this.getClass().getName(),2000);
      
      if(commPort instanceof SerialPort) {
        SerialPort serialPort = (SerialPort) commPort;
        serialPort.setSerialPortParams(57600,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
        
        //InputStream in = serialPort.getInputStream();
        //OutputStream out = serialPort.getOutputStream();
        
        //(new Thread(new SerialReader(in))).start();
        //(new Thread(new SerialWriter(out))).start();
      }
      else {
        System.out.println("Error: Only serial ports are handled by this example.");
      }
    }    
  }
}
