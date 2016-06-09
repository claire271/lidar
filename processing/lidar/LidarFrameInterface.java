package lidar;

import java.awt.geom.*;

public interface LidarFrameInterface {
  public void processFrame(Point2D.Float points[],int npoints,Line2D.Float lines[],int nlines);
}
