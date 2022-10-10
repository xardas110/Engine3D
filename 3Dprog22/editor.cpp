#include "editor.h"

#include <csignal>
#include <iostream>
#include <ui_mainwindow.h>

#include "renderwindow.h"
#include "Components.h"
#include "Debug.h"
#include <qtreeview.h>
#include <QModelIndex>
#include <qobject.h>
#include <qfiledialog.h>
#include <string>
#include <qslider.h>
#include "DeferredRendererConfig.h"

#include "Include/glm/gtx/matrix_decompose.hpp"
#include "Include/glm/gtx/euler_angles.hpp"
#include "Components.h"
#include "DeferredRenderer.h"

#define mapsPath "..3Dprog22/Assets/Maps/"
#define assetsPath = "../3Dprog22/Assets/"

Editor::Editor(RenderWindow* renderWindow, MainWindow* mainWindow, World* world)
    :mRenderWindow(renderWindow), mMainWindow(mainWindow), world(world)
{
    this->ui = mMainWindow->ui;
}

void Editor::Init()
{
    EditorCamera.SetCameraPosition({0.f, 200.f, 10.f});
    EditorCamera.SetFar(3000.f);
    EditorCamera.SetCameraSpeed(100.f);

    HideComponents();

    InitializeEntityEvents();
    InitializePlayButton();
    InititalizeResetButton();
    InitializeContentBrowser();
    InitializeSaveButton();
    InitializeWorldSettings();
    InitializeTransformComponent();
    InitializeDirlightSettings();
    InitializePathfindingButtons();  
    InitGameModes();
}

void Editor::PostWorldInit()
{
    InitGameModeUI();

    ui->gmCB->setCurrentIndex(1);
    world->StartGameMode(ui->gmCB->currentIndex());
}

void Editor::HideComponents() const
{
	 ui->entityName->hide();	
     ui->transformComponent->hide();
}

void Editor::IntersectClosestEntity(float mouseX, float mouseY)
{
    /*
    const glm::vec3 dir = ConvertMouseScreenSpaceToWorldDir(mouseX, mouseY,
        glm::inverse(world->renderCamera->GetViewMatrix()),
        glm::inverse(world->renderCamera->GetProjectionMatrix()),
        mRenderWindow->width(), mRenderWindow->height());

    RayCast ray(world->renderCamera->GetPosition(), dir);
    std::vector<std::pair<int, CollisionVolume*>> volumes;
    world->collisionSystem.Query(&ray, volumes);

    auto cmp = [](const std::pair<int, CollisionVolume*>& a, const std::pair<int, CollisionVolume*>& b)
    {
        return a.first < b.first;
    };

    if (volumes.empty())
        return;

    std::sort(volumes.begin(), volumes.end(), cmp);

    const auto* firstIntersection = volumes.front().second;

    OnEntitySelected((entt::entity)firstIntersection->parentId);
    */
}

void Editor::OnEntitySelected(entt::entity entity)
{
    Entity sEntity(selectedEntity, world);
    Entity nEntity(entity, world);

    if (sEntity.IsValid())
    {
		if (sEntity.HasComponent<MeshComponent>())
		{
	        auto& meshComponent = sEntity.GetComponent<MeshComponent>();
	        meshComponent.mesh.bIsHighlighted = false;
		}
    }

    if (nEntity.HasComponent<MeshComponent>())
    {
        auto& meshComponent = nEntity.GetComponent<MeshComponent>();
        meshComponent.mesh.bIsHighlighted = true;
    }
	
    selectedEntity = entity;

    HideComponents();
	
    {//Tag component
        if (world->entRegistry.any_of<TagComponent>(selectedEntity))
        {
            ui->entityName->show();
            auto& tagComponent = world->entRegistry.get<TagComponent>(selectedEntity);
            ui->entityName->setTitle(std::string("Entity Selected: " + tagComponent.tag).c_str());
            
        }
    }

    {//Transform Component
        if (world->entRegistry.any_of<TransformComponent>(selectedEntity))
        {
            ui->transformComponent->show();
            auto& transformComponent = world->entRegistry.get<TransformComponent>(selectedEntity);
        	
            glm::vec3 scale;
            glm::vec3 position;
            glm::quat rotation;

            DecomposeMatrix(transformComponent.GetTransform(), position, rotation, scale);

            ui->posXEdit->setText(std::to_string(position.x).c_str());
            ui->posYEdit->setText(std::to_string(position.y).c_str());
            ui->posZEdit->setText(std::to_string(position.z).c_str());

            ui->scaleXEdit->setText(std::to_string(scale.x).c_str());
            ui->scaleYEdit->setText(std::to_string(scale.y).c_str());
            ui->scaleZEdit->setText(std::to_string(scale.z).c_str());

            const glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(rotation));

            ui->rotXEdit->setText(std::to_string(eulerAngles.x).c_str());
            ui->rotYEdit->setText(std::to_string(eulerAngles.y).c_str());
            ui->rotZEdit->setText(std::to_string(eulerAngles.z).c_str());

        }
    }
}

