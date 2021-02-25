///
/// @file NGLScene.cpp
/// @brief Main program loop

#include <QColorDialog>
#include <QMouseEvent>

#include <ngl/NGLInit.h>
#include <ngl/NGLStream.h>
#include <ngl/ShaderLib.h>
#include <ngl/SimpleVAO.h>
#include <ngl/VAOFactory.h>
#include <ngl/VAOPrimitives.h>

#include <fstream>
#include <fmt/format.h>
#include <iostream>
#include <sstream>

#include "NGLScene.h"
#include "Timer.h"

NGLScene::NGLScene(QWidget *_parent) : QOpenGLWidget(_parent)
{
  setFocusPolicy(Qt::ClickFocus);   // Wherever the user clicks, give keyboard focus to
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());

  // Initialise cameras
  m_fpsCamera.Initialise(m_win.width, m_win.height, 1);
  m_staticCamera.Initialise(m_win.width, m_win.height, 0);

  // Initialise update timers
  m_cameraTimer = startTimer(2);
  m_redrawTimer = startTimer(20);
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
  if (m_vao != nullptr)
  {
    m_vao->removeVAO();
  }
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>(_w * devicePixelRatio());
  m_win.height = static_cast<int>(_h * devicePixelRatio());
  if (m_camera == 1)
  {
    m_fpsCamera.UpdateProjectionMatrix();
  }
  else
  {
    m_staticCamera.UpdateProjectionMatrix();
  }
}

constexpr auto shaderProgram = "PBR";

void NGLScene::initializeGL()
{
  ngl::NGLInit::initialize();
  glClearColor(0.8f, 0.8f, 0.8f, 1.0f);			// Grey Background
  glEnable(GL_DEPTH_TEST);                  // Enable depth testing for drawing
  glEnable(GL_MULTISAMPLE);                 // Enable multisampling for smoother drawing

  ngl::VAOPrimitives::createTrianglePlane("ground", 100, 100, 10, 10, ngl::Vec3::up());

  constexpr auto vertexShader = "PBRVertex";
  constexpr auto fragShader = "PBRFragment";
  // create the shader program
  ngl::ShaderLib::createShaderProgram(shaderProgram);
  // now we are going to create empty shaders for Frag and Vert
  ngl::ShaderLib::attachShader(vertexShader, ngl::ShaderType::VERTEX);
  ngl::ShaderLib::attachShader(fragShader, ngl::ShaderType::FRAGMENT);
  // attach the source
  ngl::ShaderLib::loadShaderSource(vertexShader, "shaders/PBRVertex.glsl");
  ngl::ShaderLib::loadShaderSource(fragShader, "shaders/PBRFragment.glsl");
  // compile the shaders
  ngl::ShaderLib::compileShader(vertexShader);
  ngl::ShaderLib::compileShader(fragShader);
  // add them to the program
  ngl::ShaderLib::attachShaderToProgram(shaderProgram, vertexShader);
  ngl::ShaderLib::attachShaderToProgram(shaderProgram, fragShader);
  // now we have associated that data we can link the shader
  ngl::ShaderLib::linkProgramObject(shaderProgram);
  // and make it active ready to load values
  ngl::ShaderLib::use(shaderProgram);
  //ngl::ShaderLib::setUniform("camPos", m_eye);

  ngl::ShaderLib::setUniform("lightPosition", 0.0f, 10.0f, 0.0f);
  ngl::ShaderLib::setUniform("lightColor", 255.0f, 255.0f, 255.0f);
  ngl::ShaderLib::setUniform("exposure", 10.0f);
  ngl::ShaderLib::setUniform("albedo", 0.950f, 0.0f, 0.0f);
  ngl::ShaderLib::setUniform("metallic", 0.0f);
  ngl::ShaderLib::setUniform("roughness", 0.0f);
  ngl::ShaderLib::setUniform("ao", 0.0f);
}

void NGLScene::LoadMatrixToShader()
{
  ngl::ShaderLib::use("PBR");
  struct transform
  {
    ngl::Mat4 MVP;
    ngl::Mat4 normalMatrix;
    ngl::Mat4 M;
  };
  transform t;
  t.M = m_transform.getMatrix();

  t.MVP = m_projectionMatrix * m_viewMatrix * t.M;
  t.normalMatrix = t.M;
  t.normalMatrix.inverse().transpose();
  
  ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);
}

