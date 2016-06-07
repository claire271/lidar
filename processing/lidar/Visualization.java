package lidar;
import javax.swing.*;
import java.awt.*;

public class Visualization extends JFrame {
  private VisPanel panel;
  
  public Visualization() {
    super();

    this.setTitle("LIDAR Visualization");
    this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

    panel = new VisPanel();
    this.add(panel);

    this.pack();
    this.setVisible(true);
  }
}

class VisPanel extends JPanel {
  public static final int width = 800;
  public static final int height = 600;

  private static final int bgi = 63;
  private static final int scale = 2;
  private static final int gridw = 100;

  public VisPanel() {
    super();

    this.setPreferredSize(new Dimension(width,height));
  }
  public void paint(Graphics g) {
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
  }
}