void Editor::InitializeEntityEvents()
{
    world->entRegistry.on_construct<TagComponent>().connect<&Editor::OnEntityCreated>(this);
    world->entRegistry.on_destroy<TagComponent>().connect<&Editor::OnEntityDestroyed>(this);
}

void Editor::InitializePlayButton()
{
    connect(ui->playButton, &QPushButton::clicked, this, &Editor::on_playButton_pressed);
}

void Editor::InititalizeResetButton()
{
    connect(ui->resetButton, &QPushButton::clicked, this, &Editor::on_resetButton_pressed);
}

void Editor::InitializeContentBrowser()
{
    static QString path = "../3Dprog22/Assets";

    directories = new QFileSystemModel(ui->ContentBrowser);
    directories->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    directories->setRootPath(path);
    ui->treeView->setModel(directories);

    files = new QFileSystemModel(ui->ContentBrowser);
    QStringList filters;
    filters << "*.world";
	
    files->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    files->setNameFilters(filters);
	//files->setProperty()
    files->setRootPath(path);
    ui->listView->setModel(files);

    ui->treeView->setRootIndex(directories->index(path));
    ui->listView->setRootIndex(files->index(path));

    connect(ui->treeView, &QTreeView::clicked, this, &Editor::on_treeView_clicked);
    connect(ui->listView, &QListView::doubleClicked, this, &Editor::on_fileView_clicked);

}

void Editor::InitializeSaveButton()
{
    connect(ui->saveButton, &QPushButton::clicked, this, &Editor::on_saveButton_clicked);
}


void Editor::InitializeWorldSettings()
{
    connect(ui->DrawDynamicOctree, &QCheckBox::clicked, this, &Editor::on_drawDynamicOctree_checked);
    connect(ui->DrawDebugBoxes, &QCheckBox::clicked, this, &Editor::on_drawDebugBoxes_checked);
}

