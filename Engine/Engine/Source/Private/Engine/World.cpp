// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Core/Application.h"
#include "Engine/ActorUtility.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Physics/PhysicCompontent.h"
#include "User/LocalEgo.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Debug/DebugTraceLine.h"
#include "Cli/CommandLineInterface.h"
#include "Debug/DebugTraceSphere.h"
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Jafg::LWorld::LWorld(const LSimpleString& InHumanReadableName, const EWorldState::Type InWorldType): WorldState(InWorldType)
{
    LObjectContext::operator=(GlobalCarnifex);
    this->SetHumanReadableName(InHumanReadableName);
    check( this->WorldState != EWorldState::None )

    return;
}

Jafg::LEngine* Jafg::LWorld::GetEngine() const
{
    check( GEngine )
    return GEngine;
}

Jafg::LLocalEgo* Jafg::LWorld::GetLocalEgo() const
{
    return this->GetEngine()->GetLocalEgo();
}

Jafg::APersonaController* Jafg::LWorld::GetLocalController() const
{
    return this->GetLocalEgo()->GetPossessed();
}

Jafg::LCommandLineInterface* Jafg::LWorld::GetCommandLineInterface() const
{
    return this->GetEngine()->GetCommandLineInterface();
}

Jafg::APawn* Jafg::LWorld::GetLocalPawn() const
{
    APawn* Out = this->GetLocalController()->GetPossessed();

    if (Out && Out->GetOuter() == this)
    {
        return Out;
    }

    return nullptr;
}

void Jafg::LWorld::InitializeWorld(const LLevel& Level)
{
    this->RealTimeWhenWorldWasLaunched = static_cast<float>(Application::GetDeltaSinceStaticStorageInitialization());
    check( this->RealTimeWhenWorldWasLaunched > 0.0f )

    this->WorldState = EWorldState::Initializing;

    this->GetEngine()->OnWorldBeginLife.Broadcast(this);

    LOG_VERBOSE(LogWorld, "Initializing {} level actors.", this->Actors.GetSize())
    for (AActor* Actor : this->Actors)
    {
        MakeDeferredActorFinal(Actor);
    }

    this->Collection.DeferredInitialize(this);
    this->Collection.InitializeSubsystems(JWorldSubsystem::StaticClass());

    this->WorldState = EWorldState::Running;

    return;
}

void Jafg::LWorld::Tick(const float DeltaTime)
{
    this->AcquireTickableObjectsLock();
    for (LTickableObject* Tickable : this->TickableObjects)
    {
        Tickable->Tick(DeltaTime);
    }
    this->ReleaseTickableObjectsLock();
    for (LTickableObject* Tickable : this->DeletedTickableObjects)
    {
        this->TickableObjects.RemoveOnceChecked(Tickable);
    }
    this->DeletedTickableObjects.Empty();

    return;
}

void Jafg::LWorld::Draw(const LViewport& Viewport, const LEye& Eye) const
{
    const bool bUpdateFrustum    = this->GetLocalEgo()->GetVariable_UpdateFrustum();
    const bool bVisualizeFrustum = this->GetLocalEgo()->GetVariable_VisualizeFrustum();

    LMatrix P;
    LMatrix V;
    if (const auto Cache = this->EyeToMatrices.find(&Eye); bUpdateFrustum || Cache == this->EyeToMatrices.end())
    {
        P = Maths::MakePerspectiveProjectionMatrix(
            Maths::ToRadians(Eye.GetDegYFov()),
            static_cast<float>(Viewport.GetDimensions().X) / static_cast<float>(Viewport.GetDimensions().Y),
            Eye.GetNearFrustum(), Eye.GetFarFrustum()
        );
        V = Eye.GetViewMatrix();
        this->EyeToMatrices[&Eye] = { P, V };
    }
    else
    {
        P = Cache->second.P;
        V = Cache->second.V;
    }

    // const LMatrix Projection = Maths::MakePerspectiveProjectionMatrix
    // (
    //     Maths::ToRadians(Eye.GetDegYFov()),
    //     static_cast<float>(Viewport.GetDimensions().X) / static_cast<float>(Viewport.GetDimensions().Y),
    //     0.1f, 10.0f
    // );
    // const LMatrix View = Eye.GetViewMatrix();
    glm::vec4 ndcCorners[8] = {
        {-1, -1, -1, 1}, // Near Bottom Left
        { 1, -1, -1, 1}, // Near Bottom Right
        {-1,  1, -1, 1}, // Near Top Left
        { 1,  1, -1, 1}, // Near Top Right
        {-1, -1,  1, 1}, // Far Bottom Left
        { 1, -1,  1, 1}, // Far Bottom Right
        {-1,  1,  1, 1}, // Far Top Left
        { 1,  1,  1, 1}  // Far Top Right
    };
    glm::mat4 projMatrix = glm::make_mat4(&P.Matrix[0][0]);
    glm::mat4 viewMatrix = glm::make_mat4(&V.Matrix[0][0]);
    glm::mat4 invVP = glm::inverse(projMatrix * viewMatrix);
    glm::vec3 worldCorners[8];
    for (int i = 0; i < 8; i++) {
        glm::vec4 worldPos = invVP * ndcCorners[i];
        worldCorners[i] = glm::vec3(worldPos) / worldPos.w; // Perspective divide
    }
    // std::vector<std::pair<int, int>> frustumEdges = {
    //     {0, 1}, {1, 3}, {3, 2}, {2, 0}, // Near Plane Edges
    //     {4, 5}, {5, 7}, {7, 6}, {6, 4}, // Far Plane Edges
    //     {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting Near and Far Planes
    // };

    for (const AActor* Actor : this->Actors)
    {
        check( Actor->IsGarbage() == false )

        if (Actor->IsRendererComponentValid())
        {
            Actor->GetRendererComponent()->Draw(Viewport, Eye);
        }

        continue;
    }

    if (bVisualizeFrustum)
    {
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[0].x, worldCorners[0].y, worldCorners[0].z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[1].x, worldCorners[1].y, worldCorners[1].z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[2].x, worldCorners[2].y, worldCorners[2].z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[3].x, worldCorners[3].y, worldCorners[3].z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[4].x, worldCorners[4].y, worldCorners[4].z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[5].x, worldCorners[5].y, worldCorners[5].z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[6].x, worldCorners[6].y, worldCorners[6].z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(worldCorners[7].x, worldCorners[7].y, worldCorners[7].z)
        ));
    }

    for (LTemporalWorldObject* const& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->Draw(*this, Viewport, Eye);
    }

    return;
}

