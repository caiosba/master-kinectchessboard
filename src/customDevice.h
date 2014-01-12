/*
 * customDevice.h
 * Created on: Apr 10, 2013
 * Author: heekinho
 */

#ifndef CUSTOMDEVICE_H
#define CUSTOMDEVICE_H

#include "iostream"
#include "OpenNI.h"

#include "opencv2/opencv.hpp"
#include "opencv2/calib3d/calib3d.hpp"

using namespace std;
using namespace openni;
using namespace cv;


class CustomDevice {
public:

	CustomDevice(DeviceInfo info, int id){
		this->id = id;
		name = info.getUri();
		device.open(info.getUri());
		openDepthStream();
		openColorStream();

		streams[0] = &depth;
		streams[1] = &color;
	}

	~CustomDevice(){
		color.stop();
		color.destroy();
		depth.stop();
		depth.destroy();
		device.close();
	}

	void openDepthStream(){
		if (device.getSensorInfo(SENSOR_DEPTH) != NULL) {
			Status status = depth.create(device, SENSOR_DEPTH);
			if (status != STATUS_OK) printError();

			status = depth.start();
			if (status != STATUS_OK) printError();
		}
	}

	void openColorStream(){
		if (device.getSensorInfo(SENSOR_COLOR) != NULL) {
			Status status = color.create(device, SENSOR_COLOR);
			if (status != STATUS_OK) printError();

			status = color.start();
			if (status != STATUS_OK) printError();
		}
	}

	int readFrame(){
		int readyStream = -1;
		Status status = OpenNI::waitForAnyStream(streams, 2, &readyStream);
		if(readyStream == 0) depth.readFrame(&frame);
		else color.readFrame(&frame);
		return readyStream;
	}

//	void mainLoop(){
//		cout << "main-loop" << endl;
//		char key = 0;
//
//		Mat image;
////	    while (key != 27)
//		{
//	    	cout << "Reading frame..." << endl;
//	    	readFrame();
//
//	    	// Só processamos quando o frame for RGB
//	    	if(frame.getVideoMode().getPixelFormat() == PIXEL_FORMAT_RGB888){
//	    		image = Mat(frame.getHeight(), frame.getWidth(), CV_8UC3, (void*)frame.getData());
//	    		cvtColor(image, image, CV_BGR2RGB);
//	    		imshow("Kinect " + name, image);
//	    	}
//
//			key = cvWaitKey(1);
//	    }
//	}

	void printError(){
		cout << "Couldn't create stream" << OpenNI::getExtendedError();
	}

	int id;
	string name;
	Device device;
	VideoStream depth, color;
	VideoFrameRef frame;
	VideoStream* streams[2];
};


#endif /* CUSTOMDEVICE_H */