void Editor::InitializeTransformComponent()
{
    connect(ui->posXEdit, &QLineEdit::editingFinished, [&]()
	{
        Entity ent(selectedEntity, world);
        auto currentPos = ent.GetPosition();
        currentPos.x = ui->posXEdit->text().toFloat();

        ent.SetPosition(currentPos);
    });
	
    connect(ui->posYEdit, &QLineEdit::editingFinished, [&]()
    {
        Entity ent(selectedEntity, world);
        auto currentPos = ent.GetPosition();
        currentPos.y = ui->posYEdit->text().toFloat();

        ent.SetPosition(currentPos);
    });
	
    connect(ui->posZEdit, &QLineEdit::editingFinished, [&]()
    {
        Entity ent(selectedEntity, world);
        auto currentPos = ent.GetPosition();
        currentPos.z = ui->posZEdit->text().toFloat();

        ent.SetPosition(currentPos);
    });

    connect(ui->scaleXEdit, &QLineEdit::editingFinished, [&]()
        {
            Entity ent(selectedEntity, world);
            auto newScale = ent.GetScale();
            newScale.x = ui->scaleXEdit->text().toFloat();
            ent.SetScale(newScale);
            
        });
	
    connect(ui->scaleYEdit, &QLineEdit::editingFinished, [&]()
        {
            Entity ent(selectedEntity, world);
            auto newScale = ent.GetScale();
            newScale.y = ui->scaleYEdit->text().toFloat();
            ent.SetScale(newScale);
        });
	
    connect(ui->scaleZEdit, &QLineEdit::editingFinished, [&]()
        {
            Entity ent(selectedEntity, world);
            auto newScale = ent.GetScale();
            newScale.z = ui->scaleZEdit->text().toFloat();
            ent.SetScale(newScale);
        });

    connect(ui->rotXEdit, &QLineEdit::editingFinished, [&]()
        {
            Entity ent(selectedEntity, world);

            auto rotX(glm::identity<glm::quat>());
            auto rotY(glm::identity<glm::quat>());
            auto rotZ(glm::identity<glm::quat>());

            rotX = glm::rotate(rotX, glm::radians(ui->rotXEdit->text().toFloat()), { 1.f, 0.f, 0.f });
            rotY = glm::rotate(rotY, glm::radians(ui->rotYEdit->text().toFloat()), { 0.f, 1.f, 0.f });
            rotZ = glm::rotate(rotZ, glm::radians(ui->rotZEdit->text().toFloat()), { 0.f, 0.f, 1.f });

            const auto finalRotation = rotZ * rotY * rotX;

            ent.SetRotation(finalRotation);
        });
	
    connect(ui->rotYEdit, &QLineEdit::editingFinished, [&]()
        {
            Entity ent(selectedEntity, world);

            auto rotX(glm::identity<glm::quat>());
            auto rotY(glm::identity<glm::quat>());
            auto rotZ(glm::identity<glm::quat>());

            rotX = glm::rotate(rotX, glm::radians(ui->rotXEdit->text().toFloat()), { 1.f, 0.f, 0.f });
            rotY = glm::rotate(rotY, glm::radians(ui->rotYEdit->text().toFloat()), { 0.f, 1.f, 0.f });
            rotZ = glm::rotate(rotZ, glm::radians(ui->rotZEdit->text().toFloat()), { 0.f, 0.f, 1.f });

            const auto finalRotation = rotZ * rotY * rotX;

            ent.SetRotation(finalRotation);
        });
	
    connect(ui->rotZEdit, &QLineEdit::editingFinished, [&]()
        {
            Entity ent(selectedEntity, world);

            auto rotX(glm::identity<glm::quat>());
            auto rotY(glm::identity<glm::quat>());
            auto rotZ(glm::identity<glm::quat>());

            rotX = glm::rotate(rotX, glm::radians(ui->rotXEdit->text().toFloat()), { 1.f, 0.f, 0.f });
            rotY = glm::rotate(rotY, glm::radians(ui->rotYEdit->text().toFloat()), { 0.f, 1.f, 0.f });
            rotZ = glm::rotate(rotZ, glm::radians(ui->rotZEdit->text().toFloat()), { 0.f, 0.f, 1.f });

            const auto finalRotation = rotZ * rotY * rotX;

            ent.SetRotation(finalRotation);
        });
    
}

void Editor::InitializeDirlightSettings()
{
    //connect(ui->horizontalSlider, &QSlider::sliderMoved, this, &Editor::on_dirlight_itensity_slider_moved);
}

void Editor::InitializePathfindingButtons()
{
    connect(ui->pf1, &QCheckBox::clicked, this, &Editor::on_ShowPathOccfinding_checked);
    connect(ui->pf2, &QCheckBox::clicked, this, &Editor::on_ShowPathfinding_checked);

    auto& ws = world->worldSettings;
    connect(ui->worldAxises, &QCheckBox::clicked, [&](bool checked) {
            ws.bShowWorldAxises = checked;
        });

}

void Editor::OnUpdateGUIFromRenderWindow(float deltaTime)
{	
    if (selectedEntity == entt::null)
        return;
}


void Editor::InitGameModeUI()
{
    connect(ui->gmStart, &QPushButton::clicked, [&]()
        {
            if (!world) return;
            world->StartGameMode(ui->gmCB->currentIndex());
        });
}

void Editor::InitGameModes()
{
    ui->gmCB->addItem("2VISM");
}

void Editor::on_worldHierList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (current)
		OnEntitySelected((entt::entity)current->data(1).toUInt());
}

void Editor::OnEntityCreated(entt::registry& registry, entt::entity entity)
{
    auto* widgetItem = new QListWidgetItem();
    widgetItem->setData(1, (std::uint32_t)entity);
    widgetItem->setText(registry.get<TagComponent>(entity).tag.c_str());
    ui->worldHierList->addItem(widgetItem);
}

void Editor::OnEntityDestroyed(entt::registry& registry, entt::entity entity)
{
    for (auto i = 0; i < ui->worldHierList->count(); i++)
    {
        auto* item = ui->worldHierList->item(i);
    	if (item->data(1).toUInt() == (std::uint32_t)entity)
    	{
            ui->worldHierList->removeItemWidget(item);
            delete item;
    	}
    }
}

void Editor::on_treeView_clicked(const QModelIndex &index)
{
    QString path = directories->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(files->setRootPath(path));
}

