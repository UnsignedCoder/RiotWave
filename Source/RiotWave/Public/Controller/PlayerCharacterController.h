// Copyright notice...

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Interface/Weapon/WeaponDetectionInterface.h"
#include "PlayerCharacterController.generated.h"

class APlayerCharacter;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

/**
 * Primary controller class managing player input and character interactions.
 * 
 * This controller implements both standard FPS movement controls and weapon handling,
 * separated into distinct mapping contexts to allow for dynamic enabling/disabling
 * of different control schemes based on game state (e.g., during cutscenes or when
 * weapons are unavailable).
 * 
 * Implements IWeaponDetectionInterface to handle weapon pickup events and automatically
 * enable weapon-specific controls when appropriate.
 */
UCLASS()
class RIOTWAVE_API APlayerCharacterController : public APlayerController, public IWeaponDetectionInterface {
    GENERATED_BODY()

protected:
    /** 
     * Initializes controller-character relationship and input mappings when possessing a pawn.
     * Called automatically by the engine when this controller takes control of a pawn.
     */
    virtual void OnPossess(APawn* aPawn) override;

    /**
     * Processes movement input and translates it to character movement.
     * Uses Enhanced Input system for more precise control and better input handling across different platforms.
     * 
     * @param Value - Contains both the X (forward/backward) and Y (left/right) movement values
     */
    void HandleMoveAction(const FInputActionValue& Value);

    /**
     * Handles camera/view rotation input from mouse or analog stick.
     * Separate from movement to allow for independent sensitivity settings and input processing.
     * 
     * @param Value - Contains both the pitch (up/down) and yaw (left/right) rotation values
     */
    void HandleLookAction(const FInputActionValue& Value);

    /**
     * Initiates jump sequence.
     * Separated from HandleStopJumpAction to support both tap-to-jump and hold-to-jump mechanics.
     */
    void HandleJumpAction();

    /**
     * Ends jump sequence for hold-to-jump functionality.
     * Separate method allows for more precise jump height control.
     */
    void HandleStopJumpAction();

    /**
     * Processes weapon firing input.
     * Only active when WeaponHandlingMappingContext is enabled (i.e., player has a weapon).
     */
    void HandleWeapWeaponFireAction();

private:
    // Reference to the controlled character, cached for performance
    UPROPERTY()
    TObjectPtr<APlayerCharacter> FPSCharacter;

    /**
     * Input mapping context for basic movement controls.
     * Separated from weapon handling to allow basic movement even when weapons are unavailable.
     */
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> TraversalMappingContext;

    /**
     * Input mapping context for weapon-related actions.
     * Separated from traversal to easily disable weapon actions in weapon-free zones or during cutscenes.
     */
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> WeaponHandlingMappingContext;

    // Core input actions, exposed to editor for easy remapping
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> WeaponFireAction;

public:
    /**
     * Implements IWeaponDetectionInterface.
     * Called when the player picks up a weapon, enabling weapon-specific input handling.
     * 
     * @param OwningActor - The weapon actor that was picked up
     */
    virtual void OnWeaponPicked(AActor* OwningActor) override;
};