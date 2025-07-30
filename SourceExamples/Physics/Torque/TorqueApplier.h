#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TorqueApplier.generated.h"

/**
 * ETorqueApplicationMode
 * Enum to specify the torque application method.
 */
UENUM(BlueprintType)
enum class ETorqueApplicationMode : uint8
{
    ConstantTorque UMETA(DisplayName="Constant Torque"),
    ImpulseTorque UMETA(DisplayName="Impulse Torque"),
    DampedTorque UMETA(DisplayName="Damped Torque")
};

/**
 * ATorqueApplier
 * 
 * Actor that applies torque to a target physics component.
 * Supports constant torque, impulse torque, and damped torque modes.
 * Includes debug visualization of torque vectors and angular velocity.
 */
UCLASS()
class YOURPROJECT_API ATorqueApplier : public AActor
{
    GENERATED_BODY()

public:
    ATorqueApplier();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /** The component to apply torque on; must simulate physics */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Torque")
    UPrimitiveComponent* TargetComponent;

    /** Torque application mode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Torque")
    ETorqueApplicationMode TorqueMode;

    /** Axis along which torque is applied (normalized internally) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Torque", meta=(MakeEditWidget=true))
    FVector TorqueAxis;

    /** Magnitude of torque */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Torque", meta=(ClampMin="0.0"))
    float TorqueMagnitude;

    /** Damping coefficient for DampedTorque mode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Torque", meta=(ClampMin="0.0", ClampMax="10.0"))
    float DampingFactor;

    /** Ignore mass when applying torque */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Torque")
    bool bIgnoreMass;

private:
    /** Tracks if impulse torque has been applied */
    bool bImpulseApplied;

    /** Applies torque based on current mode */
    void ApplyTorque(float DeltaTime);

    /** Draw debug info */
    void DrawDebugInfo() const;
};
