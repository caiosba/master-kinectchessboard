Using Ubuntu 12.04, 32 bits, I had to install the following:

1. libfreenect from Github, commit c4505cc from master

* `mkdir build`
* `cd build`
* `cmake ..`
* `make`
* `sudo make install`

If you also installed libfreenect from Ubuntu repositories, you may need to remove it. Check if the static library is
being correctly linked by running `ldd libOpenNI2.so`. You may need to do the following:

* sudo rm /usr/lib/libfreenect.so.0.0
* ln -s /usr/local/lib/libfreenect.so /usr/lib/libfreenect.so.0.0

2. OpenNI2-FreenectDriver from Github, commit 8ca95ba from master

* `./waf configure build`

3. OpenNI2 2.2.0.33 from http://www.openni.org/openni-sdk/

* Just decompress the file and copy the driver to here
* `cp path/to/OpenNI2-FreenectDriver/build/libFreenect.so Samples/Bin/OpenNI2/Drivers`
* `cp path/to/OpenNI2-FreenectDriver/build/libFreenect.so Redist/OpenNI2/Drivers`
* Set the path to this directory on "run" and "Makefile"
