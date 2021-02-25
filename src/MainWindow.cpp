///
/// @file MainWindow.cpp
/// @brief Handles GUI

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_gl = new NGLScene(this);

  m_ui->s_mainWindowGridLayout->addWidget(m_gl, 0, 0, 2, 1);

  // Buttons
  connect(m_ui->m_readImages_btn, SIGNAL(clicked()), m_gl, SLOT(readImages()));
  connect(m_ui->m_checkImages_btn, SIGNAL(clicked()), m_gl, SLOT(checkImages()));
  connect(m_ui->m_sampleImages_btn, SIGNAL(clicked()), m_gl, SLOT(sampleImages()));
  connect(m_ui->m_marchCubes_btn, SIGNAL(clicked()), m_gl, SLOT(marchCubes()));
  connect(m_ui->m_generateMesh_btn, SIGNAL(clicked()), m_gl, SLOT(generateMesh()));
  connect(m_ui->m_exportMesh_btn, SIGNAL(clicked()), m_gl, SLOT(exportMesh()));

  connect(m_ui->m_meshColour_pb, SIGNAL(clicked()), m_gl, SLOT(setMeshColour()));
  connect(m_ui->m_lightColour_pb, SIGNAL(clicked()), m_gl, SLOT(setLightColour()));

  // Inputs
  connect(m_ui->m_sampleResolution_sb, SIGNAL(valueChanged(int)), m_gl, SLOT(setSampleResolution(int)));
  connect(m_ui->m_surfaceLevel_sb, SIGNAL(valueChanged(int)), m_gl, SLOT(setSurfaceLevel(int)));
  connect(m_ui->m_metallicness_sb, SIGNAL(valueChanged(double)), m_gl, SLOT(setMetallicness(double)));
  connect(m_ui->m_roughness_sb, SIGNAL(valueChanged(double)), m_gl, SLOT(setRoughness(double)));
  connect(m_ui->m_ao_sb, SIGNAL(valueChanged(double)), m_gl, SLOT(setAO(double)));
  connect(m_ui->m_exposure_sb, SIGNAL(valueChanged(double)), m_gl, SLOT(setExposure(double)));
  connect(m_ui->m_posX_sb, SIGNAL(valueChanged(double)), m_gl, SLOT(setLightXPos(double)));
  connect(m_ui->m_posY_sb, SIGNAL(valueChanged(double)), m_gl, SLOT(setLightYPos(double)));
  connect(m_ui->m_posZ_sb, SIGNAL(valueChanged(double)), m_gl, SLOT(setLightZPos(double)));

  // Rotation
  connect(m_ui->m_rotationX_dsb, SIGNAL(valueChanged(double)), m_gl, SLOT(setXRotation(double)));
  connect(m_ui->m_rotationY_dsb, SIGNAL(valueChanged(double)), m_gl, SLOT(setYRotation(double)));
  connect(m_ui->m_rotationZ_dsb, SIGNAL(valueChanged(double)), m_gl, SLOT(setZRotation(double)));

  // Scale
  connect(m_ui->m_uniformScale_dsb, SIGNAL(valueChanged(double)), m_gl, SLOT(setUniformScale(double)));

  // Cameras
  connect(m_ui->m_cameraSelection_cb, SIGNAL(currentIndexChanged(int)), m_gl, SLOT(setCamera(int)));

  // Toggles
  connect(m_ui->m_wireframe_cb, SIGNAL(toggled(bool)), m_gl, SLOT(toggleWireframeMode(bool)));
  connect(m_ui->m_cull_cb, SIGNAL(toggled(bool)), m_gl, SLOT(toggleBackFaceCull(bool)));
}

void MainWindow::on_m_readImages_btn_clicked()
{
  QString q_imagesPath = m_ui->m_imagePath_le->text();
  std::string s_imagesPath = q_imagesPath.toStdString();
  m_gl->setImagePath(s_imagesPath);
}

void MainWindow::on_m_exportMesh_btn_clicked()
{
  QString q_exportPath = m_ui->m_exportPath_le->text();
  std::string s_exportPath = q_exportPath.toStdString();
  m_gl->setExportPath(s_exportPath);

  QString q_fileName = m_ui->m_fileName_le->text();
  std::string s_fileName = q_fileName.toStdString();
  m_gl->setFileName(s_fileName);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
