package lidar;

import java.awt.geom.*;

public class Lidar implements RawFrameInterface {
  public static final int VD = 633;
  public static final int H = 100;
  public static final int IW = 640;
  public static final int IH = 480;

  private Point2D.Float points[] = new Point2D.Float[IW];
  private int npoints;

  private LidarFrameInterface receiver = null;

  public Lidar() {
    npoints = 0;
    for(int i = 0;i < points.length;i++) {
      points[i] = new Point2D.Float(0,0);
    }
  }

  public void setReceiver(LidarFrameInterface receiver) {
    this.receiver = receiver;
  }

  public void processFrame(int data[]) {
    npoints = 0;

    for(int i = 0;i < data.length;i++) {
      if(data[i] == 254 || data[i] == 0) continue;

      points[npoints].y = VD * H * 1.0f / data[i];
      points[npoints].x = points[npoints].y * (i - IW/2) / VD;
      npoints++;
    }

    if(receiver != null) {
      receiver.processFrame(points,npoints);
    }
  }
}
