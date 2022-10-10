#include "AiCharacter.h"
#include "Debug.h"
#include "PointGenerator.h"
#include "RenderDebugger.h"
#include "PathFinding.h"

static float aStarTimer{ 0.f };


void AiCharacter::OnCreate()
{
    auto* world = GetWorld();
    auto* sm = world->GetSkeletalMeshManager();

    auto& skeletalMesh = AddComponent<SkeletalMeshComponent>().skeletalMesh;
    auto& body = AddComponent<PhysicsComponent>().body;
    auto& collider = AddComponent<CollisionComponent>(CollideableType::Capsule).col;

    collider.SetExtents(glm::vec3(8.f));
    collider.onHit = [this](entt::entity other) {this->OnHit(other); };

    body.bLinearOnly = true;

    if (sm->LoadSkeletalMesh("../3Dprog22/Assets/Models/nightshade/nightshade.dae", skeletalMesh))
    {
	    skeletalMesh.LoadAnimation("Idle", "../3Dprog22/Assets/Models/nightshade/animations/Idle.dae");
	    skeletalMesh.LoadAnimation("Running", "../3Dprog22/Assets/Models/nightshade/animations/Running.dae");
	    skeletalMesh.PlayAnimation("Idle", 0.5f);
    }

    skeletalMesh.transform.SetPosition({ 0.f, -10.f, 0.f });
    skeletalMesh.transform.SetScale({ 0.1f, 0.1f, 0.1f });
	
}

void AiCharacter::OnBeginPlay()
{   
    cachedWorldUp = World::GetWorldUp();
}

void AiCharacter::OnUpdate(float deltaTime)
{   
    aStarTimer += deltaTime;

    UpdateAiState(deltaTime);
    UpdateStunTimer(deltaTime);
    UpdateMovement(deltaTime);
    UpdateAnimationState(deltaTime);
    DrawPatrolFunction();
    DrawWayPoints();
}

void AiCharacter::UpdateStunTimer(float deltatime)
{
    currentStunTimer -= deltatime;
}

void AiCharacter::OnHit(entt::entity otherEnt)
{
    Entity oe((entt::entity)otherEnt, GetWorld());
    auto& oEntTag = oe.GetComponent<TagComponent>().tag;

    //Oppgave 9
    if (oEntTag.find("EnemyTrophy") != std::string::npos)
    {
        GetWorld()->DeleteEntity((entt::entity)otherEnt);
    }
    if (oEntTag.find("Bomb") != std::string::npos)
    {
        currentStunTimer = stunTimer;
        GetWorld()->DeleteEntity((entt::entity)otherEnt);
    }
}

void AiCharacter::UpdateAiState(float deltaTime)
{
    switch (aiState)
    {
    case AiState::Patrolling:
    {
        UpdatePatrolState(deltaTime);
    }
    break;
    case AiState::Follow:
    {
        UpdateFollow(deltaTime);
    }
    break;
    case AiState::None:
    {
    }
    break;
    default:
    {
        std::cout << "Not all ai cases are accounted for" << std::endl;
    }
    break;
    }
}

void AiCharacter::UpdatePatrolState(float deltaTime)
{
    switch (aiPatrolState)
    {
    case AiPatrolState::Function:
    {
        UpdatePatrolFunction(deltaTime);
    }
    break;
    case AiPatrolState::Bezier: //Brukes til bezier kurven
    {
        UpdateBezierFunc(deltaTime);
    }
    break;
    case AiPatrolState::WayPoints:
    {
        UpdateWayPoint(deltaTime);
    }
    break;
    case AiPatrolState::None:
    {

    }
    break;
    default:
    {
        std::cout << "Patrol state not defined " << std::endl;
    }
    break;
    }
}

