/*
 *  pandaViewer.cpp
 *
 *  Created on: Mar 25, 2013
 *  Author: heekinho
 */

#include "pandaViewer.h"

#include "pandaFramework.h"
#include "load_prc_file.h"
#include "texture.h"
#include "pta_uchar.h"
#include "pta_ushort.h"
#include "cardMaker.h"
#include "nodePath.h"
#include "geomVertexData.h"
#include "geomTriangles.h"
#include "geomPoints.h"
#include "shaderPool.h"
#include "pnmImage.h"
#include "transparencyAttrib.h"
#include "omniBoundingVolume.h"
#include "pointCloud.h"

PandaFramework* pframework;
WindowFramework* pwindow;

NodePath render, scene;


PointCloud* cloud[2];
void* panda_thread(void* param){
	int argc = 0;
	char** argv;

	load_prc_file("config.prc");

	pframework = new PandaFramework();
	pframework->open_framework(argc, argv);
	pwindow = pframework->open_window();

	pframework->enable_default_keys();
	pwindow->setup_trackball();
	pwindow->enable_keyboard();

	render = pwindow->get_render();
	scene = render.attach_new_node("Scene Node");



	cloud[0] = new PointCloud(render);
	cloud[1] = new PointCloud(render);

	CardMaker cm = CardMaker("card");
	cm.set_frame_fullscreen_quad();
	NodePath floor = render.attach_new_node(cm.generate());
	floor.set_scale(0.16);
	floor.set_pos(-0.075, 0.11, 0);


	pframework->main_loop();

	return param;
}



