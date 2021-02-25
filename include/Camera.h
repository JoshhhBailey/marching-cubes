/// \brief Creation and management of cameras
/// \author Josh Bailey
/// \version 1.0
/// \date 13/01/21 Updated to NCCA Coding Standard
/// Revision History:
///
/// \todo

#ifndef CAMERA_H_
#define CAMERA_H_

#include <ngl/NGLStream.h>
#include <ngl/Mat4.h>
#include <ngl/ShaderLib.h>
#include <ngl/Vec3.h>

// The following class is adapted from:
// Macey, J., 2020. Camera Demo & First Person Camera Demo [online]
// Available from: https://github.com/NCCA/Camera
//                 https://github.com/NCCA/FirstPersonCamera
// Accessed [10 December 2020]

class Camera {
  public:
    Camera() = default;
    void Initialise(int _width, int _height, bool _cameraType);

    // View Matrix
    void SetViewMatrix(ngl::Vec3 _eye, ngl::Vec3 _target, ngl::Vec3 _upDir);
    ngl::Mat4 GetViewMatrix() { return m_viewMatrix; }
    ngl::Vec3 GetEye() { return m_eye; }
    void UpdateViewMatrix();

    // Projection Matrix
    void SetProjectionMatrix(float _fov, int _width, int _height, float _nearPlane, float _farPlane);
    ngl::Mat4 GetProjectionMatrix() { return m_projectionMatrix; }
    void UpdateProjectionMatrix();

    // Movement
    void updateCameraVectors() noexcept;
    void Move(float _x, float _z, float _deltaTime);
    void ProcessMovement(float _diffx, float _diffy, GLboolean _constrainPitch = true) noexcept;

  private:
    // Type: 0 = Static, 1 = FPS
    bool m_cameraType = 0;

    // View Matrix
    ngl::Mat4 m_viewMatrix;
    ngl::Vec3 m_eye;
    ngl::Vec3 m_target;
    ngl::Vec3 m_upDir;

    // Projection Matrix
    ngl::Mat4 m_projectionMatrix;
    float m_fov;
    int m_width;
    int m_height;
    float m_nearPlane;
    float m_farPlane;

    // Movement
    float m_moveSpeed;
    float m_lookSpeed;
    ngl::Vec3 m_front;
    ngl::Vec3 m_up;
    ngl::Vec3 m_right;
    ngl::Real m_yaw = 0.0f;
    ngl::Real m_pitch = 90.0f;
};

#endif  // _CAMERA_H_