#if AS_CLIENT
void Jafg::LWorld::LateTick(const float DeltaTime)
{
    for (LTemporalWorldObject* const& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->ReduceLifeTime(DeltaTime);
    }
    this->TemporalObjects.RemoveAllByPredicate( [] (LTemporalWorldObject*& TemporalObject)
    {
        if (TemporalObject->IsAlive())
        {
            return false;
        }

        delete TemporalObject;
        TemporalObject = nullptr;
        return true;
    });

    return;
}
#endif /* AS_CLIENT */

void Jafg::LWorld::TearDownContext()
{
    check( this->GetWorldState() == EWorldState::Running )
    this->WorldState = EWorldState::TearingDown;

    LOG_VERBOSE(LogWorld, "Tearing down world subsystems for world [{}].", this->GetHumanReadableName())
    this->Collection.TearDownSubsystems();

    // Preserve order!
    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, Tasks::Private::RunAllTasks);
    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, Tasks::Private::RunAllTasks);
    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, Tasks::Private::RunAllTasks);

    LOG_VERBOSE(LogWorld, "Killing {} actors of world [{}].", this->Actors.GetSize(), this->GetHumanReadableName())
    for (AActor* Actor : this->Actors)
    {
        Actor->KillYourSelfNow();
    }

    this->Actors.Empty();
    this->TickableObjects.Empty();
    this->DeletedTickableObjects.Empty();

    LObjectContext::TearDownContext();

    this->WorldState = EWorldState::WaitingForKill;

    return;
}

void Jafg::LWorld::RegisterTickableObject(LTickableObject* Tickable)
{
    if (this->TickableObjects.Contains(Tickable))
    {
        panic( "Found duplicate tickable object" )
        return;
    }

    this->TickableObjects.Add(Tickable);

    return;
}

void Jafg::LWorld::UnregisterTickableObject(LTickableObject* Tickable)
{
    if (this->TickableObjects.RemoveOnce(Tickable))
    {
        return;
    }

    panic( "Failed to find tickable object" )

    return;
}

float Jafg::LWorld::GetRealTimeSecondsSinceWorldLaunch() const
{
    const float Now = static_cast<float>(Application::GetDeltaSinceStaticStorageInitialization());
    return Now - this->RealTimeWhenWorldWasLaunched;
}

bool Jafg::LWorld::LineTraceByChannel(
    TdhArray<LHitResult>& OutHits,
    const LVector& Begin,
    const LVector& End,
    const ECollisionChannel::Type Channel,
    const LCollisionQueryParams& Params
) const
{
    check( (Begin - End).Magnitude() > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small distances." )

    LHitResult Dummy;
    for (const AActor* Actor : this->Actors)
    {
        if (Actor->GetPhysicsComponent()->Sweep(Begin, End, Dummy))
        {
            OutHits.Add(Dummy);
            Dummy.Reset();

            if (Params.bSingleHit)
            {
                break;
            }
        }

        continue;
    }

    return OutHits.IsEmpty() == false;
}
