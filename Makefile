# Includes

INCPATH = -I/usr/include
LOCALINCPATH = -I/usr/local/include
PANDAINCPATH = -I/usr/include/panda3d
PYTHONINCPATH = -I/usr/include/python2.7
OPENNI2INCPATH = -I/usr/include/openni2
FREENECTINCPATH = -I/usr/local/include/libfreenect
INCLUDES = $(INCPATH) $(LOCALINCPATH) $(PANDAINCPATH) $(PYTHONINCPATH) $(OPENNI2INCPATH) $(FREENECTINCPATH)

# Lib paths

LIBPATH = -L/usr/lib/
LOCALLIBPATH = -L/usr/local/lib
PANDALIBPATH = -L/usr/lib/panda3d
LIBPATHS = $(PANDALIBPATH) $(LOCALLIBPATH) $(LIBPATH)

# Libs

PANDALIBS = -lp3framework -lpanda -lpandafx -lpandaexpress -lp3dtoolconfig -lp3dtool -lp3pystub
OPENCVLIBS = -lopencv_core -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_gpu -lopencv_calib3d -lcv2
OPENNI2LIBS = -lOpenNI2
OTHERLIBS = -lfreenect -lpthread
LIBS = $(OPENCVLIBS) $(PANDALIBS) $(OPENNI2LIBS) $(OTHERLIBS)

CXX = g++-4.4
CFLAGS = -fPIC -g -w `pkg-config --cflags opencv` -O3 -Wno-switch

HEADERFILES := $(foreach dir,src,$(wildcard $(dir)/*.h))
SRC := $(foreach dir,src,$(wildcard $(dir)/*.cpp))
OBJECTFILES := $(patsubst src/%.cpp,src/%.o$,$(SRC))

opencv2-openni : $(HEADERFILES) $(OBJECTFILES)
	$(CXX) $(CFLAGS) $(OBJECTFILES) -o opencv2-openni $(LIBPATHS) $(LIBS)

.cpp.o: $*.h
	@echo "Compiling: " $*
	@($(CXX) -o $*.o -c $(CFLAGS) $(INCLUDES) $*.cpp)
	
clean:
	rm -rf *.o src/*.o opencv2-openni

all: opencv2-openni
