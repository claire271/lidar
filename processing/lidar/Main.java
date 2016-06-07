package lidar;

public class Main {
  public static void main(String[] args) {

    Visualization vis = new Visualization();
    Comm comm = new Comm();
    try {
      comm.connect("/dev/ttyUSB0");
    }
    catch (Exception ex) {
      ex.printStackTrace();
    }
  }
}
