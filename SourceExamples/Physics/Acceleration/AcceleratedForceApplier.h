#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AcceleratedForceApplier.generated.h"

/**
 * EForceMode
 * Enum representing different ways to apply force.
 * - ContinuousForce: Force applied every tick, simulating acceleration.
 * - InstantImpulse: Instantaneous velocity change.
 * - TorqueForce: Rotational force applied around an axis.
 */
UENUM(BlueprintType)
enum class EForceMode : uint8
{
    ContinuousForce UMETA(DisplayName = "Continuous Force"),
    InstantImpulse UMETA(DisplayName = "Instant Impulse"),
    TorqueForce UMETA(DisplayName = "Torque Force")
};

/**
 * AAcceleratedForceApplier
 * 
 * Actor class designed to demonstrate and control the application of physics forces and impulses on a target component.
 * Features:
 * - Apply continuous forces with acceleration effect.
 * - Apply instantaneous impulses.
 * - Apply torque for rotational acceleration.
 * - Visual debug lines for all applied forces.
 * - Fully customizable via Blueprint and C++.
 */
UCLASS()
class YOURPROJECT_API AAcceleratedForceApplier : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor’s properties
    AAcceleratedForceApplier();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame, delta time provided
    virtual void Tick(float DeltaTime) override;

    /** Target component to apply physics forces on.
     * Must simulate physics to receive forces.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Target")
    UPrimitiveComponent* TargetComponent;

    /** Mode of force application, choose between continuous force, impulse, or torque */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Force")
    EForceMode ForceMode;

    /** Direction vector for force or impulse (normalized internally) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Force", meta=(MakeEditWidget=true))
    FVector ForceDirection;

    /** Magnitude scalar for force or impulse */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Force", meta=(ClampMin = "0.0"))
    float ForceMagnitude;

    /** Torque axis and magnitude (only used if ForceMode == TorqueForce) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Torque")
    FVector TorqueAxis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Torque", meta=(ClampMin = "0.0"))
    float TorqueMagnitude;

    /** If true, forces ignore mass and act as direct acceleration/velocity changes */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics|Force")
    bool bIgnoreMass;

private:
    /** Current accumulated continuous force magnitude for acceleration simulation */
    float CurrentForceMagnitude;

    /** Applies physics forces based on current mode and parameters */
    void ApplyPhysicsForce(float DeltaTime);

    /** Draws debug visualization for applied forces */
    void DrawDebugForces() const;
};
