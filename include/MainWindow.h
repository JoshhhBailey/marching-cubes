/// \file MainWindow.h
/// \brief Qt main window
/// \author Josh Bailey
/// \version 1.0
/// \date 16/01/21 Updated to NCCA Coding Standard
/// Revision History:
///
/// \todo

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "NGLScene.h"

namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  private:
    Ui::MainWindow *m_ui;
	  NGLScene *m_gl;

  private slots:
    // Buttons
    void on_m_readImages_btn_clicked();
    void on_m_exportMesh_btn_clicked();
};

#endif // _MAINWINDOW_H
