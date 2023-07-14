#include "Character.h"
#include "renderwindow.h"
#include "Components.h"
#include "Debug.h"
#include "RenderDebugger.h"

Character::Character()
{}

Character::~Character()
{}

void Character::OnCreate()
{
	auto* world = GetWorld();
	auto* smm = world->GetSkeletalMeshManager();
	auto* sm = world->GetStaticMeshManager();

	auto& inputComponent =		AddComponent<InputComponent>();
	auto& camera =				AddComponent<CameraComponent>().camera;
	auto& skeletalMesh =		AddComponent<SkeletalMeshComponent>().skeletalMesh;
	auto& springArmComponent =	AddComponent<SpringArmComponent>();
	auto& audios =				AddComponent<AudioComponent>().audios;
	auto& collider =			AddComponent<CollisionComponent>(CollideableType::Capsule).col;
	auto& body =				AddComponent<PhysicsComponent>().body;

	collider.SetExtents(glm::vec3(8.f));
	collider.onHit = [this](entt::entity other) {this->OnHit(other); };

	body.SetMass(100.f);
	body.SetElasticity(0.f);
	body.SetFriction(1.f);
	camera.SetFar(3000.f);
	body.bLinearOnly = true;

	inputComponent.BindMousePressed([this](QMouseEvent* e) {this->OnMousePressed(e); });
	inputComponent.BindMouseReleased([this](QMouseEvent* e) {this->OnMouseReleased(e); });
	inputComponent.BindMouseHeld([this](std::map<int, bool>* m, float d) {this->OnMouseHeld(m, d); });
	inputComponent.BindMouseMove([this](QMouseEvent* e) {this->OnMouseMove(e); });
	inputComponent.BindKeyHeld([this](std::map<int, bool>* m, float d) {this->OnKeyHeld(m, d); });

	if (smm->LoadSkeletalMesh("../3Dprog22/Assets/Models/GreenLady/Ch34_nonPBR.dae", skeletalMesh))
	{
		skeletalMesh.LoadAnimation("Idle", "../3Dprog22/Assets/Models/GreenLady/animations/Idle.dae");
		skeletalMesh.LoadAnimation("Running", "../3Dprog22/Assets/Models/GreenLady/animations/Running.dae");
		skeletalMesh.LoadAnimation("RunningBack", "../3Dprog22/Assets/Models/GreenLady/animations/Running Backward.dae");
		skeletalMesh.LoadAnimation("Stunned", "../3Dprog22/Assets/Models/GreenLady/animations/Stunned.dae");
		skeletalMesh.PlayAnimation("Idle", 0.5f);
	}

	audios[Audio::FootSteps].LoadAudio("../3Dprog22/Assets/Audio/footstepsgrass1-mono.mp3");
	audios[Audio::FootSteps].SetPitch(0.6f);
	audios[Audio::FootSteps].SetGain(20.f);
	audios[Audio::FootSteps].SetLocalPosition({ 0.f, -5.f, 0.f });
	audios[Audio::FootSteps].SetLooping(true);
	
	skeletalMesh.transform.SetScale({ 0.12f, 0.12f, 0.12f });
	skeletalMesh.transform.SetPosition({ 0.f, -10.f, 0.f });
	springArmComponent.SetRootOffset({ 0.f, 15.f, 0.f });
	springArmComponent.SetLength(80.f);
	springArmComponent.SetPitch(40.f);

	SyncCameraWithSpringArm();
}

void Character::OnBeginPlay()
{
	auto& camera = GetComponent<CameraComponent>().camera;
	auto* world = GetWorld();
	world->SetRenderCamera(&camera);
}

void Character::OnUpdate(float deltaTime)
{
	UpdateSpringArm(deltaTime);
	UpdateAnimationState(deltaTime);
	UpdateAudioState(deltaTime);
	UpdateStunTimer(deltaTime);
}

void Character::UpdateStunTimer(float deltatime)
{
	CurrentStunTimer -= deltatime;
}

