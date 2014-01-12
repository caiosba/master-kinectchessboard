#include "common.h"
#include "pointCloud.h"
#include "geomVertexWriter.h"
#include "geomVertexData.h"
#include "geomTriangles.h"
#include "geomPoints.h"
#include "geomNode.h"
#include "shaderPool.h"
#include "pnmImage.h"

PointCloud::PointCloud(const NodePath &scene) {
  width = 640;
  height = 480;

  configureTexture();
  createCloud();

  camera = scene.attach_new_node("camera");
  cloud.reparent_to(camera);
}

PointCloud::~PointCloud() {
  cloud.remove_node();
}

void PointCloud::configureTexture() {

  // Configures the color texture
  color = new Texture("stream-rgb");
  color->set_compression(Texture::CM_off);
  color->setup_2d_texture(width, height, Texture::T_unsigned_byte, Texture::F_rgb);
  color->set_keep_ram_image(true);

  // Configures the depth texture
  depth = new Texture("stream-depth");
  depth->set_compression(Texture::CM_off);
  depth->setup_2d_texture(width, height, Texture::T_unsigned_short, Texture::F_depth_component);
  depth->set_keep_ram_image(true);

}

void PointCloud::createCloud() {
  cloud = createPointCloud(width, height);

  cloud.set_shader(ShaderPool::load_shader("depth.cg"));
  //FIXME cloud.set_transparency(TransparencyAttrib::M_alpha);
  cloud.set_texture(new TextureStage("depth"), depth);
  cloud.set_texture(new TextureStage("color"), color);

  cloud.set_render_mode_thickness(2);
}

void PointCloud::updateColor(void* data) {
  // Copy directly since the set_data seems to be buggy
  PTA_uchar colorBufferMem = PTA_uchar::empty_array(sizeof(uchar) * width * height * 3);
  memcpy(colorBufferMem.p(), data, sizeof(uchar) * width * height * 3);
  color->set_ram_image(CPTA_uchar(colorBufferMem));
}

void PointCloud::updateDepth(void* data) {
  // Copy directly since the set_data seems to be buggy
  PTA_uchar depthBufferMem = PTA_uchar::empty_array(sizeof(ushort) * width * height);
  memcpy(depthBufferMem.p(), data, sizeof(ushort) * width * height);
  depth->set_ram_image(CPTA_uchar(depthBufferMem));
}

void PointCloud::updateCameraMatrix(Mat m, bool invert) {
  LMatrix4f pmat = LMatrix4f(
    m.at<double>(0,0), m.at<double>(0,1), m.at<double>(0,2), m.at<double>(0,3),
    m.at<double>(1,0), m.at<double>(1,1), m.at<double>(1,2), m.at<double>(1,3),
    m.at<double>(2,0), m.at<double>(2,1), m.at<double>(2,2), m.at<double>(2,3),
    m.at<double>(3,0), m.at<double>(3,1), m.at<double>(3,2), m.at<double>(3,3));
  pmat.transpose_in_place();

  camera.set_mat(pmat);

  LVecBase3 pos = camera.get_pos();
  if (invert && DEBUG) cout << pos << endl;
  LVecBase3 hpr = camera.get_hpr();
  if (invert && DEBUG) cout << "HPR: " << hpr << endl;

  camera.set_pos(0, 0, 0);
  camera.set_hpr(0, 0, 0);

  float factor = (0.01 * 2.25);
  
  if (invert) {
    camera.set_x(pos[1] * factor);
    camera.set_y(pos[0] * factor);
    camera.set_z(pos[2] * factor);
    camera.set_p(hpr[1]);
    camera.set_r(hpr[2]);
    camera.set_h(90-hpr[0]);
  }
  
  else {
    camera.set_x(-pos[1] * factor);
    camera.set_y(-pos[0] * factor);
    camera.set_z(-pos[2] * factor);
    camera.set_p(-hpr[1]);
    camera.set_r(180+hpr[2]);
    camera.set_h(90-hpr[0]);
  }
}

NodePath PointCloud::createPointCloud(int width, int height) {
  PT(GeomVertexData) vdata = new GeomVertexData("name", GeomVertexFormat::get_v3n3(), Geom::UH_static);

  GeomVertexWriter vertex, normal, color, texcoord;
  vertex = GeomVertexWriter(vdata, "vertex");

  // Creates a grid with z = 0 for all points. The shader will do the work
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      vertex.add_data3f(i, j, 0.0f);
    }
  }

  PT(GeomPoints) prim = new GeomPoints(Geom::UH_static);
  for(int i = 0; i < width * height; i++) prim->add_vertex(i);
  prim->close_primitive();

  PT(Geom) cloud_geom = new Geom(vdata);
  cloud_geom->add_primitive(prim);

  PT(GeomNode) cloud_gn = new GeomNode("depthmap");
  cloud_gn->add_geom(cloud_geom);

  return NodePath(cloud_gn);
}
