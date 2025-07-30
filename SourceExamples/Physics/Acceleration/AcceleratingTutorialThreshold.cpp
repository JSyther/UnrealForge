#include "AcceleratingTutorialThreshold.h"

// Constructor: Sets default values and enables ticking every frame
AAcceleratingTutorialThreshold::AAcceleratingTutorialThreshold()
{
    // Enable Tick() to be called every frame
    PrimaryActorTick.bCanEverTick = true;

    // Initialize the threshold value to 0 (start)
    Threshold = 0.f;

    // Base interpolation speed — starting speed for threshold progression
    BaseInterpSpeed = 0.1f;

    // Interpolation acceleration — how much the interp speed increases per second (acceleration)
    InterpAcceleration = 0.3f;

    // Current interpolation speed starts at the base speed
    CurrentInterpSpeed = BaseInterpSpeed;

    // Initialize tutorial phase state to Phase_0
    CurrentPhase = EPhase::Phase_0;

    // Store previous phase to detect changes and trigger events
    PreviousPhase = CurrentPhase;
}

// Called once when the game starts or actor is spawned
void AAcceleratingTutorialThreshold::BeginPlay()
{
    Super::BeginPlay();

    // (Optional) Initialization logic can be placed here if needed
}

// Called every frame, DeltaTime is the time elapsed since last frame
void AAcceleratingTutorialThreshold::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Increase current interpolation speed over time to simulate acceleration
    // This creates an effect where the threshold progresses slowly at first,
    // then speeds up as time passes (like a car accelerating)
    CurrentInterpSpeed += InterpAcceleration * DeltaTime;

    // Interpolate the Threshold value smoothly towards 1.0 using the current interpolation speed
    // FInterpTo does a smooth exponential-like interpolation based on DeltaTime and speed
    Threshold = FMath::FInterpTo(Threshold, 1.f, DeltaTime, CurrentInterpSpeed);

    // Update the current tutorial phase based on the new threshold value
    UpdatePhase();
}

// Updates the tutorial phase based on the threshold value and broadcasts event on phase change
void AAcceleratingTutorialThreshold::UpdatePhase()
{
    // Calculate the current phase index by scaling threshold (0 to 1) to 0 to 5 phases
    // FloorToInt ensures the phase changes only when threshold passes discrete boundaries
    // Clamp ensures phase index stays within valid range [0,5]
    int32 PhaseIndex = FMath::Clamp(FMath::FloorToInt(Threshold * 5.f), 0, 5);

    // Cast the integer phase index to the enum type representing phases
    CurrentPhase = static_cast<EPhase>(PhaseIndex);

    // If the phase has changed since last tick, broadcast the event and update PreviousPhase
    if (CurrentPhase != PreviousPhase)
    {
        // Broadcast phase change event for any listeners (Blueprints or C++ classes)
        OnPhaseChanged.Broadcast(CurrentPhase);

        // Log the phase change to the output log for debugging purposes
        UE_LOG(LogTemp, Log, TEXT("Tutorial Phase Changed: %d"), (int32)CurrentPhase);

        // Update previous phase to the new current phase for next comparisons
        PreviousPhase = CurrentPhase;
    }
}
