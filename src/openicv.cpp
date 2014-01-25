// This is the main file

#include "common.h"
#include "iostream"
#include "OpenNI.h"
#include "pointCloud.h"
#include "pandaViewer.h"
#include "customDevice.h"
#include "opencv2/opencv.hpp"
#include "opencv2/calib3d/calib3d.hpp"

int numframe = 0;

using namespace std;
using namespace openni;
using namespace cv;

vector<CustomDevice*> devices;

extern PointCloud* cloud[2];

/**
 * TODO: Verify if we should use the depth map to get the position (distance) and rotation,
 * instead of using only RGB and chessboard on calibration
 */
void findMatrix(vector<Point3f> &objectPoints, vector<Point2f> &imagePoints, Mat intrinsic, Mat distCoeffs, int id) {
  Mat rvec, tvec;
  solvePnP(objectPoints, imagePoints, intrinsic, distCoeffs, rvec, tvec);

  if (DEBUG) cout << "Rotation: " << rvec  << endl;
  if (DEBUG) cout << "Translation: " << tvec * 2.3f << endl;

  // Calculate rotation matrix using Rodrigues
  Mat rotMat = Mat(3, 3, CV_64F);
  Rodrigues(rvec, rotMat);
  if (DEBUG) cout << rotMat << endl;

  // Calculate transpose of the matrix in order to build the homogeneous one
  Mat rotMatT = rotMat.t();
  if (DEBUG) cout << rotMatT << endl;

  Mat RTt = -rotMatT * tvec;
  if (DEBUG) cout << "RTt: " << RTt << endl;

  // Get the homogeneous
  Mat homogeneous = Mat::eye(4, 4, CV_64F);

  // Put the transposed rotation matrix on the homogeneous
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      homogeneous.at<double>(i, j) = rotMatT.at<double>(i, j);
    }
  }

  // Put the translation on the homogeneous
  for (int i = 0; i < 3; i++) {
    homogeneous.at<double>(i, 3) = RTt.at<double>(0, i);
  }

  if (DEBUG) cout << "Matrix: " <<  homogeneous << endl;

  //FIXME: Is it necessary? // set_mat(homogenea, numframe);
  cloud[id]->updateCameraMatrix(homogeneous, id == 1);
}


void trackCameraPosition(int numCornersHor, int numCornersVer, CustomDevice* device) {
  // FIXME: Hardcoded here after calibration, but whatever...
  Mat intrinsic = (Mat_<double>(3,3) << 526.0, 0, 318.0, 0, 526.0, 270.0, 0, 0, 1);
  Mat distCoeffs = (Mat_<double>(1,5) << 0.2396517942750204, -0.9133942441833466, 0.000202332030289716, 0.001075496181332869, 1.275027377722337);

  int numSquares = numCornersHor * numCornersVer;
  Size boardSize = Size(numCornersHor, numCornersVer);

  vector<Point2f> corners;
  vector<Point3f> boardReference;
  for (int i = 0; i < numSquares; i++) {
    boardReference.push_back(Point3f(i/numCornersHor, i%numCornersHor, 0.0f));
  }

  Mat imageDistorted, image, grayImage, imageDepth;

  const VideoFrameRef& frame = device->frame;
  char key = 0;
  //FIXME while (key != 27)
  {
    if (DEBUG) cout << "Reading frame..." << endl;
    //FIXME readKinectFrame();

    // Only process RGB frames
    if (frame.getVideoMode().getPixelFormat() == PIXEL_FORMAT_RGB888) {

      //FIXME imageDistorted = Mat(frame.getHeight(), frame.getWidth(), CV_8UC3, (void*)frame.getData());
      //FIXME undistort(imageDistorted, image, intrinsic, distCoeffs);
      image = Mat(frame.getHeight(), frame.getWidth(), CV_8UC3, (void*)frame.getData());
      cvtColor(image, image, CV_BGR2RGB);
      cvtColor(image, grayImage, CV_RGB2GRAY);

      //FIXME set_rgb_data(image.cols, image.rows, (void*)image.data, numframe);
      cloud[device->id]->updateColor(image.data);

      bool found = findChessboardCorners(grayImage, boardSize, corners,  (CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS));
      
      if (found) {
        // cornerSubPix(grayImage, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(image, boardSize, corners, found);
        findMatrix(boardReference, corners, intrinsic, distCoeffs, device->id);
      }

      imshow("RGB Kinect " + device->name, image);
      //FIXME imshow("Gray Image", grayImage);

    }
    
    else if ((frame.getVideoMode().getPixelFormat() == PIXEL_FORMAT_DEPTH_100_UM) ||
            (frame.getVideoMode().getPixelFormat() == PIXEL_FORMAT_DEPTH_1_MM) ) {
      //FIXME imageDistorted = Mat(frame.getHeight(), frame.getWidth(), CV_16UC1, (void*)frame.getData());
      //FIXME undistort(imageDistorted, imageDepth, intrinsic, distCoeffs);
      imageDepth = Mat(frame.getHeight(), frame.getWidth(), CV_16UC1, (void*)frame.getData());
      Mat imgDEP(frame.getHeight(), frame.getWidth(), CV_16UC1, (void*) frame.getData());

      // Convert and display
      imgDEP.convertTo(imgDEP, CV_8U, 255.0 / 4096.0);
      imshow("Image Depth " + device->name, imgDEP);
      //FIXME get3DMapping();
      //FIXME set_depth_data(imageDepth.cols, imageDepth.rows, (void*)imageDepth.data, numframe);
      if (DEBUG) cout << "updateDepth" << endl;
      cloud[device->id]->updateDepth(imageDepth.data);
    }

    key = cvWaitKey(10);
    if (key == 'a') {
      if (DEBUG) cout << "Space" << endl;
      numframe++;
    }
  }
}

Status initOpenNI() {
  // Initialize OpenNI
  Status rc = OpenNI::initialize();
  if (rc != STATUS_OK) {
    if (DEBUG) cout << "Initialize failed" << OpenNI::getExtendedError();
    return rc;
  }

  // Verify and open Kinects
  Array<DeviceInfo> devicesInfo;
  OpenNI::enumerateDevices(&devicesInfo);

  if (DEBUG) cout << devicesInfo.getSize() << " Kinect(s) found" << endl;
  if (devicesInfo.getSize() == 0) {
    if (DEBUG) cout << "Fatal: No Kinect found!" << endl;
    exit;
  }

  for (int i = 0; i < devicesInfo.getSize(); i++) {
    devices.push_back(new CustomDevice(devicesInfo[i], i));
  }

  // No thread
  while (true) {
    //FIXME cout << "Main Thread Rodando" << endl;
    //FIXME devices[0]->mainLoop();
    devices[0]->readFrame();
    trackCameraPosition(9, 6, devices[0]);

    //FIXME devices[1]->mainLoop();
    devices[1]->readFrame();
    trackCameraPosition(9, 6, devices[1]);
  }
  return rc;
}

int main(int argc, char** argv) {
  // Create a panda thread for visualization
  pthread_t thread_id;
  int param;
  pthread_create(&thread_id, NULL, panda_thread, &param);

  // Initialize OpenNI
  Status rc = initOpenNI();
  if (rc != STATUS_OK) {
    if (DEBUG) cout << "Error while loading OpenNI. Exiting..." << endl;
    return 1;
  }

  //FIXME kinectRgbCalibration(10, 9, 6);

  //FIXME trackCameraPosition(9, 6);

  OpenNI::shutdown();
  return 0;
}
