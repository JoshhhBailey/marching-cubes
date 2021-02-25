/// \file NGLScene.h
/// \brief Main program loop
/// \author Josh Bailey
/// \version 1.0
/// \date 16/01/21 GUI changes
/// Revision History:
///
/// \todo

#ifndef NGLSCENE_H_
#define NGLSCENE_H_

#include <ngl/AbstractVAO.h>
#include <ngl/Mat4.h>
#include <ngl/Transformation.h>
#include <ngl/Vec3.h>

// this must be included after NGL includes else we get a clash with gl libs
#include <QEvent>
#include <QOpenGLWidget>
#include <QResizeEvent>
#include <QSet>

#include "Camera.h"
#include "ImageStack.h"
#include "Mesh.h"
#include "Timer.h"
#include "WindowParams.h"

class NGLScene : public QOpenGLWidget
{
  Q_OBJECT  // Use of signals and slots
  public:
    NGLScene(QWidget *_parent);
    ~NGLScene() override;
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int _w, int _h) override;

    // GUI paths and names
    void setImagePath(std::string _imagesPath) { m_imagesPath = _imagesPath; }
    void setExportPath(std::string _exportPath) { m_exportPath = _exportPath; }
    void setFileName(std::string _fileName) { m_fileName = _fileName; }
  
  public slots:
    // Buttons
    void readImages();
    void checkImages();
    void sampleImages();
    void marchCubes();
    void generateMesh();
    void exportMesh();
    void setMeshColour();
    void setLightColour();

    // Inputs
    void setSampleResolution(int _resolution);
    void setSurfaceLevel(int _level);
    void setMetallicness(double _metallicness);
    void setRoughness(double _roughness);
    void setAO(double _ao);
    void setExposure(double _exposure);
    void setLightXPos(double _xpos);
    void setLightYPos(double _ypos);
    void setLightZPos(double _zpos);

    // Rotation
    void setXRotation(double _x);
	  void setYRotation(double _y);
  	void setZRotation(double _z);

    // Scale
    void setUniformScale(double _scale);

    // Cameras
    void setCamera(int _camera);

    // Toggles
    void toggleWireframeMode(bool _mode);
    void toggleBackFaceCull(bool _mode);

  private:
    void keyPressEvent(QKeyEvent *_event) override;
    void keyReleaseEvent(QKeyEvent *_event) override;
    void mouseMoveEvent (QMouseEvent * _event) override;
    void mousePressEvent (QMouseEvent *_event) override;
    void mouseReleaseEvent (QMouseEvent *_event ) override;
    void wheelEvent(QWheelEvent *_event) override;

    // Called when timers trigger, update and draw
    void timerEvent(QTimerEvent *) override;

    // window parameters for mouse control etc.
    WinParams m_win;

    // Delta time
    Timer m_timer;
    float m_deltaTime;

    // Update timers
    int m_redrawTimer;
    int m_cameraTimer;

    // Cameras
    Camera m_fpsCamera;
    Camera m_staticCamera;
    ngl::Mat4 m_viewMatrix;
    ngl::Mat4 m_projectionMatrix;
    int m_camera = 0;
    ngl::Vec3 m_eye;
    void MoveCamera();

    // Store keystrokes
    QSet<Qt::Key> m_keysPressed;

    // Toggles
    bool m_wireframe = false;
    bool m_cull = false;

    // Stack
    ImageStack m_stack;
    std::string m_imagesPath;

    // Store all mesh data
    Mesh m_mesh;
    std::vector<ngl::Vec3> m_vertexData;
    std::vector<ngl::Vec3> m_normals;

    // Export
    bool m_exported = false;
    std::string m_exportPath;
    std::string m_fileName;
    void ExportToOBJ(std::string _exportPath, std::string _fileName);

    // VAO
    void BuildVAO();
    bool m_builtVAO = false;
    std::unique_ptr<ngl::AbstractVAO> m_vao;

    // Transformations to pass to the shader
    ngl::Mat4 m_tx;
    ngl::Transformation m_transform;
    ngl::Vec3 m_rotation = 0.0;
    ngl::Vec3 m_scale = 1.0;

    void LoadMatrixToShader();

    // Light position
    float m_lightXPos = 0.0f;
    float m_lightYPos = 100.0f;
    float m_lightZPos = 0.0f;
    
    // Process errors
    void ErrorMessage(std::string _type, std::string _line1, std::string _line2 = "");
};

#endif  // _NGLSCENE_H_
