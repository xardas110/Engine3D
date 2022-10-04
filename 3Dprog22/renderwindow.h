#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QElapsedTimer>
#include "World.h"
#include "editor.h"

class QOpenGLContext;
class Shader;
class MainWindow;

/// This inherits from QWindow to get access to the Qt functionality and
// OpenGL surface.
// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
// This is the same as using "glad" and "glw" from general OpenGL tutorials
class RenderWindow : public QWindow, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
        friend class MainWindow;
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow() override;

    QOpenGLContext *context() { return mContext; }

    void exposeEvent(QExposeEvent *) override;  //gets called when app is shown and resized

    int GetWidth();
    int GetHeight();

private slots:

private:
    void InitGame(World* world, Editor* editor);
	
    void Update(float deltaTime);
    void Render(float deltaTime);
	
    QOpenGLContext *mContext{nullptr};  //Our OpenGL context

    MainWindow *mMainWindow{nullptr};        //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};  //helper class to get some clean debug info from OpenGL
    class Logger *mLogger{nullptr};         //logger - Output Log in the application

    bool bInitialized{ false };
	
    ///Helper function that uses QOpenGLDebugLogger or plain glGetError()
    void checkForGLerrors();
       
    ///Starts QOpenGLDebugLogger if possible
    void startOpenGLDebugger();

    /// <summary>
    /// Should be weak pointers, main window will have all ownerships
    /// </summary>
    World* world{ nullptr };
    Editor* editor{ nullptr };   
protected:
    std::map<int, bool> mKeysHeld;
    std::map<int, bool> mMouseButtonsHeld;
	
    double mX{}, mY{};
    double lastMX{}, lastMY{};

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;              
    void keyReleaseEvent(QKeyEvent *event) override;
	
    //    void wheelEvent(QWheelEvent *event) override{}
public:
    static RenderWindow* Get();
};

#endif // RENDERWINDOW_H