void NGLScene::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Clear the screen and depth buffer
  glViewport(0, 0, m_win.width, m_win.height);

  // Wireframe mode toggle
  if (!m_wireframe)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  // Back face cull toggle
  if (!m_cull) {
    glDisable(GL_CULL_FACE);
  }
  else
  {
    glEnable(GL_CULL_FACE);
  }

  // Update per frame
  if (m_camera == 1)
  {
    m_viewMatrix = m_fpsCamera.GetViewMatrix();
    m_projectionMatrix = m_fpsCamera.GetProjectionMatrix();
    m_eye = m_fpsCamera.GetEye();
  }
  else
  {
    m_viewMatrix = m_staticCamera.GetViewMatrix();
    m_projectionMatrix = m_staticCamera.GetProjectionMatrix();
    m_eye = m_staticCamera.GetEye();
  }

  m_deltaTime = m_timer.DeltaTime();

  // Transform objects in scene
	m_transform.setScale(m_scale);
	m_transform.setRotation(m_rotation);

  LoadMatrixToShader();

  // Placeholder geometry until vao is built
  if (!m_builtVAO)
  {
    ngl::VAOPrimitives::draw("ground");
    ngl::VAOPrimitives::draw("cube");
  }

  if (m_builtVAO)
  {
    glFrontFace(GL_CW);   // Change winding order of m_vao to flip front face to point out
    m_vao->bind();
    m_vao->draw();
    m_vao->unbind();
  }
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // Track keystrokes
  m_keysPressed += static_cast<Qt::Key>(_event->key());

  // If keyboard inputs are necessary
  /*switch (_event->key())
  {
    default:
    {
      break;
    }
  }*/
    // Update the GLWindow and re-draw
    update();
}

void NGLScene::keyReleaseEvent(QKeyEvent *_event)
{
	// Untrack keystrokes
	m_keysPressed -= static_cast<Qt::Key>(_event->key());
}

void NGLScene::MoveCamera()
{
	float x = 0;
	float z = 0;

  if (m_camera == 1)
  {
    // Loop key strokes and query cases
    foreach(Qt::Key key, m_keysPressed)
    {
      switch (key)
      {
        case Qt::Key_A:
        {
          z = -0.1f;
          break;
        }

        case Qt::Key_D:
        {
          z = 0.1f;
          break;
        }

        case Qt::Key_W:
        {
          x = 0.1f;
          break;
        }

        case Qt::Key_S:
        {
          x = -0.1f;
          break;
        }

        default:
        {
          break;
        }
      }
    }
	}

	// If keys have been tracked move camera
	if (m_keysPressed.size() != 0)
  {
		m_fpsCamera.Move(x, z, m_deltaTime);
	}
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
	if (_event->timerId() == m_redrawTimer)
  {
		update();
	}

	if (_event->timerId() == m_cameraTimer)
  {
		MoveCamera();
	}
}

void NGLScene::BuildVAO()
{
  // Clear previous data
  if (m_vao != nullptr)
  {
    m_vao->removeVAO();
  }

  std::cout << "Building VAO...\n";
  int verticesEnd = m_vertexData.size();
  std::cout << "Calculating Normals...\n";
  for (int i = 0; i < m_vertexData.size() - 3; i += 3)
  {
    ngl::Vec3 normal = ngl::calcNormal(m_vertexData[i], m_vertexData[i + 1], m_vertexData[i + 2]);
    m_normals.push_back(normal);
    m_normals.push_back(normal);
    m_normals.push_back(normal);
  }
  std::cout << "Normals calculated!\n";
  std::cout << "Tranferring normals...\n";
  for (int i = 0; i < m_normals.size(); ++i)
  {
    m_vertexData.push_back(m_normals[i]);
  }
  std::cout << "Normals transferred!\n";
  m_vao = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES);
  m_vao->bind();
  m_vao->setData(ngl::SimpleVAO::VertexData(m_vertexData.size() * sizeof(ngl::Vec3), m_vertexData[0].m_x));
  m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);   // Vertices
  m_vao->setVertexAttributePointer(1, 3, GL_FLOAT, 0, verticesEnd * 3);   // Normals
  m_vao->setNumIndices(m_vertexData.size() / 2);
  m_vao->unbind();
  std::cout << "VAO built!\n";
}

void NGLScene::ExportToOBJ(std::string _exportPath, std::string _fileName)
{
  std::cout << "Exporting mesh to .obj file...\n";
  std::ofstream file;
  file.open(fmt::format(_exportPath + _fileName + ".obj"));
  std::stringstream ss;

  for (int i = 0; i < (m_vertexData.size() / 2) - 3; i += 3)    // Don't want to write normals, only vertices
  {
    ss << "v " << m_vertexData[i].m_x << " " << m_vertexData[i].m_y << " " << m_vertexData[i].m_z << " \n";
    ss << "v " << m_vertexData[i + 1].m_x << " " << m_vertexData[i + 1].m_y << " " << m_vertexData[i + 1].m_z << " \n";
    ss << "v " << m_vertexData[i + 2].m_x << " " << m_vertexData[i + 2].m_y << " " << m_vertexData[i + 2].m_z << " \n";
    ss << "f -1 -2 -3\n\n";   // Reverse face order from "-3 -2 -1", to flip front face to point out
  }

  file << ss.rdbuf();
  file.close();
  std::cout << "Exported!\n";
}

void NGLScene::ErrorMessage(std::string _type, std::string _line1, std::string _line2)
{
  std::cout << "==============================================\n"
            << _type << ":\n"
            << "      " << _line1 << '\n'
            << "      " << _line2 << '\n'
            << "==============================================\n";
}

void NGLScene::readImages()
{
  m_stack.ReadImages(m_imagesPath);
}

void NGLScene::checkImages()
{
  m_stack.CheckDimensions();
}

