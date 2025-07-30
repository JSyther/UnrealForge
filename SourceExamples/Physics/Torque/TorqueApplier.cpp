#include "TorqueApplier.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"

ATorqueApplier::ATorqueApplier()
{
    PrimaryActorTick.bCanEverTick = true;

    // Defaults
    TargetComponent = nullptr;

    TorqueMode = ETorqueApplicationMode::ConstantTorque;
    TorqueAxis = FVector::UpVector;
    TorqueMagnitude = 1000.f;
    DampingFactor = 1.f;
    bIgnoreMass = false;

    bImpulseApplied = false;
}

void ATorqueApplier::BeginPlay()
{
    Super::BeginPlay();

    if (!TargetComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] TargetComponent is not assigned!"), *GetName());
        return;
    }

    if (!TargetComponent->IsSimulatingPhysics())
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] TargetComponent is not simulating physics. Enabling physics simulation."), *GetName());
        TargetComponent->SetSimulatePhysics(true);
    }
}

void ATorqueApplier::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetComponent || !TargetComponent->IsSimulatingPhysics())
        return;

    ApplyTorque(DeltaTime);

    DrawDebugInfo();
}

void ATorqueApplier::ApplyTorque(float DeltaTime)
{
    FVector NormalizedAxis = TorqueAxis.GetSafeNormal();

    switch (TorqueMode)
    {
        case ETorqueApplicationMode::ConstantTorque:
        {
            // Apply steady torque every tick, optionally ignoring mass for direct angular acceleration
            TargetComponent->AddTorqueInRadians(NormalizedAxis * TorqueMagnitude, NAME_None, bIgnoreMass);
            break;
        }

        case ETorqueApplicationMode::ImpulseTorque:
        {
            // Apply one-time angular impulse
            if (!bImpulseApplied)
            {
                TargetComponent->AddAngularImpulseInRadians(NormalizedAxis * TorqueMagnitude, NAME_None, bIgnoreMass);
                bImpulseApplied = true;
            }
            break;
        }

        case ETorqueApplicationMode::DampedTorque:
        {
            // Apply torque plus damping (angular friction)
            // Get current angular velocity and apply torque opposing it multiplied by damping factor
            FVector AngularVelocity = TargetComponent->GetPhysicsAngularVelocityInRadians();

            // Torque to apply this tick
            FVector AppliedTorque = NormalizedAxis * TorqueMagnitude;

            // Damping torque opposes angular velocity component along torque axis
            float AngularVelAlongAxis = FVector::DotProduct(AngularVelocity, NormalizedAxis);
            FVector DampingTorque = -NormalizedAxis * AngularVelAlongAxis * DampingFactor;

            FVector TotalTorque = AppliedTorque + DampingTorque;

            TargetComponent->AddTorqueInRadians(TotalTorque, NAME_None, bIgnoreMass);
            break;
        }

        default:
            UE_LOG(LogTemp, Warning, TEXT("[%s] Unknown TorqueMode!"), *GetName());
            break;
    }
}

void ATorqueApplier::DrawDebugInfo() const
{
#if ENABLE_DRAW_DEBUG
    if (!TargetComponent)
        return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    FVector Location = TargetComponent->GetComponentLocation();
    FVector AxisNormalized = TorqueAxis.GetSafeNormal();

    // Draw torque vector (scaled down for visualization)
    FVector TorqueVector = AxisNormalized * TorqueMagnitude * 0.01f;
    DrawDebugLine(World, Location, Location + TorqueVector, FColor::Red, false, -1.f, 0, 3.f);
    DrawDebugString(World, Location + TorqueVector, TEXT("Torque"), nullptr, FColor::Red, 0.f, true);

    // Draw angular velocity vector
    FVector AngularVel = TargetComponent->GetPhysicsAngularVelocityInRadians();
    FVector AngularVelScaled = AngularVel * 0.1f; // scaled down for debug

    DrawDebugLine(World, Location, Location + AngularVelScaled, FColor::Green, false, -1.f, 0, 3.f);
    DrawDebugString(World, Location + AngularVelScaled, TEXT("Angular Velocity"), nullptr, FColor::Green, 0.f, true);
#endif
}
