#include "mainwindow.h"

#include <iostream>

#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDebug>

#include "renderwindow.h"
#include "logger.h"
#include "RenderDebugger.h"

MainWindow::MainWindow(QWidget *parent, World* world) :
    QMainWindow(parent), ui(new Ui::MainWindow), world(world)
{
    //this sets up what's in the mainwindow.ui - the GUI
    ui->setupUi(this);
    init(); //initializes parts of the program
}

MainWindow::~MainWindow()
{
	//Mainwindow has currently ownership of the game.
    delete world;
    delete editor;
	
    delete mRenderWindow;
    delete ui;
}

void MainWindow::init()
{
    //This will contain the setup of the OpenGL surface we will render into
    QSurfaceFormat format;

    //OpenGL v 4.1 - (Ole Flatens Mac does not support higher than this - sorry!)
    //you can try other versions, but then have to update RenderWindow and Shader
    //to inherit from other than QOpenGLFunctions_4_1_Core
    //(The 3DProgramming course does not use anything from higher than 4.1, so pleas don't change it)
    format.setVersion(4, 5);
    //Using the main profile for OpenGL - no legacy code permitted
    format.setProfile(QSurfaceFormat::CoreProfile);
    //A QSurface can be other types than OpenGL
    format.setRenderableType(QSurfaceFormat::OpenGL);

    //This should activate OpenGL debug Context used in RenderWindow::startOpenGLDebugger().
    //This line (and the startOpenGLDebugger() and checkForGLerrors() in RenderWindow class)
    //can be deleted, but it is nice to have some OpenGL debug info!
    format.setOption(QSurfaceFormat::DebugContext);

    // The surface will need a depth buffer - (not requiered to set in glfw-tutorials)
    format.setDepthBufferSize(24);
    format.setSwapInterval(0);

    //Just prints out what OpenGL format we try to get
    // - this can be deleted
    qDebug() << "Requesting surface format: " << format;

    //We have a surface format for the OpenGL window, so let's make it:
    mRenderWindow = new RenderWindow(format, this);

    //Check if renderwindow did initialize, else prints error and quit
    if (!mRenderWindow->context()) {
        qDebug() << "Failed to create context. Can not continue. Quits application!";
        delete mRenderWindow;
        return;
    }

    //The OpenGL RenderWindow got made, so continuing the setup:
    //We put the RenderWindow inside a QWidget so we can put in into a
    //layout that is made in the .ui-file
    // (Qt has a QOpenGLWidget but that forces us to use more Qt specific OpenGL-stuff
    //  So we make a renderwindow manually to get more freedom)
    mRenderWindowContainer = QWidget::createWindowContainer(mRenderWindow);
    //OpenGLLayout is made in the mainwindow.ui-file!
    ui->OpenGLLayout->addWidget(mRenderWindowContainer);

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    mRenderWindowContainer->setFocus();

    //feed in MainWindow to the logger - have to be done, else logger will crash program
    Logger::getInstance()->setMainWindow(this);

    mUpdateTimer = new QTimer();
	
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(GameLoop()));
    
    editor = new Editor(mRenderWindow, this, world);
    
    mRenderWindow->InitGame(world, editor);
    
    renderEngine = new RenderEngine();

    editor->Init();
    world->OnInit(&editor->EditorCamera);

    editor->PostWorldInit();

    mTimeStart.start();
    mUpdateTimer->start(0);
}

void MainWindow::GameLoop()
{
    mTimeStart.restart();
    mRenderWindow->Update(deltaTime);
    //RenderDebugger::Get()->Update(deltaTime);
    mRenderWindow->Render(deltaTime);
    //RenderDebugger::Get()->Render();
    calculateFramerate(deltaTime);  
}

void MainWindow::calculateFramerate(float& deltaTime)
{
    long nsecElapsed = mTimeStart.nsecsElapsed();

    deltaTime = nsecElapsed / 1000000000.f;

    static int frameCount{ 0 };                       //counting actual frames for a quick "timer" for the statusbar

    ++frameCount;
    if (frameCount > 30)    //once pr 30 frames = update the message == twice pr second (on a 60Hz monitor)
    {
        //showing some statistics in status bar
        this->statusBar()->showMessage(" Time pr FrameDraw: " +
            QString::number(nsecElapsed / 1000000.f, 'g', 4) + " ms  |  " +
            "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
        frameCount = 0;     //reset to show a new message in 30 frames
    }   
}

//File menu Exit closes the program
void MainWindow::on_fileExit_triggered()
{
    close();       //Shuts down the whole program
}

void MainWindow::on_worldHierList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    editor->on_worldHierList_currentItemChanged(current, previous);
}
