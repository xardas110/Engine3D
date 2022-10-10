#ifndef EDITOR_H
#define EDITOR_H
#include "mainwindow.h"
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include "Camera.h"
#include "Entity.h"
#include <QVBoxLayout>
#include <qlistwidget.h>
#include <QFileSystemModel>
#include "ui_mainwindow.h"

class DeferredRendererConfig;

namespace EditorState
{
	enum EditorState
	{
		Editor,
		Play
	};
}

class Editor : public QObject
{
    friend class RenderWindow;
    friend class MainWindow;
public:
    Editor(RenderWindow* mMainWindow, MainWindow* mainWindow, World* world);

    void Init();
    void PostWorldInit();

    void OnUpdateFromRenderWindow(float deltaTime, std::map<int, bool> keyPressed, std::map<int, bool> mousePressed, double mouseX, double mouseY, double lastX, double lastY);
    void OnMousePressedInRenderWindow(QMouseEvent* event);
    void OnMouseReleasedInRenderWindow(QMouseEvent* event);
    void OnUpdateGUIFromRenderWindow(float deltaTime);
    void OnKeyPressedRenderWindow(QKeyEvent* event);

    Camera EditorCamera;
private slots:
    void on_treeView_clicked(const QModelIndex &index);
    void on_fileView_clicked(const QModelIndex& index);

    void on_saveButton_clicked();

    void on_drawDynamicOctree_checked();
    void on_drawDebugBoxes_checked();

    void on_ShowPathfinding_checked();
    void on_ShowPathOccfinding_checked();

    void on_playButton_pressed();
    void on_resetButton_pressed();

    void on_dirlight_itensity_slider_moved(int moveVal);
private:
    EditorState::EditorState state = EditorState::Editor;
	
    float mx{ 0.f }, my{ 0.f };
    //weak ptrs, no ownership;
    RenderWindow* mRenderWindow;
    MainWindow* mMainWindow;
    World* world;
    class Ui::MainWindow* ui{ nullptr };
	
    QFileSystemModel* directories;
    QFileSystemModel* files;
   
    entt::entity selectedEntity;

    void InitializeEntityEvents();
    void InitializePlayButton();
    void InititalizeResetButton();
    void InitializeContentBrowser();
    void InitializeSaveButton();
    void InitializeAddComponent();
    void InitializeWorldSettings();
    void InitializeTransformComponent();
    void InitializeDirlightSettings();
    void InitializePathfindingButtons();
    void InitRenderSettings();
    void InitRenderConfigGUI();
    void InitGameModeUI();
    void InitGameModes();

    void IntersectClosestEntity(float mouseX, float mouseY);
    
    void HideComponents() const;

    // callback from this
    void OnEntitySelected(entt::entity entity);
    void OnEntityCreated(entt::registry& registry, entt::entity entity);
    void OnEntityDestroyed(entt::registry& registry, entt::entity entity);
	
    //callbacks from main window
    void on_worldHierList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);    
};

#endif // EDITOR_H
