package lidar;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;

public class Visualization extends JFrame implements LidarFrameInterface {
  private VisPanel panel;
  
  public Visualization() {
    super();

    this.setTitle("LIDAR Visualization");
    this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    panel = new VisPanel();
    this.add(panel);

    this.pack();
    this.setVisible(true);
  }
  public void processFrame(Point2D.Float points[],int npoints,Line2D.Float lines[],int nlines) {
    panel.processFrame(points,npoints,lines,nlines);
  }
}

class VisPanel extends JPanel implements LidarFrameInterface {
  public static final int width = 800;
  public static final int height = 600;

  private static final int bgi = 63;
  private static final int scale = 2;
  private static final int gridw = 100;

  private BufferedImage image;
  private long old_time;
  private float fps;

  private static final float TDC = 0.8f;

  public VisPanel() {
    super();

    old_time = System.currentTimeMillis();
    fps = 10.0f;
    image = new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);
    this.setPreferredSize(new Dimension(width,height));
  }
  public void paint(Graphics g) {
    g.drawImage(image,0,0,Color.BLACK,null);
  }
  public void processFrame(Point2D.Float points[],int npoints,Line2D.Float lines[],int nlines) {
    Graphics2D g = image.createGraphics();

    //Background stuff
    g.setColor(new Color(0,0,0));
    g.fillRect(0,0,width,height);

    g.setColor(new Color(bgi,0,bgi));
    g.drawLine(width/2,0,width/2,height);
    g.drawLine(0,height,width,height);

    g.setColor(new Color(bgi,0,0));
    for(int i = height - gridw/scale;i >= 0;i -= gridw/scale) {
      g.drawLine(0,i,width,i);
    }
    for(int i = gridw/scale;i < width/2;i += gridw/scale) {
      g.drawLine(width/2 + i,0,width/2 + i,height);
      g.drawLine(width/2 - i,0,width/2 - i,height);
    }

    //Draw points
    g.setColor(new Color(0,255,0));
    for(int i = 0;i < npoints;i++) {
      g.drawRect((int)(width/2 + points[i].x/scale),(int)(height - points[i].y/scale),0,0);
    }

    //Draw lines
    g.setColor(new Color(0,255,255));
    for(int i = 0;i < nlines;i++) {
      g.drawLine((int)(width/2 + lines[i].x1/scale),(int)(height - lines[i].y1/scale),
                 (int)(width/2 + lines[i].x2/scale),(int)(height - lines[i].y2/scale));
    }

    //Calculate framerate
    long cur_time = System.currentTimeMillis();
    float cur_fps = 1000.0f / (cur_time - old_time);
    old_time = cur_time;
    fps = fps * TDC + cur_fps * (1.0f - TDC);
    //Draw framerate
    g.setColor(Color.WHITE);
    g.drawString("Current FPS: " + ((int)(fps * 100 + 0.5))/100.0,5,15);

    this.repaint();
  }
}
