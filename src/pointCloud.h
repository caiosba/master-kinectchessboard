/*
 * pointCloud.h
 * Created on: Apr 10, 2013
 * Author: heekinho
 */

#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "nodePath.h"
#include "texture.h"

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;

class PointCloud {
public:
	PointCloud(const NodePath &scene);
	~PointCloud();

	static NodePath createPointCloud(int width, int height);
	void createCloud();

	void configureTexture();
	void updateColor(void* data);
	void updateDepth(void* data);

	void updateCameraMatrix(Mat m, bool invert = false);


	NodePath cloud;
	NodePath camera;

	PT(Texture) color;
	PT(Texture) depth;

	int width, height;
};


#endif /* POINTCLOUD_H */