void AiCharacter::UpdateAnimationState(float deltaTime)
{
    switch (aiMoveState)
    {
    case AiMoveState::None:
    {
        auto& sm = GetComponent<SkeletalMeshComponent>().skeletalMesh;
        sm.PlayAnimation("Idle", 1.f);
    }
    break;
    case AiMoveState::DestinationReached:
    {
    }
    break;
    case AiMoveState::Moving:
    {
        auto& sm = GetComponent<SkeletalMeshComponent>().skeletalMesh;
        sm.PlayAnimation("Running", 1.0f);
    }
    break;
    }
}

void AiCharacter::UpdateBezierFunc(float deltatime)
{
    if (!IsMoveState(AiMoveState::DestinationReached) && !IsMoveState(AiMoveState::None)) return;

    if (!PatrolFunction) return;

    if (currentX >= endX - step - FLT_EPSILON && sign == 1) { sign = -1; }
    if (currentX <= startX + step + FLT_EPSILON && sign == -1) { sign = 1; }

    currentX += step * (float)sign;
    glm::vec2 bezierPos = bezierFunc(currentX);

    const glm::vec3 transformedPatPos2D = patrolFunctionTransform * glm::vec4(bezierPos.x, 0.f, bezierPos.y, 1.f);
    MoveToIn2D(transformedPatPos2D);
}

void AiCharacter::UpdateWayPoint(float deltatime)
{
    if (!IsMoveState(AiMoveState::DestinationReached)) return;

    static int index{ 0 };

    if (index == waypoints.size())
    {
        auto& sm = GetComponent<SkeletalMeshComponent>().skeletalMesh;
        sm.PlayAnimation("Idle", 1.f);

        SetMoveState(AiMoveState::None);
        SetPatrolState(AiPatrolState::None);
        SetAiState(AiState::None);
        index = 0;
        return;
    }
    MoveToIn2D(waypoints[index++]);
}

int AiCharacter::CalculateAStarWayPts(const glm::vec3& current, const glm::vec3& target)
{
    auto* world = World::Get();

    auto currentNode = world->pathFindingSystem.GetNode(current);

    if (!currentNode) return -1;
       
    auto targetNode = world->pathFindingSystem.GetNode(target);

    glm::vec3 a;

    memcpy(&a, targetNode->GetPosition(), 12);

    VEC3(a)

    if (!targetNode) return -2;
 
    waypoints.clear();

    auto start = std::chrono::steady_clock::now();

    bool aResult = PathFinding::AStar(currentNode, targetNode, waypoints);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Astar elapsed time: " << elapsed_seconds.count() * 1000.f << "ms\n";

    glm::vec3 target2D = target; target2D.y = 0.f;
    waypoints.emplace_back(target2D);

    return aResult;
}

void AiCharacter::UpdatePatrolFunction(float deltaTime)
{
    if (!IsMoveState(AiMoveState::DestinationReached) && !IsMoveState(AiMoveState::None)) return;

    if (!PatrolFunction) return;
	
    if (currentX >= endX - step - FLT_EPSILON && sign == 1) { sign = -1; }       
    if (currentX <= startX + step + FLT_EPSILON && sign == -1) {sign = 1;}
    
    currentX += step * (float)sign;
    const float currentZ = PatrolFunction(currentX);

    const glm::vec3 transformedPatPos2D = patrolFunctionTransform * glm::vec4(currentX, 0.f, currentZ, 1.f);
    MoveToIn2D(transformedPatPos2D);
}

void AiCharacter::UpdateFollow(float deltaTime)
{
    Entity otherEnt(entityToFollow, GetWorld());

    if (!otherEnt.IsValid())
    {
        SetAiState(AiState::None);
        SetMoveState(AiMoveState::None);
        return;
    }

    const auto aiPos = GetPosition();
    const auto otherEntPos = otherEnt.GetPosition();
    const glm::vec3 otherEntPos2D = { otherEntPos.x, 0.f, otherEntPos.z };

    const auto dist2 = glm::distance2(otherEntPos, aiPos);
	
    if (dist2 < stopFollowDist2)
    {
        SetMoveState(AiMoveState::None);
        return;
    }
    else if (dist2 > stopFollowDist2 && dist2 < stopFollowDist2 + stopFollowThreshold)
	{
        return;
	}
    MoveToIn2D(otherEntPos2D);
}

