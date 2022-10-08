#include "renderwindow.h"
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>
#include <string>
#include "mainwindow.h"
#include "logger.h"
#include "lissajous.h"
#include "AudioManager.h"
#include "Include/ImGui/QtImGui.h"
#include "Include/ImGui/ImGuiRenderer.h"

RenderWindow* renderWindow = nullptr;

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mMainWindow(mainWindow)
{
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);

    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }
    renderWindow = this;
}

void RenderWindow::InitGame(World* world, Editor* editor)
{
    create();
	
    this->world = world;
    this->editor = editor;

    mLogger = Logger::getInstance();
   
    if (!mContext->makeCurrent(this)) {
        qDebug() << "makeCurrent() failed";
        return;
    }
	
    initializeOpenGLFunctions();
	
    mLogger->logText("The active GPU and API:", LogType::HIGHLIGHT);
    std::string tempString;
    tempString += std::string("  Vendor: ") + std::string((char*)glGetString(GL_VENDOR)) + "\n" +
        std::string("  Renderer: ") + std::string((char*)glGetString(GL_RENDERER)) + "\n" +
        std::string("  Version: ") + std::string((char*)glGetString(GL_VERSION));
    mLogger->logText(tempString);

    startOpenGLDebugger();
	
    AudioManager::Get(); // initing singleton

    QtImGui::initialize(this);

    bInitialized = true; 

    mUpdateTimer = new QTimer();

    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(GameLoop()));

    mTimeStart.start();
    mUpdateTimer->start(0);
}


void RenderWindow::GameLoop()
{
    long nsecElapsed = mTimeStart.nsecsElapsed();
    mTimeStart.restart();

    float deltaTime = nsecElapsed / 1000000000.f;
    this->deltatime = deltaTime;

    UpdateMouse();
    Update(deltaTime);
    Render(deltaTime);
}

void RenderWindow::UpdateMouse()
{
    editor->OnUpdateFromRenderWindow(deltatime, mKeysHeld, mMouseButtonsHeld, mX, mY, lastMX, lastMY);
}

void RenderWindow::Update(float deltaTime)
{
    QtImGui::newFrame();
    world->OnUpdate(deltaTime);
   
    world->OnMouseButtonHeldFromRenderWindow(&mMouseButtonsHeld, deltaTime);
    world->OnKeysHeld(&mKeysHeld, deltaTime);
}

void RenderWindow::Render(float deltaTime)
{
    mContext->makeCurrent(this);
    //The renderer will run from the worlds render loop to make code easier to use 
    world->OnRender(deltaTime);

    ImGui::Render();
    QtImGui::render();

    mContext->swapBuffers(this);
}

void RenderWindow::exposeEvent(QExposeEvent * e)
{
    if (!bInitialized) return;
    //This is just to support modern screens with "double" pixels (Macs and some 4k Windows laptops)
    const qreal retinaScale = devicePixelRatio();

    //Set viewport width and height to the size of the QWindow we have set up for OpenGL
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    editor->EditorCamera.SetAspect(float((float)width() * retinaScale) / float((float)height() * retinaScale));

    world->OnExposeEvent(e);
}

int RenderWindow::GetWidth()
{
    const qreal retinaScale = devicePixelRatio();
    return static_cast<GLint>(width() * retinaScale);
}

int RenderWindow::GetHeight()
{
    const qreal retinaScale = devicePixelRatio();
    return static_cast<GLint>(height() * retinaScale);
}

void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)  //if our machine got this class to work
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
        {
            if (!(message.type() == message.OtherType)) // get rid of uninteresting "object ...
                                                        // will use VIDEO memory as the source for
                                                        // buffer object operations"
                // valid error message:
                mLogger->logText(message.message().toStdString(), LogType::REALERROR);
        }
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            mLogger->logText("glGetError returns " + std::to_string(err), LogType::REALERROR);
            switch (err) {
            case 1280:
                mLogger->logText("GL_INVALID_ENUM - Given when an enumeration parameter is not a "
                                "legal enumeration for that function.");
                break;
            case 1281:
                mLogger->logText("GL_INVALID_VALUE - Given when a value parameter is not a legal "
                                "value for that function.");
                break;
            case 1282:
                mLogger->logText("GL_INVALID_OPERATION - Given when the set of state for a command "
                                "is not legal for the parameters given to that command. "
                                "It is also given for commands where combinations of parameters "
                                "define what the legal parameters are.");
                break;
            }
        }
    }
}

void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            mLogger->logText("This system can not use QOpenGLDebugLogger, so we revert to glGetError()",
                             LogType::HIGHLIGHT);

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            mLogger->logText("This system can log extended OpenGL errors", LogType::HIGHLIGHT);
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                mLogger->logText("Started Qt OpenGL debug logger");
        }
    }
}

RenderWindow* RenderWindow::Get()
{
    return renderWindow;
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    lastMX = event->globalPosition().x();
    lastMY = event->globalPosition().y();
	
	mX = event->globalPosition().x();
    mY = event->globalPosition().y();

    mMouseButtonsHeld[event->button()] = true;
	
    world->OnMousePressedFromRenderWindow(event);

    editor->OnMousePressedInRenderWindow(event);
	
    QWindow::mousePressEvent(event);
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    editor->OnMouseReleasedInRenderWindow(event);
    world->OnMouseReleasedFromRenderWindow(event);
    mMouseButtonsHeld[event->button()] = false;
	
    QWindow::mouseReleaseEvent(event);
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    world->OnMouseMoveFromRenderWindow(event);
	
    mX = event->globalPosition().x();
    mY = event->globalPosition().y();
	
	QWindow::mouseMoveEvent(event);
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        mMainWindow->close(); 
    }
    world->OnKeyPressedFromRenderWindow(event);
	
    mKeysHeld[event->key()] = true;
    QWindow::keyPressEvent(event);
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    world->OnKeyReleasedFromRenderWindow(event);
    mKeysHeld[event->key()] = false;
    QWindow::keyReleaseEvent(event);
}

RenderWindow::~RenderWindow()
{
    delete mUpdateTimer; mUpdateTimer = nullptr;
}
