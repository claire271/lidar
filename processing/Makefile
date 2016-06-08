FLAGS = -g
JC = javac
.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java -cp "./rxtx-2.2pre2-bins/RXTXcomm.jar:."

CLASSES = \
	lidar/Visualization.java \
	lidar/RawFrameInterface.java \
	lidar/LidarFrameInterface.java \
	lidar/Lidar.java \
	lidar/Comm.java \
	lidar/Main.java 

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) lidar/*.class