void AiCharacter::UpdateMovement(float deltaTime)
{
    if (currentStunTimer > 0.f) return;

    if (!IsMoveState(AiMoveState::Moving)) return;

    if (HasReachedDestination())
    {
        SetMoveState(AiMoveState::DestinationReached);	
        return;
    }

    auto& rigidBody = GetComponent<PhysicsComponent>().body;

    auto currentPos2D = GetPosition(); currentPos2D.y = 0.f;
    auto targetPos2D = targetPosition; targetPos2D.y = 0.f;

    auto dir2D = glm::normalize(targetPos2D - currentPos2D);

    RotateTo2DDirection(dir2D);
    
    const float lastVelY = rigidBody.GetVelocity().y;

    glm::vec3 newVelocity = dir2D * movementSpeed;
    newVelocity.y = lastVelY;

    rigidBody.SetVelocity(newVelocity);
}

void AiCharacter::RotateTo2DDirection(const glm::vec3& dir2D)
{
    auto& sm = GetComponent<SkeletalMeshComponent>().skeletalMesh;

    const auto right = glm::cross(cachedWorldUp, dir2D);

    const glm::mat4 lookAt =
    {
        {right, 0.f},
        {cachedWorldUp, 0.f},
        {dir2D, 0.f},
        {0.f, 0.f, 0.f, 1.f}
    };

    const glm::mat4 lookAt2 =
    {
        {right, 0.f},
        {cachedWorldUp, 0.f},
        {-dir2D, 0.f},
        {0.f, 0.f, 0.f, 1.f}
    };

    sm.transform.SetRotation(lookAt2);
    SetRotation(lookAt);
}

void AiCharacter::MoveToIn2D(const glm::vec3& pos2D)
{
    SetMoveState(AiMoveState::Moving);
    targetPosition = pos2D;
}

bool AiCharacter::HasReachedDestination()
{
    auto currentPos = GetPosition();
    currentPos.y = 0.f;
    return glm::distance2(currentPos, targetPosition) <= 5.f * 5.f;
}

void AiCharacter::SetPatrolFunction(float(*func)(float x), float startX, float endX, float step)
{
    SetPatrolState(AiPatrolState::Function);
    PatrolFunction = func;
    this->startX = startX;
    this->endX = endX;
    this->step = step;
    this->sign = 1;
	
    const auto inversePT = glm::inverse(patrolFunctionTransform);

    const glm::vec3 localFunc = inversePT * glm::vec4(GetPosition(), 1.f);

    this->currentX = glm::clamp(localFunc.x, startX, endX);
}

void AiCharacter::ResetPatrolFunction()
{
    SetPatrolState(AiPatrolState::None);

    PatrolFunction = nullptr;
    startX = 0.f;
    endX = 0.f;
    currentX = 0.f;
    step = 0.25f;
}

void AiCharacter::SetPatrolBezierTransform(const glm::mat4& newTransform)
{
    SetPatrolFunctionTransform(newTransform);
}

void AiCharacter::SetPatrolBezier(std::function<glm::vec2(float)> func, float startX, float endX, float step)
{
    SetPatrolState(AiPatrolState::Bezier);
    bezierFunc = func;
    this->startX = startX;
    this->endX = endX;
    this->step = step;
    this->sign = 1;

    const auto inversePT = glm::inverse(patrolFunctionTransform);

    const glm::vec3 localFunc = inversePT * glm::vec4(GetPosition(), 1.f);

    this->currentX = glm::clamp(localFunc.x, startX, endX);
}

void AiCharacter::ResetPatrolBezier()
{
    SetPatrolState(AiPatrolState::Bezier);

    bezierFunc = nullptr;
    startX = 0.f;
    endX = 0.f;
    currentX = 0.f;
    step = 0.25f;
}

