///
/// @file NGLScene.cpp
/// @brief Handle mouse controls

#include <QMouseEvent>

#include <ngl/NGLStream.h>

#include "NGLScene.h"


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent(QMouseEvent* _event)
{
  // Difference in X
  m_win.curr_x = _event->x();
  float diffx = _event->x() - m_win.prev_x;
  m_win.prev_x = m_win.curr_x;

  // Difference in Y
  m_win.curr_y = _event->y();
  float diffy = _event->y() - m_win.prev_y;
  m_win.prev_y = m_win.curr_y;

  if (m_camera == 1)
  {
    m_fpsCamera.ProcessMovement(diffx, diffy);
  }
  update();
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent(QMouseEvent* _event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent(QMouseEvent* _event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent* _event)
{

}
