/*
 * utils.cpp
 * Created on: Apr 10, 2013
 * Author: heekinho
 */

#include "iostream"
#include "OpenNI.h"

#include "opencv2/opencv.hpp"
#include "opencv2/calib3d/calib3d.hpp"

using namespace std;
using namespace openni;
using namespace cv;


void showChessboard(Mat& image){
    // Output vectors of image points
    std::vector<cv::Point2f> imageCorners;

    // Number of corners on the chessboard
    cv::Size boardSize(9, 6);

    // Get the chessboard corners
    bool found = cv::findChessboardCorners(image, boardSize, imageCorners, 2 & 4);

    // Draw the corners
    cv::drawChessboardCorners(image, boardSize, imageCorners, found);

    const char* chessboardstr = "chessboard";
    namedWindow(chessboardstr);
    imshow(chessboardstr, image);
}
