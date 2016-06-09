package lidar;

import java.awt.geom.*;

public class Lidar implements RawFrameInterface {
  public static final int VD = 633;
  public static final int H = 100;
  public static final int IW = 640;
  public static final int IH = 480;

  private Point2D.Float points[] = new Point2D.Float[IW];
  private int npoints;
  private Line2D.Float lines[] = new Line2D.Float[IW];
  private int nlines;

  private LidarFrameInterface receiver = null;

  public Lidar() {
    npoints = 0;
    for(int i = 0;i < points.length;i++) {
      points[i] = new Point2D.Float(0,0);
    }
    nlines = 0;
    for(int i = 0;i < lines.length;i++) {
      lines[i] = new Line2D.Float(0,0,0,0);
    }
  }

  public void setReceiver(LidarFrameInterface receiver) {
    this.receiver = receiver;
  }

  public void processFrame(int data[]) {
    npoints = 0;
    nlines = 0;

    int prev_data = 0;
    int prev_index = 0;
    for(int i = 0;i < data.length;i++) {
      if(data[i] == 254 || data[i] == 0) continue;

      //Finding points
      points[npoints].y = VD * H * 1.0f / data[i];
      points[npoints].x = points[npoints].y * (i - IW/2) / VD;

      //Finding groups of points
      if(npoints > 1) {
        if(i - prev_index < 4 &&
           Math.abs(data[i] - prev_data) < 4) {
          lines[nlines].x1 = points[npoints - 1].x;
          lines[nlines].y1 = points[npoints - 1].y;
          lines[nlines].x2 = points[npoints].x;
          lines[nlines].y2 = points[npoints].y;

          nlines++;
        }
      }

      npoints++;
      prev_data = data[i];
      prev_index = i;
    }

    if(receiver != null) {
      receiver.processFrame(points,npoints,lines,nlines);
    }
  }
}