void Character::UpdateAnimationState(float deltatime)
{
	if (!keyHeld) return;

	auto& sm = GetComponent<SkeletalMeshComponent>().skeletalMesh;
	auto& body = GetComponent<PhysicsComponent>().body;

	if (CurrentStunTimer > 0.f) 
	{ 
		sm.PlayAnimation("Stunned", 1.f);
		auto newVel = body.GetVelocity(); newVel.x = 0; newVel.z = 0.f;
		body.SetVelocity(newVel);
		animState = CharacterAnimationState::Stunned;
		return;
	}
	else
	{
		sm.ResetAnimation("Stunned");
		
	}

	if ((*keyHeld)[Qt::Key_W]) 
	{
		sm.PlayAnimation("Running", 1.f);
		animState = CharacterAnimationState::Forward;
		
	}
	else if ((*keyHeld)[Qt::Key_S])
	{
		sm.PlayAnimation("RunningBack", 1.0f);
		animState = CharacterAnimationState::Back;
	}
	else
	{
		sm.PlayAnimation("Idle", 1.0f);
		animState = CharacterAnimationState::Idle;
	}
}

void Character::UpdateAudioState(float deltatime)
{
	auto& audios = GetComponent<AudioComponent>().audios;
	
	if (animState == CharacterAnimationState::Forward || animState == CharacterAnimationState::Back)
	{
		if (audios[Audio::FootSteps].GetState() != AudioState::Play)
			audios[Audio::FootSteps].Play();
		
	}
	else
	{
		audios[Audio::FootSteps].Stop();
	}
}

void Character::UpdateSpringArm(float deltaTime)
{
	SyncCameraWithSpringArm();
}

void Character::OnHit(entt::entity otherEnt)
{
	Entity oe(otherEnt, GetWorld());

	auto& oEntTag = oe.GetComponent<TagComponent>().tag;

	if (oEntTag.find("Bomb") != std::string::npos)
	{
		std::cout << "character hit" << std::endl;
		CurrentStunTimer = stunTimer;
		GetWorld()->DeleteEntity((entt::entity)otherEnt);
	}

	std::cout << "Player hit other ent " << oEntTag << std::endl;

	//Oppgave 8
	if (oEntTag.find("PlayerTrophy") != std::string::npos)
	{
		std::cout << "character hit" << std::endl;
		GetWorld()->DeleteEntity((entt::entity)otherEnt);
	}
}

void Character::GetCamera2DAxises(glm::vec3& forward, glm::vec3& right)
{
	auto& camera = GetComponent<CameraComponent>().camera;
	auto camInvView = glm::inverse(camera.GetViewMatrix());

	camInvView[2][1] = 0.f;
	camInvView[1] = glm::vec4(World::GetWorldUp(), 0.f);

	forward = camInvView[2] * -1.f;
	forward.y = 0.f;
	forward = glm::normalize(forward);
	right = camInvView[0];
}

void Character::SyncCameraWithSpringArm()
{
	auto& camera = GetComponent<CameraComponent>().camera;
	auto& transform = GetComponent<TransformComponent>();
	auto& spc = GetComponent<SpringArmComponent>();

	camera.SetCameraPosition(transform.GetPosition() + spc.GetAttachPosition());
	const auto camDir = glm::normalize(spc.GetRootPosition() - spc.GetAttachPosition());
	camera.SetCameraDirection(camDir);
}

bool Character::RotateTo2DVelocity()
{
	auto& rigidBody = GetComponent<PhysicsComponent>().body;
	
	auto velocityDir = rigidBody.GetVelocity(); velocityDir.y = 0.f;
	velocityDir = glm::normalize(velocityDir);

	if (glm::any(glm::isnan(velocityDir))) return false;

	const auto worldUp = GetWorld()->GetWorldUp();

	const auto right = glm::cross(worldUp, velocityDir);

	const glm::mat4 lookAt =
	{
		{right, 0.f},
		{worldUp, 0.f},
		{velocityDir, 0.f},
		{0.f, 0.f,0.f, 1.f}
	};

	lastRotation = lookAt;
	SetRotation(lookAt);

	return true;
	
}

