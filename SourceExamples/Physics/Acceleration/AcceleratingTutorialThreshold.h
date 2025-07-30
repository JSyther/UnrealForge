#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AcceleratingTutorialThreshold.generated.h"

UENUM(BlueprintType)
enum class EPhase : uint8
{
    Phase_0 UMETA(DisplayName = "Phase 0"),
    Phase_1 UMETA(DisplayName = "Phase 1"),
    Phase_2 UMETA(DisplayName = "Phase 2"),
    Phase_3 UMETA(DisplayName = "Phase 3"),
    Phase_4 UMETA(DisplayName = "Phase 4"),
    Phase_5 UMETA(DisplayName = "Phase 5")
};

// Delegate for phase changed event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChangedSignature, ETutorialPhase, NewPhase);

UCLASS()
class UNREALENGINE_API AAcceleratingTutorialThreshold : public AActor
{
    GENERATED_BODY()

public:
    AAcceleratingTutorialThreshold();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Current Threshold (0..1)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float Threshold;

    // Current Phase
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EPhase CurrentPhase;

    // Base interpolation speed (starting speed)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.01"))
    float BaseInterpSpeed;

    // Acceleration amount for interpolation speed (how much it increases per second)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
    float InterpAcceleration;

    // Current interpolating speed (starts at BaseInterpSpeed and increases)
    float CurrentInterpSpeed;

    // Event dispatcher for phase change
    UPROPERTY(BlueprintAssignable)
    FOnPhaseChangedSignature OnPhaseChanged;

private:
    void UpdatePhase();

    // Store previous phase for event checking
    EPhase PreviousPhase;
};
