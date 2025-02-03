// Copyright notice...

#include "Controller/PlayerCharacterController.h"
#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Weapon/WeaponHandlingComponent.h"

void APlayerCharacterController::OnPossess(APawn* aPawn) {
    // Always call parent implementation first to ensure proper initialization chain
    Super::OnPossess(aPawn);

    // Cache the controlled character for performance and type safety
    FPSCharacter = Cast<APlayerCharacter>(aPawn);
    if (FPSCharacter) {
        // Verify we're using Enhanced Input system - crash if not since this controller requires it
        UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
        checkf(EnhancedInputComponent, TEXT("Enhanced Input system not valid"))

        // Set up basic movement controls at priority 0 (base level)
        // Movement mapping is added first since it's our core control scheme
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
            Subsystem->AddMappingContext(TraversalMappingContext, 0);
        }

        // Bind all movement-related actions
        // Using Triggered for continuous actions (move/look) and Started/Completed for discrete actions (jump)
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleMoveAction);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleLookAction);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacterController::HandleJumpAction);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacterController::HandleStopJumpAction);
    }
}

void APlayerCharacterController::HandleMoveAction(const FInputActionValue& Value) {
    // Extract 2D movement value - X for right/left, Y for forward/backward
    const FVector2D MovementValue = Value.Get<FVector2D>();

    // Convert 2D input into world-space movement using character's orientation
    // Y axis maps to forward/backward motion, X axis to right/left strafing
    FPSCharacter->AddMovementInput(FPSCharacter->GetActorForwardVector(), MovementValue.Y);
    FPSCharacter->AddMovementInput(FPSCharacter->GetActorRightVector(), MovementValue.X);
}

void APlayerCharacterController::HandleLookAction(const FInputActionValue& Value) {
    // Extract 2D look value - X for yaw (left/right), Y for pitch (up/down)
    const FVector2D LookAxisValue = Value.Get<FVector2D>();

    // Apply rotation using UE's built-in controller rotation system
    // This automatically handles smoothing and platform-specific sensitivity
    FPSCharacter->AddControllerYawInput(LookAxisValue.X);
    FPSCharacter->AddControllerPitchInput(LookAxisValue.Y);
}

void APlayerCharacterController::HandleJumpAction() {
    // Delegate to character's jump system which handles actual jump mechanics
    FPSCharacter->Jump();
}

void APlayerCharacterController::HandleStopJumpAction() {
    // Stop jumping when button is released - enables variable jump heights
    FPSCharacter->StopJumping();
}

void APlayerCharacterController::HandleWeapWeaponFireAction() {
    // Delegate weapon firing to the dedicated weapon handling component
    FPSCharacter->GetWeaponHandlingComponent()->FIreWeapon();
}

void APlayerCharacterController::OnWeaponPicked(AActor* OwningActor) {
    // Verify the weapon was picked up by our controlled character
    if (FPSCharacter == Cast<APlayerCharacter>(OwningActor)) {
        if (FPSCharacter) {
            // Re-verify Enhanced Input system is still valid
            UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
            checkf(EnhancedInputComponent, TEXT("Enhanced Input system not valid"))

            // Add weapon controls at same priority as movement
            // This ensures consistent input handling between systems
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
                Subsystem->AddMappingContext(WeaponHandlingMappingContext, 0);
            }

            // Bind weapon firing - uses Triggered for continuous firing support
            EnhancedInputComponent->BindAction(WeaponFireAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::HandleWeapWeaponFireAction);
        }
    }
}