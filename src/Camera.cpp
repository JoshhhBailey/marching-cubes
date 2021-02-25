///
/// @file Camera.cpp
/// @brief Creation and management of cameras

#include "Camera.h"

void Camera::Initialise(int _width, int _height, bool _cameraType)
{
  // Initialise View Matrix
  m_eye = ngl::Vec3{0.0f, 100.0f, -150.0f};
  m_target = ngl::Vec3{0.0f, 0.0f, 0.0f};
  m_upDir = ngl::Vec3{0.0f, 1.0f, 0.0f};
  m_viewMatrix = ngl::lookAt(m_eye, m_target, m_upDir);

  // Initialise Projection Matrix
  m_fov = 45.0f;
  m_width = _width;
  m_height = _height;
  m_nearPlane = 0.1f;
  m_farPlane = 5000.0f;
  m_projectionMatrix = ngl::perspective(m_fov, static_cast<float>(m_width) / m_height, m_nearPlane, m_farPlane);

  // Movement
  m_moveSpeed = 50.0f;
  m_lookSpeed = 0.5f;

  // FPS Camera
  if (m_cameraType == 1)
  {
    updateCameraVectors();
  }
}

void Camera::SetViewMatrix(ngl::Vec3 _eye, ngl::Vec3 _target, ngl::Vec3 _upDir)
{
  m_eye = _eye;
  m_target = _target;
  m_upDir = _upDir;
  UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
  m_viewMatrix = ngl::lookAt(m_eye, m_target, m_upDir);
}

void Camera::SetProjectionMatrix(float _fov, int _width, int _height, float _nearPlane, float _farPlane)
{
  m_fov = _fov;
  m_width = _width;
  m_height = _height;
  m_nearPlane = _nearPlane;
  m_farPlane = _farPlane;
  UpdateProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
  m_projectionMatrix = ngl::perspective(m_fov, static_cast<float>(m_width) / m_height, m_nearPlane, m_farPlane);
}

void Camera::updateCameraVectors() noexcept
{
  // Calculate new vectors and normalize
  // them to maintain a consistent speed
  // when their lengths get closer to 0 (top / bottom).
  m_front.m_x = cosf(ngl::radians(m_yaw)) * cosf(ngl::radians(-m_pitch));
  m_front.m_y = sinf(ngl::radians(-m_pitch));
  m_front.m_z = sinf(ngl::radians(m_yaw)) * cosf(ngl::radians(-m_pitch));
  m_front.normalize();

  m_right = m_front.cross(m_upDir);
  m_right.normalize();
  m_front.normalize();
  m_target = m_eye + m_front;

  UpdateViewMatrix();
}

void Camera::Move(float _x, float _z, float _deltaTime)
{
  float velocity = m_moveSpeed * _deltaTime;
  m_eye += m_front * velocity * _x;
  m_eye += m_right * velocity * _z;
  updateCameraVectors();
}

void Camera::ProcessMovement(float _diffx, float _diffy, GLboolean _constrainPitch) noexcept
{
    _diffx *= m_lookSpeed;
    _diffy *= m_lookSpeed;

    m_yaw += _diffx;
    m_pitch += _diffy;

    // When pitch is out of range, stop screen flipping
    if (_constrainPitch)
    {
      if (m_pitch > 89.0f)
        m_pitch = 89.0f;
      if (m_pitch < -89.0f)
        m_pitch = -89.0f;
    }
    updateCameraVectors();
}