void Character::OnMouseMove(QMouseEvent* event)
{
	if ((*mouseHeld)[Qt::RightButton])
	{
		HandleSpringArmRotation(event->pos().x(), event->pos().y());
	}
}

void Character::HandleSpringArmRotation(const float mx, const float my)
{
	auto& spc = GetComponent<SpringArmComponent>();

	auto* renderWindow = RenderWindow::Get();
	const QPoint midPoint = lastFrameMP;

	const float dx = static_cast<float>(lastFrameMP.x() - mx) * 0.1f;
	const float dy = static_cast<float>(lastFrameMP.y() - my) * 0.1f;

	spc.AddYaw(-dx);
	spc.AddPitch(-dy);

	const auto globalPos = renderWindow->mapToGlobal(midPoint);
	QCursor::setPos(globalPos);
}

void Character::OnMousePressed(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		QCursor cursor(Qt::BlankCursor);
		QApplication::setOverrideCursor(cursor);
		QApplication::changeOverrideCursor(cursor);
		
		lastFrameMP = event->pos();
	}
}

void Character::OnMouseReleased(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		QCursor cursor(Qt::ArrowCursor);
		QApplication::setOverrideCursor(cursor);
		QApplication::changeOverrideCursor(cursor);
	}
}

void Character::OnMouseHeld(std::map<int, bool>* mHeld, float deltaTime)
{
	mouseHeld = mHeld;
}

void Character::OnKeyHeld(std::map<int, bool>* keyHeld, float deltaTime)
{
	this->keyHeld = keyHeld;
	//Oppgave 4
	auto& rigidBody = GetComponent<PhysicsComponent>().body;
	auto& sm = GetComponent<SkeletalMeshComponent>().skeletalMesh;

	if (CurrentStunTimer > 0.f)
	{
		glm::quat newRot = glm::identity<glm::quat>();
		newRot = glm::rotate(newRot, -currentRot, { 0.f, 1.f, 0.f });
		SetRotation(newRot);

		rigidBody.SetRotation(glm::vec3(0.f, -currentRot, 0.f));
		return;
	}

	glm::vec3 fwd;
	glm::vec3 runDirection{ 0.f, rigidBody.GetVelocity().y, 0.f };

	fwd = GetForwardVector();

	int iSign = 1.f;
	float speedReductionRatio = 1.f;

	if ((*keyHeld)[Qt::Key_W])
	{
		runDirection += fwd;
	}
	if ((*keyHeld)[Qt::Key_S])
	{
		runDirection -= fwd;
		iSign = -1.f;
		speedReductionRatio = backwardSpeedRatio;
	}	
	if ((*keyHeld)[Qt::Key_D])
	{
		currentRot -= glm::radians(rotSpeed * deltaTime) * iSign;
	}
	if ((*keyHeld)[Qt::Key_A])
	{
		currentRot += glm::radians(rotSpeed * deltaTime) * iSign;
	}
	
	//Rigid body-en bruker euler angles akkurat nå, har ikke hadd nok til å skrive refaktoringa av fysikk motoren
	//Derfor kalkuleres alt dette i euler angles
	//Har heller ikke animation som springer baklengs for nå
	//Mulig jeg legger det til om jeg får tid

	glm::quat newRot = glm::identity<glm::quat>();
	newRot = glm::rotate(newRot, -currentRot, { 0.f, 1.f, 0.f });
	glm::quat otherRot = glm::identity<glm::quat>();
	otherRot = glm::rotate(otherRot, currentRot * 2.f, { 0.f, 1.f, 0.f });
	sm.transform.SetRotation(otherRot);
	SetRotation(newRot);

	if (glm::length2(runDirection) < 1.f) return;

	glm::vec3 newVelocity = glm::normalize(runDirection) * speed * speedReductionRatio;
	newVelocity.y = 0.f;
	rigidBody.SetVelocity(newVelocity);
}

