import processing.serial.*;

static final String ARDUINO_PORT_NAME = "/dev/tty.usbmodemfa131";
static final int WIDTH = 600;
static final int HEIGHT = 370;

Serial port = null;

float lineHeight = 0.0;

AxisControl[] axes = new AxisControl[] {
  new AxisControl(0, 0, int('D'), int('A')),
  new AxisControl(1, 0, int('W'), int('S')),
  new AxisControl(2, 0, RIGHT, LEFT)
};

void setup() {
  size(600, 370);
  background(0);
  
  lineHeight = textAscent() + textDescent();
  
  textAlign(LEFT, TOP);
  text("Trying to open port: " + ARDUINO_PORT_NAME + "...", 0, 0);  
  try {
    //port = new Serial(this, ARDUINO_PORT_NAME, 9600);
    text("Port ready", 0, lineHeight);
  } catch (Exception e) {
    text("Unable to connect to arduino: " + e.getMessage(), 0, lineHeight);        
  }    
  
}


void draw() {
  int colWidth = WIDTH / 3;
  
  fill(0);
  rect(0, 50, WIDTH, 150);
  fill(255);
  
  for (int i = 0; i < axes.length; i++) {
    text("" + axes[i].getValue(), (2 * i + 1) * colWidth / 2, 100);
  }
  
}

void keyPressed() {
  for (AxisControl axis: axes) {
    axis.handle(keyCode);    
  }
}

class AxisControl {
  
  private int index;
  private int value;  
  private int up, down;
  
  public AxisControl(int index, int value, int up, int down) {
    this.index = index;
    this.value = value;
    this.up = up;
    this.down = down;    
  }
  
  public void handle(int command) {
    int old = value;
    if (command == up) {
      value = Math.min(value + 1, 180);
    } else if (command == down) {
      value = Math.max(value - 1, 0);
    }
    
    if (old != value && port != null) {
      port.write(index + ":" + value + "$");
    }
  }
  
  public int getValue() {
    return value;
  }
  
}