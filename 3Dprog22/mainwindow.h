#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QElapsedTimer>
#include <qlistwidget.h>
#include <QMainWindow>
#include "World.h"
#include "editor.h"
#include "RenderEngine.h"

class QWidget;
class RenderWindow;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

        
public:
    friend class Editor;
    explicit MainWindow(QWidget *parent = nullptr, World* world = nullptr);
    ~MainWindow();

private slots:

    void GameLoop();

    ///Slot called from the File->Exit menu in the mainwindow.ui file
    void on_fileExit_triggered();

    void on_worldHierList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void calculateFramerate(float& deltaTime);

private:
    ///called from the constructor. Initializes different parts of the program.
    void init();

    float deltaTime{ 0.f };
	
    Ui::MainWindow *ui{nullptr};                //the GUI of the app
    QWidget *mRenderWindowContainer{nullptr};   //Qt container for the RenderWindow
    RenderWindow *mRenderWindow{nullptr};       //The class that actually renders OpenGL

    QTimer* mUpdateTimer{nullptr};
    QElapsedTimer mTimeStart;               //time variable that reads the calculated FPS
    //
    //Logger class uses private ui pointer from this class
    friend class Logger;
	
    Editor* editor{ nullptr };
    World* world{ nullptr };
    RenderEngine* renderEngine{nullptr};
};

#endif // MAINWINDOW_H
