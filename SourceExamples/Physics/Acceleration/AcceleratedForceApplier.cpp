#include "AcceleratedForceApplier.h"
#include "DrawDebugHelpers.h"   // For debug visualization
#include "Components/PrimitiveComponent.h"

// Constructor: Setup defaults and enable ticking every frame
AAcceleratedForceApplier::AAcceleratedForceApplier()
{
    PrimaryActorTick.bCanEverTick = true;

    // Default force mode: Continuous force application
    ForceMode = EForceMode::ContinuousForce;

    // Default directions (pointing forward)
    ForceDirection = FVector::ForwardVector;
    TorqueAxis = FVector::UpVector;

    // Default magnitudes
    ForceMagnitude = 1000.f;
    TorqueMagnitude = 500.f;

    // Default: respect mass by default (forces scale with mass)
    bIgnoreMass = false;

    // Initialize internal state
    CurrentForceMagnitude = 0.f;

    TargetComponent = nullptr; // User must assign this in editor or code
}

void AAcceleratedForceApplier::BeginPlay()
{
    Super::BeginPlay();

    // Validate TargetComponent presence and physics simulation enabled
    if (!TargetComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] TargetComponent not assigned! Physics forces won't apply."), *GetName());
        return;
    }

    if (!TargetComponent->IsSimulatingPhysics())
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] TargetComponent is not simulating physics! Enabling simulation."), *GetName());
        TargetComponent->SetSimulatePhysics(true);
    }
}

void AAcceleratedForceApplier::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetComponent || !TargetComponent->IsSimulatingPhysics())
    {
        // Early out if no valid physics target
        return;
    }

    // Apply physics forces based on the selected mode and parameters
    ApplyPhysicsForce(DeltaTime);

    // Draw debug visualizations (optional, controlled by build flags)
    DrawDebugForces();
}

void AAcceleratedForceApplier::ApplyPhysicsForce(float DeltaTime)
{
    // Normalize directions to avoid scaling errors
    const FVector NormalizedForceDir = ForceDirection.GetSafeNormal();
    const FVector NormalizedTorqueAxis = TorqueAxis.GetSafeNormal();

    // Handle different force application modes
    switch (ForceMode)
    {
    case EForceMode::ContinuousForce:
    {
        // Simulate acceleration by increasing force magnitude over time (simple acceleration curve)
        CurrentForceMagnitude += ForceMagnitude * DeltaTime;

        // Clamp CurrentForceMagnitude so it doesn't grow indefinitely (optional, tweak as needed)
        CurrentForceMagnitude = FMath::Clamp(CurrentForceMagnitude, 0.f, ForceMagnitude * 10.f);

        // Apply continuous force scaled by mass if bIgnoreMass==false
        // bAccelChange=true means force is applied as acceleration, ignoring mass.
        TargetComponent->AddForce(NormalizedForceDir * CurrentForceMagnitude, NAME_None, bIgnoreMass);

        break;
    }
    case EForceMode::InstantImpulse:
    {
        // Apply an instantaneous velocity change (impulse)
        // bVelChange true means velocity changes directly ignoring mass.
        TargetComponent->AddImpulse(NormalizedForceDir * ForceMagnitude, NAME_None, bIgnoreMass);

        // Reset CurrentForceMagnitude since impulse is instantaneous
        CurrentForceMagnitude = 0.f;

        // For impulses, you usually want one-time application; consider disabling ticking or mode switch after impulse
        break;
    }
    case EForceMode::TorqueForce:
    {
        // Apply torque for angular acceleration
        TargetComponent->AddTorqueInRadians(NormalizedTorqueAxis * TorqueMagnitude, NAME_None, bIgnoreMass);

        break;
    }
    default:
        UE_LOG(LogTemp, Warning, TEXT("[%s] Unknown ForceMode."), *GetName());
        break;
    }
}

void AAcceleratedForceApplier::DrawDebugForces() const
{
#if ENABLE_DRAW_DEBUG
    if (!TargetComponent)
        return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    const FVector Origin = TargetComponent->GetComponentLocation();

    switch (ForceMode)
    {
    case EForceMode::ContinuousForce:
    {
        const FVector EndPoint = Origin + ForceDirection.GetSafeNormal() * CurrentForceMagnitude * 0.01f; // Scale down for visualization
        DrawDebugLine(World, Origin, EndPoint, FColor::Blue, false, -1.f, 0, 3.f);
        DrawDebugString(World, EndPoint, TEXT("Continuous Force"), nullptr, FColor::Blue, 0.f, true);
        break;
    }
    case EForceMode::InstantImpulse:
    {
        const FVector EndPoint = Origin + ForceDirection.GetSafeNormal() * ForceMagnitude * 0.01f;
        DrawDebugLine(World, Origin, EndPoint, FColor::Green, false, 1.f, 0, 3.f);
        DrawDebugString(World, EndPoint, TEXT("Instant Impulse"), nullptr, FColor::Green, 0.f, true);
        break;
    }
    case EForceMode::TorqueForce:
    {
        const FVector EndPoint = Origin + TorqueAxis.GetSafeNormal() * TorqueMagnitude * 0.01f;
        DrawDebugLine(World, Origin, EndPoint, FColor::Red, false, -1.f, 0, 3.f);
        DrawDebugString(World, EndPoint, TEXT("Torque Force"), nullptr, FColor::Red, 0.f, true);
        break;
    }
    }
#endif
}