void NGLScene::sampleImages()
{
  m_stack.SampleImages();
}

void NGLScene::marchCubes()
{
  if (m_stack.CheckSampledImages())
  {
    m_vertexData.clear();   // Clear previous data
    m_mesh.Initialise(m_stack.m_sampledPoints, m_stack.GetImageWidth(), m_stack.GetImageHeight(), m_stack.GetSampleResolution());
    m_vertexData = m_mesh.MarchCubes();
  }
  else
  {
    ErrorMessage("MARCHING CUBES ERROR", "Cannot run Marching Cubes algorithm.", "Images must first be read, checked and sampled.");
  }
}

void NGLScene::generateMesh()
{
  if (!m_builtVAO)
  {
    if (m_vertexData.size() > 0)
    {
      BuildVAO();
      m_builtVAO = true;
    }
    else
    {
      ErrorMessage("MESH ERROR", "No vertex data to draw.");
    }
  }
  else
  {
    ErrorMessage("MESH ERROR", "Mesh has already been built.");
  }
}

void NGLScene::exportMesh()
{
  if (m_builtVAO)
  {
    if (!m_exported)
    {
      ExportToOBJ(m_exportPath, m_fileName);
      m_exported = true;
    }
    else
    {
      ErrorMessage("EXPORT ERROR", "Cannot export mesh.", "Mesh has already been exported.");
    }
  }
  else
  {
     ErrorMessage("EXPORT ERROR", "Cannot export mesh.", "Mesh has not been generated yet.");
  }
}

void NGLScene::setMeshColour()
{
  QColor colour = QColorDialog::getColor();
	if(colour.isValid())
	{
    ngl::ShaderLib::use(shaderProgram);
    ngl::ShaderLib::setUniform("albedo", static_cast<float>(colour.redF()), static_cast<float>(colour.greenF()), static_cast<float>(colour.blueF()));
    update();
	}
}

void NGLScene::setLightColour()
{
  QColor colour = QColorDialog::getColor();
	if(colour.isValid())
	{
    ngl::ShaderLib::use(shaderProgram);
    ngl::ShaderLib::setUniform("lightColor", static_cast<float>(colour.redF()), static_cast<float>(colour.greenF()), static_cast<float>(colour.blueF()));
    update();
	}
}

void NGLScene::setSampleResolution(int _resolution)
{
  m_stack.SetSampleResolution(_resolution);
  update();
}

void NGLScene::setSurfaceLevel(int _level)
{
  m_mesh.SetSurfaceLevel(_level);
  update();
}

void NGLScene::setMetallicness(double _metallicness)
{
  ngl::ShaderLib::use(shaderProgram);
  ngl::ShaderLib::setUniform("metallic", static_cast<float>(_metallicness));
  update();
}

void NGLScene::setRoughness(double _roughness)
{
  ngl::ShaderLib::use(shaderProgram);
  ngl::ShaderLib::setUniform("roughness", static_cast<float>(_roughness));
  update();
}

void NGLScene::setAO(double _ao)
{
  ngl::ShaderLib::use(shaderProgram);
  ngl::ShaderLib::setUniform("ao", static_cast<float>(_ao));
  update();
}

void NGLScene::setExposure(double _exposure)
{
  ngl::ShaderLib::use(shaderProgram);
  ngl::ShaderLib::setUniform("exposure", static_cast<float>(_exposure));
  update();
}

void NGLScene::setLightXPos(double _xpos)
{
  m_lightXPos = static_cast<float>(_xpos);
  ngl::ShaderLib::use(shaderProgram);
  ngl::ShaderLib::setUniform("lightPosition", static_cast<float>(-_xpos), m_lightYPos, m_lightZPos);
  update();
}

void NGLScene::setLightYPos(double _ypos)
{
  m_lightYPos = static_cast<float>(_ypos);
  ngl::ShaderLib::use(shaderProgram);
  ngl::ShaderLib::setUniform("lightPosition", -m_lightXPos, static_cast<float>(_ypos), m_lightZPos);
  update();
}

void NGLScene::setLightZPos(double _zpos)
{
  m_lightZPos = static_cast<float>(_zpos);
  ngl::ShaderLib::use(shaderProgram);
  ngl::ShaderLib::setUniform("lightPosition", -m_lightXPos, m_lightYPos, static_cast<float>(_zpos));
  update();
}

void NGLScene::setXRotation(double _x)
{
  m_rotation.m_x = _x;
  update();
}

void NGLScene::setYRotation(double _y)
{
  m_rotation.m_y = _y;
  update();
}

void NGLScene::setZRotation(double _z)
{
  m_rotation.m_z = _z;
  update();
}

void NGLScene::setUniformScale(double _scale)
{
  m_scale.m_x = _scale;
  m_scale.m_y = _scale;
  m_scale.m_z = _scale;
  update();
}

void NGLScene::setCamera(int _camera)
{
  m_camera = _camera;
  update();
}

void NGLScene::toggleWireframeMode(bool _mode)
{
  m_wireframe = _mode;
  update();
}

void NGLScene::toggleBackFaceCull(bool _mode)
{
  m_cull = _mode;
  update();
}