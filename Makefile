# Includes

# PANDAPATHINCLUDE = -I/home/caiosba/Downloads/panda3d-1.8.1/built/include -I/home/caiosba/Downloads/eigen-eigen-ffa86ffb5570
PANDAPATHINCLUDE = -I/usr/include/panda3d
PYTHONPATH = -I/usr/include/python2.7
OPENNI2PATH = -I/usr/include/openni2

# Libs

OPENNI2LIBPATH = /usr/lib/
# PANDAPATHLIB = -L/home/caiosba/Downloads/panda3d-1.8.1/built/lib
PANDAPATHLIB = -L/usr/lib/panda3d
PANDALIBS = $(PANDAPATHLIB) -lp3framework -lpanda -lpandafx -lpandaexpress -lp3dtoolconfig -lp3dtool -lp3pystub
LIBOPENCV1 = -L/usr/local/lib -lopencv_core -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_gpu -lopencv_calib3d -lcv2
# LIBOPENCV2 = -L/usr/include/opencv2 -lcxcore -lcvaux -lhighgui -lcv -lcvgpu
FREE_LIBS = -L/usr/local/lib -lfreenect -lpthread
OPENNI_LIBS = -L$(OPENNI2LIBPATH) -lOpenNI2

CXX = g++-4.4
CFLAGS=-fPIC -g -w `pkg-config --cflags opencv` -O3 -Wno-switch
INCLUDE = -I/usr/local/include/libfreenect $(OPENNI2PATH) -I/usr/include $(PANDAPATHINCLUDE) $(PYTHONPATH) -I/usr/local/include

HEADERFILES := $(foreach dir,src,$(wildcard $(dir)/*.h))
SRC := $(foreach dir,src,$(wildcard $(dir)/*.cpp))
OBJECTFILES := $(patsubst src/%.cpp,src/%.o$,$(SRC))

opencv2-openni : $(HEADERFILES) $(OBJECTFILES)
	$(CXX) $(CFLAGS) $(OBJECTFILES) -o opencv2-openni $(LIBOPENCV1) $(OPENNI_LIBS) $(PANDALIBS)

.cpp.o: $*.h
	@echo "Compiling: " $*
	@($(CXX) -o $*.o -c $(CFLAGS) $(INCLUDE) $*.cpp)
	
clean:
	rm -rf *.o src/*.o opencv2-openni

all: opencv2-openni
