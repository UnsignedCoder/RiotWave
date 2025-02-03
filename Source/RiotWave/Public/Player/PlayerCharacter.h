// RiotWave - First Person Shooter Game
// PlayerCharacter.h - Core player character class that handles player embodiment and weapon interactions
//
// This header defines the player character class which serves as the primary way players interact
// with the game world. It inherits from Character to leverage Unreal's character movement system
// and implements WeaponDetectionInterface to support weapon pickup and interaction mechanics.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Weapon/WeaponDetectionInterface.h"
#include "PlayerCharacter.generated.h"

class UWeaponHandlingComponent;
class USpringArmComponent;
class UCameraComponent;

/**
 * @class APlayerCharacter
 * 
 * Main player character class that represents the player in the game world.
 * 
 * Design Decisions:
 * - Inherits from Character instead of Pawn to utilize built-in movement capabilities
 * - Implements WeaponDetectionInterface to separate weapon interaction logic from character logic
 * - Uses component-based architecture for better modularity and easier testing
 * - First-person perspective chosen for immersive shooter gameplay
 */
UCLASS()
class RIOTWAVE_API APlayerCharacter : public ACharacter, public IWeaponDetectionInterface
{
    GENERATED_BODY()

public:
    /** 
     * Constructor that sets up default component hierarchy and initial values.
     * Components are created here rather than Blueprint to ensure consistency
     * across all instances.
     */
    APlayerCharacter();

    /** 
     * Provides access to weapon handling logic while maintaining encapsulation.
     * Required by WeaponDetectionInterface to support weapon system modularity.
     */
    virtual UWeaponHandlingComponent* GetWeaponHandlingComponent() override;

protected:
    /** 
     * Initializes character state when gameplay begins.
     * Override this instead of constructor for runtime initialization
     * since component references are guaranteed to be valid here.
     */
    virtual void BeginPlay() override;

public: 
    /** 
     * Handles per-frame updates for continuous character behavior.
     * @param DeltaTime - Time elapsed since last frame, used for smooth updates
     */
    virtual void Tick(float DeltaTime) override;

private:
    /** 
     * Spring arm provides smooth camera motion and collision detection.
     * Separated from camera to allow for independent camera behavior adjustments.
     */
    UPROPERTY(VisibleAnywhere, Category = "Player")
    TObjectPtr<USpringArmComponent> SpringArm;

    /** 
     * Main camera for first-person view.
     * Separate component allows for easy switching between camera perspectives
     * and independent camera effects processing.
     */
    UPROPERTY(VisibleAnywhere, Category = "Player")
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    /** 
     * Visual representation of the player character.
     * Exposed to Blueprint for visual customization while keeping logic in C++.
     * Made private with accessor to maintain encapsulation while allowing BP access.
     */
    UPROPERTY(EditAnywhere, Category = "Player", BlueprintReadOnly, meta=(AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> PlayerMesh;

    /** 
     * Handles all weapon-related functionality.
     * Separated into component to:
     * 1. Keep weapon logic independent of character logic
     * 2. Allow easy addition/removal of weapon capabilities
     * 3. Simplify testing of weapon mechanics
     */
    UPROPERTY(VisibleAnywhere, Category = "Player")
    TObjectPtr<UWeaponHandlingComponent> WeaponHandlingComponent;

    UPROPERTY(EditAnywhere, Category = "Player", meta=(AllowPrivateAccess = true))
    float MaxHealth = 25000;

    UPROPERTY(VisibleAnywhere, Category = "Player", meta=(AllowPrivateAccess = true))
    float Health = MaxHealth;

    UPROPERTY(EditAnywhere, Category = "Player", meta=(AllowPrivateAccess = true))
    TObjectPtr<USoundBase> ImpactSound;

    UPROPERTY(EditAnywhere, Category = "Player", meta=(AllowPrivateAccess = true))
    TObjectPtr<USoundBase> DeathSound;

public:
    void Death();
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    

public:
    /** 
     * Provides controlled access to player mesh.
     * FORCEINLINE used for performance as this may be called frequently
     * for animations and visual effects.
     */
    FORCEINLINE USkeletalMeshComponent* GetPlayerMesh() const { return PlayerMesh; }
};