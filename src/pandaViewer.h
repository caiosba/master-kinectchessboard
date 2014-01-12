#include "texture.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;

void* panda_thread(void*);
void set_rgb_data(int width, int height, void* image, int f);
void set_depth_data(int width, int height, void* image, int f);

void set_mat(Mat m, int f);
void create_frame(int f);


