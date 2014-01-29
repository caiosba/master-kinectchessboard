OPENNI2PATH = /home/caiosba/Mestrado/OpenNI2

# Includes

INCPATH = -I/usr/include
LOCALINCPATH = -I/usr/local/include
PANDAINCPATH = -I/usr/local/include/panda3d
PYTHONINCPATH = -I/usr/include/python2.7
OPENNI2INCPATH = -I$(OPENNI2PATH)/Include
FREENECTINCPATH = -I/usr/local/include/libfreenect
EIGENINCPATH = -I/usr/local/include/Eigen
INCLUDES = $(EIGENINCPATH) $(FREENECTINCPATH) $(OPENNI2INCPATH) $(INCPATH) $(LOCALINCPATH) $(PANDAINCPATH) $(PYTHONINCPATH) 

# Lib paths

LIBPATH = -L/usr/lib/
LOCALLIBPATH = -L/usr/local/lib
PANDALIBPATH = -L/usr/local/lib/panda3d
OPENNI2LIBPATH = $(OPENNI2PATH)/Samples/Bin
FREENECTLIBPATH = -L/usr/local/lib
LIBPATHS = -L$(OPENNI2LIBPATH) $(FREENECTLIBPATH) $(PANDALIBPATH) $(LOCALLIBPATH) $(LIBPATH)

# Libs

PANDALIBS = -lp3framework -lpanda -lpandafx -lpandaexpress -lp3dtoolconfig -lp3dtool -lp3pystub
OPENCVLIBS = -lopencv_core -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_gpu -lopencv_calib3d -lcv2
OPENNI2LIBS = -lOpenNI2
OTHERLIBS = -lpthread -lfreenect
LIBS = $(OPENCVLIBS) $(PANDALIBS) $(OPENNI2LIBS) $(OTHERLIBS)

CXX = g++-4.6
CFLAGS = -fPIC -g -w `pkg-config --cflags opencv` -O3 -Wno-switch

HEADERFILES := $(foreach dir,src,$(wildcard $(dir)/*.h))
SRC := $(foreach dir,src,$(wildcard $(dir)/*.cpp))
OBJECTFILES := $(patsubst src/%.cpp,src/%.o$,$(SRC))

opencv2-openni : $(HEADERFILES) $(OBJECTFILES)
	$(CXX) $(CFLAGS) $(OBJECTFILES) -o $(OPENNI2LIBPATH)/opencv2-openni $(LIBPATHS) $(LIBS) -Wl,-rpath ./

.cpp.o: $*.h
	@echo "Compiling: " $*
	@($(CXX) -o $*.o -c $(CFLAGS) $(INCLUDES) $*.cpp)

clean:
	rm -rf *.o src/*.o $(OPENNI2LIBPATH)/opencv2-openni

all: opencv2-openni