void Editor::on_fileView_clicked(const QModelIndex& index)
{
    const QString filePath = files->fileInfo(index).filePath();
    const QString suffix = files->fileInfo(index).completeSuffix();
    
    if (suffix == "world")
    {
        world->LoadWorld(filePath.toStdString());
    }
	
}

void Editor::on_saveButton_clicked()
{
    QString filters("World file (*.world)");

    /* Static method approach */
   QString path =  QFileDialog::getSaveFileName(0, "Save file", QDir::currentPath().append("/Assets/World"),
        filters, nullptr);

   world->SaveWorld(path.toStdString());
}

void Editor::on_drawDynamicOctree_checked()
{
	if (ui->DrawDynamicOctree->isChecked())
        world->worldSettings.bDrawDynamicOctree = true;
    else
        world->worldSettings.bDrawDynamicOctree = false;
}

void Editor::on_drawDebugBoxes_checked()
{
    if (ui->DrawDebugBoxes->isChecked())
        world->worldSettings.bDrawDebugBoxes = true;
    else
        world->worldSettings.bDrawDebugBoxes = false;
}

void Editor::on_ShowPathfinding_checked()
{

    world->worldSettings.bShowPathfinding = ui->pf2->isChecked();

}

void Editor::on_ShowPathOccfinding_checked()
{
    std::cout << "on_ShowPathOccfinding_checked" << std::endl;

    if (ui->pf1->isChecked())
        world->worldSettings.bShowPathfindingOccupied = true;
    else
        world->worldSettings.bShowPathfindingOccupied = false;
}

void Editor::on_playButton_pressed()
{
    //Oppgave 6 begin og endplay, laget som slot
    if (state == EditorState::Editor)
    {
        state = EditorState::Play;
        world->OnBeginPlay();      
        ui->playButton->setText("End Play");
    }
    else 
    {
        state = EditorState::Editor;
        world->OnEndPlay();
        world->SetRenderCamera(&EditorCamera);
        ui->playButton->setText("Begin Play");   
    }
}

void Editor::on_resetButton_pressed()
{
    ui->playButton->setEnabled(false);
    world->StartGameMode(ui->gmCB->currentIndex());
    ui->playButton->setEnabled(true);
}

void Editor::on_dirlight_itensity_slider_moved(int moveVal)
{
    world->skySystem.SetLightItensity(moveVal);
}

void Editor::OnUpdateFromRenderWindow(float deltaTime, std::map<int, bool> keyPressed, std::map<int, bool> mousePressed, double mouseX, double mouseY, double lastX, double lastY)
{
    if (EditorState::Editor != state) return;

    mx = mouseX;
    my = mouseY;

    float forward{}, right{}, up{};
    if (keyPressed[Qt::Key_W])
    {
        forward += 1.f;
    }
    if (keyPressed[Qt::Key_S])
    {
        forward -= 1.f;
    }
    if (keyPressed[Qt::Key_D])
    {
        right += 1.f;
    }
    if (keyPressed[Qt::Key_A])
    {
        right -= 1.f;
    }
    //Oppgave 5a
    if (keyPressed[Qt::Key_Q])
    {
        up -= 1.f;
    }
    //Oppgave 5a
    if (keyPressed[Qt::Key_E])
    {
        up += 1.f;
    }

    if (mousePressed[Qt::RightButton])
    {
        const double dX = (mouseX - lastX);
        const double dY = (mouseY - lastY);

        QCursor::setPos({ (int)lastX, (int)lastY });
        EditorCamera.ProcessMouseMovement(dX, dY);
    }

    EditorCamera.MoveForward(forward, deltaTime);
    EditorCamera.MoveRight(right, deltaTime);

    EditorCamera.MoveUp(up, deltaTime);

    EditorCamera.UpdateCamera(deltaTime);

    OnUpdateGUIFromRenderWindow(deltaTime);
}

void Editor::OnMousePressedInRenderWindow(QMouseEvent* event)
{
    if (EditorState::Editor != state) return;

    if (event->button() == Qt::LeftButton)
    {
        IntersectClosestEntity(event->x(), event->y());
    }
}

void Editor::OnMouseReleasedInRenderWindow(QMouseEvent* event)
{
    if (EditorState::Editor != state) return;
}

void Editor::OnKeyPressedInRenderWindow(QKeyEvent* event)
{
    if (event->key() == Qt::Key_F11)
    {
        bHideEditor = !bHideEditor;
        ui->splitter_6->setHidden(bHideEditor);
        ui->tabWidget->setHidden(bHideEditor);
        ui->frame->setHidden(bHideEditor);
    }
}
