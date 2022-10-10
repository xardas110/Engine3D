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
    InitializeAddComponent();
    InitializeWorldSettings();
    InitializeTransformComponent();
    InitializeDirlightSettings();
    InitializePathfindingButtons();  
    InitGameModes();
}

void Editor::PostWorldInit()
{
    InitRenderSettings();
    InitRenderConfigGUI();
    InitGameModeUI();

    ui->gmCB->setCurrentIndex(5);
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

    ui->AddComponentButton->setDisabled(false);
	
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

void Editor::InitializeAddComponent()
{
    ui->ComponentTypes->addItem("BoxCollision Component");
    ui->ComponentTypes->addItem("Physics Component");
    ui->AddComponentButton->setDisabled(true);
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

void Editor::InitRenderSettings()
{
    if (!world) return;
    
    auto& config = world->deferedRenderer->config;
       
    ui->GI->setChecked(config.gi.enabled);

    ui->volumetricLight->setCheckable(config.volumetricLight.enabled);
    ui->vlRaymarching->setValue(config.volumetricLight.numRays);
    ui->vlScattering->setValue(config.volumetricLight.scattering);
    ui->vlLightColor0->setValue(config.volumetricLight.sunColor[0]);
    ui->vlLightColor1->setValue(config.volumetricLight.sunColor[1]);
    ui->vlLightColor2->setValue(config.volumetricLight.sunColor[2]);

    ui->volumetricClouds->setChecked(config.volumetricClouds.enabled);
    ui->vcCloudColor0->setValue(config.volumetricClouds.cloudBotColor[0]);
    ui->vcCloudColor1->setValue(config.volumetricClouds.cloudBotColor[1]);
    ui->vcCloudColor2->setValue(config.volumetricClouds.cloudBotColor[2]);
    ui->vcCoverage->setValue(config.volumetricClouds.cloudCoverage);
    ui->vcCrispiness->setValue(config.volumetricClouds.cloudFluffyness);
    ui->vcDensity->setValue(config.volumetricClouds.cloudDensity);
    ui->vcSpeed->setValue(config.volumetricClouds.cloudSpeed);
    ui->vcAIR->setValue(config.volumetricClouds.atmosphereInnerRadius);
    ui->vcAOR->setValue(config.volumetricClouds.atmosphereOuterRadius);
    ui->vcCloudRaySteps->setValue(config.volumetricClouds.numCloudRaySteps);
    ui->vcShadowRays->setValue(config.volumetricClouds.numShadowRaySteps);
    ui->vcVolumetricLightRays->setValue(config.volumetricClouds.numVolumetricLightRaySteps);
    ui->vcVLScattering->setValue(config.volumetricClouds.volumetricLightScattering);
    ui->vcLightAbsorption->setValue(config.volumetricClouds.lightAbsorption);
    ui->vcLightAbsorption->setEnabled(true);

    ui->vcDownScale->setValue(config.volumetricClouds.downScale);
    ui->vcDownScale->setEnabled(true);

    ui->vcEarthRadius->setValue(config.volumetricClouds.earthRadius);  
    ui->vcWD0->setValue(config.volumetricClouds.windDirection.x);
    ui->vcWD1->setValue(config.volumetricClouds.windDirection.y);
    ui->vcWD2->setValue(config.volumetricClouds.windDirection.z);
    ui->vcVLCol0->setValue(config.volumetricClouds.vlLightColor.x);
    ui->vcVLCol1->setValue(config.volumetricClouds.vlLightColor.y);
    ui->vcVLCol2->setValue(config.volumetricClouds.vlLightColor.z);

    ui->skyBotColor0->setValue(config.sky.botColor[0]);
    ui->skyBotColor1->setValue(config.sky.botColor[1]);
    ui->skyBotColor2->setValue(config.sky.botColor[2]);
    ui->skyTopColor0->setValue(config.sky.topColor[0]);
    ui->skyTopColor1->setValue(config.sky.topColor[1]);
    ui->skyTopColor2->setValue(config.sky.topColor[2]);
    
    ui->dof->setChecked(config.dof.enabled);
    ui->dofFocusScale->setValue(config.dof.focusScale);
    ui->dofFar->setValue(config.dof.uFar);
    ui->dofMaxBlurSize->setValue(config.dof.maxBlurSize);
    ui->dofRadScale->setValue(config.dof.radScale);

    ui->bloom->setChecked(config.bloom.enabled);
    ui->bloomThreshold0->setValue(config.bloom.threshold[0]);
    ui->bloomThreshold1->setValue(config.bloom.threshold[1]);
    ui->bloomThreshold2->setValue(config.bloom.threshold[2]);

    ui->ssao->setChecked(config.ssao.enabled);
    ui->ssaoBias->setValue(config.ssao.bias);
    ui->ssaoRadius->setValue(config.ssao.radius);

    ui->fog->setChecked(config.fog.enabled);
    ui->fogSight->setValue(config.fog.sight);

    ui->shadows->setChecked(config.shadows.enabled);
    ui->shadowRes->setValue(config.shadows.resolution);

    ui->contactShadows->setChecked(config.contactShadows.enabled);

    if (config.smaa.enabled)
        ui->aliasingCB->setCurrentIndex(0);
    else
        ui->aliasingCB->setCurrentIndex(1);

    ui->dlAmbient0->setValue(config.dirlight.ambient[0]);
    ui->dlAmbient1->setValue(config.dirlight.ambient[1]);
    ui->dlAmbient2->setValue(config.dirlight.ambient[2]);
    ui->dlDiffuse0->setValue(config.dirlight.diffuse[0]);
    ui->dlDiffuse1->setValue(config.dirlight.diffuse[1]);
    ui->dlDiffuse2->setValue(config.dirlight.diffuse[2]);
    ui->dlSpecular0->setValue(config.dirlight.specular[0]);
    ui->dlSpecular1->setValue(config.dirlight.specular[1]);
    ui->dlSpecular2->setValue(config.dirlight.specular[2]);
    ui->dlDirection0->setValue(config.dirlight.direction[0]);
    ui->dlDirection1->setValue(config.dirlight.direction[1]);
    ui->dlDirection2->setValue(config.dirlight.direction[2]);
    ui->dlItensity->setValue(config.dirlight.itensity);

    auto* terrain = world->GetTerrain();
    auto* terrainConfig = world->GetTerrainConfig();

    ui->terrainMaterialIndex->setCurrentIndex(0);
    ui->terrainLodCB->setCurrentIndex(7);

    if (terrain)
    { 
        ui->terrainVerticalScaling->setValue(terrain->GetMaterial(0).displaceScale);
        ui->terrainHorizontalScaling->setValue(terrain->GetMaterial(0).horizontalScale);
    }

    if (terrainConfig)
    { 
        ui->terrainTessFactor->setValue(terrainConfig->tessellationFactor);
        ui->terrainTessRange->setValue(terrainConfig->tessellationRange);
        ui->terrainTessShift->setValue(terrainConfig->tessellationShift);
        ui->terrainTessSlope->setValue(terrainConfig->tessellationSlope);
        ui->terrainLodInput->setValue(terrainConfig->lod_range[0]);
    }

    ui->IMLod0->setValue(config.instancedMesh.LODRanges[0]);
    ui->IMLod1->setValue(config.instancedMesh.LODRanges[1]);
    ui->IMLod2->setValue(config.instancedMesh.LODRanges[2]);
    ui->IMLod3->setValue(config.instancedMesh.LODRanges[3]);

    ui->vegLod0->setValue(config.vegetation.LODRanges[0]);
    ui->vegLod1->setValue(config.vegetation.LODRanges[1]);
    ui->vegLod2->setValue(config.vegetation.LODRanges[2]);
    ui->vegLod3->setValue(config.vegetation.LODRanges[3]);
    ui->vegLod4->setValue(config.vegetation.LODRanges[4]);
}

void Editor::InitRenderConfigGUI()
{
    //Light model -------------------------------------
    connect(ui->lightingModelCB, &QComboBox::currentIndexChanged, [&](int index)
        {
            if (!world) return;
            world->GetRenderConfig().lightModel = (DeferredRendererConfig::LightModel)index;
            ui->lightingModelCB->setCurrentIndex((DeferredRendererConfig::LightModel)index);
        });

    //GI-----------------------------------------------------------------------
    connect(ui->GI, &QGroupBox::clicked, [&](bool val)
        {
            if (!world) return;
            world->GetRenderConfig().gi.enabled = val;
        });


    //voxel cone tracing-----------------------------------------------------------------------
    connect(ui->vctDisplayVoxels, &QCheckBox::clicked, [&](bool val)
        {
            if (!world) return;
            world->GetRenderConfig().vct.bDisplay = val;
        });

    //Volumetric light-----------------------------------------------------------------------
    connect(ui->volumetricLight, &QGroupBox::clicked, [&]()
        {
            if (!world) return;      
            world->GetRenderConfig().volumetricLight.enabled = ui->volumetricLight->isChecked();
        });
    connect(ui->vlRaymarching, &QSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().volumetricLight.numRays = ui->vlRaymarching->value();
        });
    connect(ui->vlScattering, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().volumetricLight.scattering = ui->vlScattering->value();
        });
    connect(ui->vlLightColor0, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().volumetricLight.sunColor[0] = ui->vlLightColor0->value();
        });
    connect(ui->vlLightColor1, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().volumetricLight.sunColor[1] = ui->vlLightColor1->value();
        });
    connect(ui->vlLightColor2, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().volumetricLight.sunColor[2] = ui->vlLightColor2->value();
        });
    // Volumetric Clouds-------------------
    connect(ui->vcCloudRaySteps, &QSpinBox::valueChanged, [&](int val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.numCloudRaySteps = val;
        });
    connect(ui->vcShadowRays, &QSpinBox::valueChanged, [&](int val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.numShadowRaySteps = val;
        });
    connect(ui->vcVolumetricLightRays, &QSpinBox::valueChanged, [&](int val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.numVolumetricLightRaySteps = val;
        });
    connect(ui->vcVLScattering, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.volumetricLightScattering = val;
        });
    connect(ui->vcVLCol0, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.vlLightColor.x = val;
        });
    connect(ui->vcVLCol1, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.vlLightColor.y = val;
        });
    connect(ui->vcVLCol2, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.vlLightColor.z = val;
        });
    connect(ui->vcCoverage, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.cloudCoverage = val;
        });
    connect(ui->vcSpeed, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.cloudSpeed = val;
        });
    connect(ui->vcLightAbsorption, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.lightAbsorption = val;
        });
    connect(ui->vcDensity, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.cloudDensity = val;
        });
    connect(ui->vcCrispiness, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.cloudFluffyness = val;
        });
    connect(ui->vcCloudColor0, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.cloudBotColor.x = val;
        });
    connect(ui->vcCloudColor1, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.cloudBotColor.y = val;
        });
    connect(ui->vcCloudColor2, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.cloudBotColor.z = val;
        });
    connect(ui->vcWD0, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.windDirection.x = val;
        });
    connect(ui->vcWD1, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.windDirection.y = val;
        });
    connect(ui->vcWD2, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.windDirection.z = val;
        });
    connect(ui->vcEarthRadius, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.earthRadius = val;
        });
    connect(ui->vcAIR, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.atmosphereInnerRadius = val;
        });
    connect(ui->vcAOR, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.atmosphereOuterRadius = val;
        });
    connect(ui->vcDownScale, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            world->GetRenderConfig().volumetricClouds.downScale = val;
        });
    //--------------------------------------
    //Sky---------------------------------------------------------------------------------
    connect(ui->skyBotColor0, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().sky.botColor[0] = ui->skyBotColor0->value();
        });
    connect(ui->skyBotColor1, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().sky.botColor[1] = ui->skyBotColor1->value();
        });
    connect(ui->skyBotColor2, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().sky.botColor[2] = ui->skyBotColor2->value();
        });
    connect(ui->skyTopColor0, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().sky.topColor[0] = ui->skyTopColor0->value();
        });
    connect(ui->skyTopColor1, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().sky.topColor[1] = ui->skyTopColor1->value();
        });
    connect(ui->skyTopColor2, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().sky.topColor[2] = ui->skyTopColor2->value();
        });

    //Depth of field ---------------------------------------------------------------
    connect(ui->dof, &QGroupBox::clicked, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().dof.enabled = ui->dof->isChecked();
        });
    connect(ui->dofFar, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().dof.uFar = ui->dofFar->value();
        });
    connect(ui->dofFocusScale, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().dof.focusScale = ui->dofFocusScale->value();
        });
    connect(ui->dofMaxBlurSize, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().dof.maxBlurSize = ui->dofMaxBlurSize->value();
        });
    connect(ui->dofRadScale, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().dof.radScale = ui->dofRadScale->value();
        });

    //Bloom---------------------------------------------------------------------------
    connect(ui->bloom, &QGroupBox::clicked, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().bloom.enabled = ui->bloom->isChecked();
        });
    connect(ui->bloomThreshold0, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().bloom.threshold[0] = ui->bloomThreshold0->value();
        });
    connect(ui->bloomThreshold1, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().bloom.threshold[1] = ui->bloomThreshold1->value();
        });
    connect(ui->bloomThreshold2, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().bloom.threshold[2] = ui->bloomThreshold2->value();
        });

    //SSAO-----------------------------------------------------------------------------
    connect(ui->ssao, &QGroupBox::clicked, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().ssao.enabled = ui->ssao->isChecked();
        });
    connect(ui->ssaoBias, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().ssao.bias = ui->ssaoBias->value();
        });
    connect(ui->ssaoRadius, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().ssao.radius = ui->ssaoRadius->value();
        });

    //Fog------------------------------------------------------------------------------
    connect(ui->fog, &QGroupBox::clicked, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().fog.enabled = ui->fog->isChecked();
        });
    connect(ui->fogSight, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().fog.sight = ui->fogSight->value();
        });

    //Shadows
    connect(ui->shadows, &QGroupBox::clicked, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().shadows.enabled = ui->shadows->isChecked();
        });
    connect(ui->shadowRes, &QSpinBox::valueChanged, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().shadows.resolution = ui->shadowRes->value();
        });

    //Contact shadows
    connect(ui->contactShadows, &QGroupBox::clicked, [&]()
        {
            if (!world) return;
            world->GetRenderConfig().contactShadows.enabled = ui->contactShadows->isChecked();
        });

    //Aliasing
    connect(ui->aliasingCB, &QComboBox::currentIndexChanged, [&]()
        {
            if (!world) return;

            if (ui->aliasingCB->currentIndex() == 0)
            { 
                world->GetRenderConfig().smaa.enabled = true;
                world->GetRenderConfig().ssaa.enabled = false;
            }
            else
            {
                world->GetRenderConfig().smaa.enabled = false;
                world->GetRenderConfig().ssaa.enabled = true;
            }
        });

    //Dirlight
    connect(ui->dlDirection0, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetLightDirection(); oldVal.x = val;        
            world->skySystem.SetLightDirection(oldVal);
            world->GetRenderConfig().dirlight.direction[0] = ui->dlDirection0->value();
        });
    connect(ui->dlDirection1, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetLightDirection(); oldVal.y = val;      
            world->skySystem.SetLightDirection(oldVal);
            world->GetRenderConfig().dirlight.direction[1] = ui->dlDirection1->value();
        });
    connect(ui->dlDirection2, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetLightDirection(); oldVal.z = val;         
            world->skySystem.SetLightDirection(oldVal);
            world->GetRenderConfig().dirlight.direction[2] = ui->dlDirection2->value();
        });
    connect(ui->dlAmbient0, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetAmbientLight(); oldVal.x = val;         
            world->skySystem.SetLightAmbient(oldVal);
            world->GetRenderConfig().dirlight.ambient[0] = ui->dlAmbient0->value();
        });
    connect(ui->dlAmbient1, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetAmbientLight(); oldVal.y = val;          
            world->skySystem.SetLightAmbient(oldVal);
            world->GetRenderConfig().dirlight.ambient[1] = ui->dlAmbient1->value();
        });
    connect(ui->dlAmbient2, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetAmbientLight(); oldVal.z = val;          
            world->skySystem.SetLightAmbient(oldVal);
            world->GetRenderConfig().dirlight.ambient[2] = ui->dlAmbient2->value();
        });
    connect(ui->dlDiffuse0, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetDiffuseLight(); oldVal.x = val;         
            world->skySystem.SetLightDiffuse(oldVal);
            world->GetRenderConfig().dirlight.diffuse[0] = ui->dlDiffuse0->value();
        });
    connect(ui->dlDiffuse1, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetDiffuseLight(); oldVal.y = val;           
            world->skySystem.SetLightDiffuse(oldVal);
            world->GetRenderConfig().dirlight.diffuse[1] = ui->dlDiffuse1->value();
        });
    connect(ui->dlDiffuse2, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetDiffuseLight(); oldVal.z = val;          
            world->skySystem.SetLightDiffuse(oldVal);
            world->GetRenderConfig().dirlight.diffuse[2] = ui->dlDiffuse2->value();
        });
    connect(ui->dlSpecular0, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetSpecularLight(); oldVal.x = val;        
            world->skySystem.SetLightSpecular(oldVal);
            world->GetRenderConfig().dirlight.specular[0] = ui->dlSpecular0->value();
        });
    connect(ui->dlSpecular1, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetSpecularLight(); oldVal.y = val;           
            world->skySystem.SetLightSpecular(oldVal);
            world->GetRenderConfig().dirlight.specular[1] = ui->dlSpecular1->value();
        });
    connect(ui->dlSpecular2, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            auto oldVal = world->skySystem.GetSpecularLight(); oldVal.z = val;          
            world->skySystem.SetLightSpecular(oldVal);
            world->GetRenderConfig().dirlight.specular[2] = ui->dlSpecular2->value();
        });
    connect(ui->dlItensity, &QDoubleSpinBox::valueChanged, [&]()
        {
            if (!world) return;        
            world->skySystem.SetLightItensity(ui->dlItensity->value());
            world->GetRenderConfig().dirlight.itensity = ui->dlItensity->value();
        });

    //Terrain
    connect(ui->terrainHorizontalScaling, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;  
            if (ui->terrainMaterialIndex->currentIndex() > 2) return;
            if (!world->GetTerrain()) return;
            world->GetTerrain()->GetMaterial(ui->terrainMaterialIndex->currentIndex()).horizontalScale = val;
        });
    connect(ui->terrainVerticalScaling, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            if (ui->terrainMaterialIndex->currentIndex() > 2) return;
            if (!world->GetTerrain()) return;
            world->GetTerrain()->GetMaterial(ui->terrainMaterialIndex->currentIndex()).displaceScale = val;
        });
    connect(ui->terrainLodInput, &QSpinBox::valueChanged, [&](int val)
        {
            if (!world) return;
            if (ui->terrainMaterialIndex->currentIndex() > 2) return;
            if (!world->GetTerrainConfig()) return;
            world->GetTerrainConfig()->lod_range[8 - ui->terrainMaterialIndex->currentIndex()] = val;
        });
    connect(ui->terrainTessFactor, &QSpinBox::valueChanged, [&](int val)
        {
            if (!world) return;
            if (!world->GetTerrainConfig()) return;
            world->GetTerrainConfig()->tessellationFactor = val;
        });
    connect(ui->terrainTessRange, &QSpinBox::valueChanged, [&](int val)
        {
            if (!world) return;
            if (!world->GetTerrainConfig()) return;
            world->GetTerrainConfig()->tessellationRange = val;
        });
    connect(ui->terrainTessShift, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            if (!world->GetTerrainConfig()) return;
            world->GetTerrainConfig()->tessellationShift = val;
        });
    connect(ui->terrainTessSlope, &QDoubleSpinBox::valueChanged, [&](double val)
        {
            if (!world) return;
            if (!world->GetTerrainConfig()) return;
            world->GetTerrainConfig()->tessellationSlope = val;
        });
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
