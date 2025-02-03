// WeaponBase.h - Core weapon class for RiotWave FPS Game
//
// This class provides the foundation for all pickupable weapons in the game.
// It handles key aspects like collision detection for pickup, FX and sound management,
// and interactions with the player character. The design prioritizes modularity 
// and extensibility through inheritance for different weapon types.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class APlayerCharacter;
class USphereComponent;

/**
* Base weapon actor that handles core weapon functionality.
* 
* Inherits from AActor to allow weapons to exist independently in the world,
* which enables features like:
* - Weapon pickup from ground/weapon spawners
* - Weapon drops from players/enemies
* - Independent physics simulation when dropped
* - Networked weapon replication
*/
UCLASS()
class RIOTWAVE_API AWeaponBase : public AActor {
   GENERATED_BODY()

public:
   /** 
    * Constructor that establishes the base component hierarchy.
    * Components are created here rather than BeginPlay to ensure proper
    * editor preview and Blueprint inheritance behavior.
    */
   AWeaponBase();

protected:
   /** 
    * Handles weapon pickup through overlap detection.
    * Separated into a UFUNCTION to support the UE4 delegate system
    * and to isolate pickup logic for easier maintenance.
    */
   UFUNCTION()
   void OnWeaponCollisionBeginOverlap(
       UPrimitiveComponent* OverlappedComponent, 
       AActor* OtherActor,
       UPrimitiveComponent* OtherComp,
       int32 OtherBodyIndex,
       bool bFromSweep,
       const FHitResult& SweepResult
   );

   /** 
    * Runtime initialization that can't happen in constructor.
    * Kept virtual to allow derived weapons to add their own initialization
    * while maintaining base functionality.
    */
   virtual void BeginPlay() override;

   /**
    * Blueprint event for weapon pickup customization.
    * Made BlueprintImplementableEvent to allow designers to:
    * - Add custom pickup animations
    * - Trigger game events
    * - Modify weapon state
    * without requiring C++ changes.
    */
   UFUNCTION(BlueprintImplementableEvent)
   void OnWeaponPicked(AActor* OwningActor);

public:
   /** 
    * Frame update handler.
    * Kept virtual to allow derived weapons to add continuous behaviors
    * like automatic fire, heat management, or charge-up effects.
    */
   virtual void Tick(float DeltaTime) override;

private:
   /** Root scene component for transform hierarchy */
   UPROPERTY(VisibleAnywhere)
   TObjectPtr<USceneComponent> DefaultSceneRoot;
   
   /** 
    * Reference to the player character that owns this weapon.
    * Tracked to handle weapon attachment and input binding.
    */
   UPROPERTY(VisibleAnywhere)
   TObjectPtr<APlayerCharacter> OwningPlayer;
   
   /** 
    * Visual mesh for the weapon.
    * Made BlueprintReadOnly to allow animation blueprints to reference
    * the mesh while keeping core logic in C++.
    */
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = true))
   TObjectPtr<USkeletalMeshComponent> PickupWeaponMesh;

   /** 
    * Collision sphere that detects valid pickup attempts.
    * Separated from the weapon mesh to allow for flexible pickup ranges.
    */
   UPROPERTY(VisibleAnywhere, Category = "Weapon")
   TObjectPtr<USphereComponent> WeaponCollision;

   /** Sound cue played on successful pickup for player feedback */
   UPROPERTY(EditAnywhere, Category = "Weapon")
   TObjectPtr<USoundBase> PickupSound;

   // Weapon firing effects - separated for easy customization per weapon type
   /** Particle system for barrel flash when firing */
   UPROPERTY(EditAnywhere, Category = "Weapon|WeaponFX")
   UParticleSystem* MuzzleFlash;

   /** Particle system played at bullet impact point */
   UPROPERTY(EditAnywhere, Category = "Weapon|WeaponFX")
   UParticleSystem* ImpactParticle;
   
   /** Optional beam/trace effect for projectile visualization */
   UPROPERTY(EditAnywhere, Category = "Weapon|WeaponFX")
   UParticleSystem* BeamTraceParticle;

   /** Sound played when weapon fires */
   UPROPERTY(EditAnywhere, Category = "Weapon|WeaponFX")
   TObjectPtr<USoundBase> FireSound;

   /** 
    * Socket name on weapon mesh for spawning effects.
    * Modifiable per weapon to support different weapon models
    * while maintaining consistent effect spawn behavior.
    */
   UPROPERTY(EditAnywhere, Category = "Weapon|WeaponFX")
   FName WeaponSocketName = "Barrel Socket";

   UPROPERTY(EditAnywhere, Category = "Weapon")
   float BaseDamage;

   UPROPERTY(EditAnywhere, Category = "Weapon")
   float HeadshotMultiplier;

   UPROPERTY(EditAnywhere, Category = "Weapon")
   TObjectPtr<UAnimMontage> WeaponFIreMontage;
};