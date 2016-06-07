FLAGS = -g
JC = javac
.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java -cp "./rxtx-2.1-7-bins-r2/RXTXcomm.jar:."

CLASSES = \
	lidar/Visualization.java \
	lidar/Comm.java \
	lidar/Main.java 

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) lidar/*.class

run:
	java -Djava.library.path="./rxtx-2.1-7-bins-r2/Linux/x86_64-unknown-linux-gnu/" -cp "./rxtx-2.1-7-bins-r2/RXTXcomm.jar:." lidar.Main