void AiCharacter::SetPatrolFunctionTransform(const glm::mat4& newTransform)
{
    patrolFunctionTransform = newTransform;

    const auto inversePT = glm::inverse(patrolFunctionTransform);

    const glm::vec3 localFunc = inversePT * glm::vec4(GetPosition(), 1.f);

    this->currentX = glm::clamp(localFunc.x, startX, endX);
}

void AiCharacter::DrawPatrolFunction() const
{
    if (!PatrolFunction) return;

    auto* rd = RenderDebugger::Get();

    auto pts = PointGenerator::Generate(
        PatrolFunction, startX, endX, step
    );

    for (auto i = 0; i < pts.size() - 1; i++)
    {
        auto cp = patrolFunctionTransform * glm::vec4(pts[i].x, 0.f, pts[i].y, 1.f);
        auto np = patrolFunctionTransform * glm::vec4(pts[i + 1].x, 0.f, pts[i + 1].y, 1.f);

        rd->AddDebugSegment(0.f, cp, np, { 1.f, 1.f, 1.f });
    }
}

void AiCharacter::DrawWayPoints() const
{
    if (!bShowPath || !GetWorld()->GetWorldSettings().bShowPathfindingOccupied) return;

    if (waypoints.empty()) return;

    auto* rd = RenderDebugger::Get();
    for (size_t i = 0; i < waypoints.size() - 1; i++)
    {
        auto& cp = waypoints[i];
        auto& np = waypoints[i + 1];

        rd->AddDebugSegment(0.f, cp, np, {0.f, 1.f, 0.f});
    }
}

void AiCharacter::SetAiState(AiState::AiState state)
{
    aiState = state;
}

AiState::AiState AiCharacter::GetAiState() const
{
    return aiState;
}

bool AiCharacter::IsAiState(AiState::AiState state) const
{
    return aiState == state;
}

void AiCharacter::SetMoveState(AiMoveState::AiMoveState state)
{
    aiMoveState = state;
}

AiMoveState::AiMoveState AiCharacter::GetMoveState() const
{
    return aiMoveState;
}

bool AiCharacter::IsMoveState(AiMoveState::AiMoveState state) const
{
    return state == aiMoveState;
}

void AiCharacter::SetPatrolState(AiPatrolState::AiPatrolState state)
{
    SetAiState(AiState::Patrolling);
    aiPatrolState = state;
}

AiPatrolState::AiPatrolState AiCharacter::GetPatrolState() const
{
    return aiPatrolState;
}

bool AiCharacter::IsPatrolState(AiPatrolState::AiPatrolState state) const
{
    return aiPatrolState == state;
}

void AiCharacter::SetEntityToFollow(entt::entity entity)
{
    SetAiState(AiState::Follow);
    entityToFollow = entity;
}

void AiCharacter::SetStopFollowDist2(float dist2)
{
    stopFollowDist2 = dist2;
}

void AiCharacter::SetStopFollowThreshold(float val)
{
    stopFollowThreshold = val;
}

void AiCharacter::SetMovementSpeed(const float newSpeed)
{
    movementSpeed = newSpeed;
}

float AiCharacter::GetMovementSpeed() const
{
    return movementSpeed;
}

void AiCharacter::MoveTo(const glm::vec3& target)
{
    if (AiMoveState::None != aiMoveState) return;

    auto temp = target;
    temp.y = 0.f;

    if (aStarTimer <= 1.f)
    {
        return;
    }
    aStarTimer = 0.f;

    int iResult = CalculateAStarWayPts(GetPosition(), temp);
    if (iResult == 1)
    { 
        if (waypoints.empty()) return;

        SetAiState(AiState::Patrolling);  
        SetPatrolState(AiPatrolState::WayPoints);
        MoveToIn2D(waypoints[0]);
    }
    else if (iResult == 0)
    {
        std::cout << "A* path not found " << std::endl;
    }
    else if (iResult == -1)
    {
        std::cout << "A* start node occupied " << std::endl;
    }
    else if (iResult == -2)
    {
        std::cout << "A* end node occupied " << std::endl;
    }
}
