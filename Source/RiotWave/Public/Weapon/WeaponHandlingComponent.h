// WeaponHandlingComponent.h - Manages equipped weapon behavior and effects
//
// This component handles weapon functionality after pickup, separating the concerns
// of weapon existence in the world (WeaponBase) from weapon usage mechanics.
// It manages weapon mesh attachment, firing logic, effects, and player interaction.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponHandlingComponent.generated.h"

/**
* Data structure that encapsulates all visual and audio effects for a weapon.
* 
* Using a struct instead of individual parameters:
* 1. Simplifies parameter passing between classes
* 2. Makes it easier to add new effect types later
* 3. Keeps effect initialization data organized
* 4. Provides a single point of default initialization
*/
USTRUCT()
struct FInitWeaponProperties {
	GENERATED_BODY()

	/** Default constructor zeros all effect pointers for safety */
	FInitWeaponProperties() {
		MuzzleFlash = nullptr;
		ImpactParticle = nullptr;
		BeamTraceParticle = nullptr;
		FireSound = nullptr;
		Damage = 0.0f;
		HSMultiplier = 1.0f;
		WeaponFire = nullptr;
	}

	/** 
	* Convenience constructor for initializing all properties at once.
	* Parameters named distinctly from members to avoid shadowing.
	*/
	FInitWeaponProperties( UParticleSystem* MFlash, UParticleSystem* IParticle, UParticleSystem* BTParticle, USoundBase* FSound, const FName SocketName, float WDamage, float HMultiplier, UAnimMontage* WFIre ) :
		WeaponSocketName(SocketName), MuzzleFlash(MFlash), ImpactParticle(IParticle), BeamTraceParticle(BTParticle), FireSound(FSound), Damage(WDamage), HSMultiplier(HMultiplier), WeaponFire(WFIre)  {}

	/** Socket name for effect spawn location */
	FName WeaponSocketName;

	/** Flash effect at weapon muzzle when firing */
	UPROPERTY()
	TObjectPtr<UParticleSystem> MuzzleFlash;

	/** Particle effect played at bullet impact point */
	UPROPERTY()
	TObjectPtr<UParticleSystem> ImpactParticle;

	/** Optional beam effect for bullet trajectory visualization */
	UPROPERTY()
	TObjectPtr<UParticleSystem> BeamTraceParticle;

	/** Sound played when weapon fires */
	UPROPERTY()
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY()
	float Damage;

	UPROPERTY()
	float HSMultiplier;

	UPROPERTY()
	TObjectPtr<UAnimMontage> WeaponFire;
};

class APlayerCharacter;

/**
* Component that manages weapon functionality when equipped by a player.
* 
* Design Philosophy:
* - Component-based to separate weapon logic from character logic
* - Handles all weapon behavior post-pickup
* - Manages weapon effects and feedback
* - Coordinates between player input and weapon response
*/
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RIOTWAVE_API UWeaponHandlingComponent : public UActorComponent {
	GENERATED_BODY()

public:
	/** Sets up default component state */
	UWeaponHandlingComponent();

	/** Handles weapon firing logic and effect triggering */
	void FIreWeapon();

	/** Attaches weapon mesh to the owning actor */
	void AttachComponentMeshToActor( USkeletalMesh* Mesh );

	/** Sets up all weapon effects from provided data */
	void InitializeWeaponProperties( const FInitWeaponProperties& Effects );

	/** 
	* Performs trace to determine bullet impact point.
	* Returns true if trace hit something, false otherwise.
	*/
	bool PerformWorldTrace( FVector& EndTrace, FHitResult& OutHitResult ) const;

	/** Handles playing all weapon effects at appropriate locations */
	void PlayWeaponEffects(
			const FHitResult& HitResult,
			const FVector& EndEffectLocation,
			FName SocketEffectName
			) const;

protected:
	/** Runtime initialization and player reference setup */
	virtual void BeginPlay() override;

public:
	/** Frame update for continuous weapon behavior */
	virtual void TickComponent(
			float DeltaTime,
			ELevelTick TickType,
			FActorComponentTickFunction* ThisTickFunction
			) override;

private:
	/** Visual mesh for the equipped weapon */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	/** Reference to owning player for input handling */
	UPROPERTY()
	TObjectPtr<APlayerCharacter> Player;

	// Cached weapon effect references
	UPROPERTY()
	TObjectPtr<UParticleSystem> MuzzleFlash;

	UPROPERTY()
	TObjectPtr<UParticleSystem> ImpactParticle;

	UPROPERTY()
	TObjectPtr<UParticleSystem> BeamTraceParticle;

	UPROPERTY()
	TObjectPtr<USoundBase> WeaponFireSound;

	/** Socket name for spawning effects */
	UPROPERTY()
	FName EffectSocketName;

	UPROPERTY()
	float BaseDamage;

	UPROPERTY()
	float HeadshotMultiplier;

	UPROPERTY()
	TObjectPtr<UAnimMontage> WeaponFireMontage;

public:
	/** 
	* Provides access to weapon mesh for animations.
	* FORCEINLINE used for performance in animation updates.
	*/
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMeshComponent; }
	FORCEINLINE float GetBaseDamage() const { return BaseDamage; }
	FORCEINLINE float GetHeadshotMultiplier() const { return HeadshotMultiplier; }
};
