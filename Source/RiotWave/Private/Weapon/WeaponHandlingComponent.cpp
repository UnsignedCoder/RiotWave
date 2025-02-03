// WeaponHandlingComponent.cpp - Implements weapon functionality when equipped
//
// This file handles the core weapon behavior after pickup, including:
// - Weapon mesh attachment and positioning
// - Firing mechanics and hit detection
// - Visual and audio effect management
// - Integration with player systems

#include "Weapon/WeaponHandlingComponent.h"

#include "Enemy/Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interface/Weapon/WeaponDetectionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/PlayerCharacter.h"
#include "Weapon/DamageInterface.h"

/**
* Sets up default component state.
* Tick enabled to support potential continuous effects or behavior updates.
*/
UWeaponHandlingComponent::UWeaponHandlingComponent():
	BaseDamage(0),
	HeadshotMultiplier(0) { PrimaryComponentTick.bCanEverTick = true; }


/**
* Runtime initialization hook.
* Currently minimal as most initialization happens during weapon pickup.
*/
void UWeaponHandlingComponent::BeginPlay() { Super::BeginPlay(); }


/**
* Handles the attachment of a weapon mesh to the player.
* Creates and configures the weapon mesh component, then attaches it
* to the player's grip point for proper positioning.
*/
void UWeaponHandlingComponent::AttachComponentMeshToActor( USkeletalMesh* Mesh ) {
	// Get and validate player reference
	Player = Cast<APlayerCharacter>(GetOwner());
	if ( !WeaponMeshComponent && Player ) {
		// Create weapon mesh component only when needed
		WeaponMeshComponent = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("Weapon Mesh") );
		if ( WeaponMeshComponent ) {
			// Register and configure the new component
			WeaponMeshComponent->RegisterComponent();
			WeaponMeshComponent->SetSkeletalMesh(Mesh);
			
			// Attach to player's grip point for proper weapon positioning
			WeaponMeshComponent->AttachToComponent( Player->GetPlayerMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GripPoint") );

			// Notify player controller of weapon pickup if it implements the interface
			if ( IWeaponDetectionInterface* WeaponInterface = Cast<IWeaponDetectionInterface>(Player->GetController()) ) { WeaponInterface->OnWeaponPicked(Player); }
		}
	}
}


/**
* Performs line trace from screen center for hit detection.
* Uses screen-to-world conversion to support accurate aiming
* from player's view.
*/
bool UWeaponHandlingComponent::PerformWorldTrace( FVector& EndTrace, FHitResult& OutHitResult ) const {
	// Get viewport center for trace origin
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	const FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector WorldLocation, WorldDirection;

	// Convert screen position to world space for accurate tracing
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, WorldLocation, WorldDirection);

	if ( bScreenToWorld ) {
		// Perform trace with reasonable length
		const FVector StartTrace = WorldLocation;
		EndTrace = StartTrace + ( WorldDirection * 10000 );

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());
		
		GetWorld()->LineTraceSingleByChannel(OutHitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, CollisionParams);

		// Update end location if we hit something
		if ( OutHitResult.bBlockingHit ) {
			EndTrace = OutHitResult.ImpactPoint;
			return true;
		}
	}
	return false;
}


/**
* Stores weapon effect references for later use.
* Called during weapon pickup to transfer effect data from pickup actor
* to this handling component.
*/
void UWeaponHandlingComponent::InitializeWeaponProperties( const FInitWeaponProperties& Effects ) {
	MuzzleFlash = Effects.MuzzleFlash;
	ImpactParticle = Effects.ImpactParticle;
	BeamTraceParticle = Effects.BeamTraceParticle;
	EffectSocketName = Effects.WeaponSocketName;
	WeaponFireSound = Effects.FireSound;
	BaseDamage = Effects.Damage;
	HeadshotMultiplier = Effects.HSMultiplier;
	WeaponFireMontage = Effects.WeaponFire;
}

/**
* Frame update handler.
* Currently minimal as weapon behavior is primarily event-driven.
*/
void UWeaponHandlingComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if ( const USkeletalMeshComponent* Mesh = Player->GetPlayerMesh()) {
		WeaponMeshComponent->SetVisibility(Mesh->IsVisible());
	}
		
}

/**
* Handles weapon firing sequence:
* 1. Traces for potential targets
* 2. Spawns appropriate effects
*/
void UWeaponHandlingComponent::FIreWeapon() {
	FVector TraceEndLocation;
	FHitResult TraceHitResult;

	UGameplayStatics::PlaySoundAtLocation(this, WeaponFireSound, GetOwner()->GetActorLocation());
	
	Player->GetPlayerMesh()->GetAnimInstance()->Montage_Play(WeaponFireMontage);
	
	// Perform hit detection and spawn effects
	PerformWorldTrace(TraceEndLocation, TraceHitResult);

	// Debug message for development feedback
	if (IDamageInterface* DamageInterface = Cast<IDamageInterface>(TraceHitResult.GetActor())) {
		DamageInterface->BulletHit(TraceHitResult);
		PlayWeaponEffects(TraceHitResult, TraceEndLocation, EffectSocketName);

		if (AEnemy* Enemy = Cast<AEnemy>(TraceHitResult.GetActor())) {
			UGameplayStatics::ApplyDamage(Enemy, BaseDamage, Player->GetController(), GetOwner(), UDamageType::StaticClass());
		}
	} else {
		PlayWeaponEffects(TraceHitResult, TraceEndLocation, EffectSocketName);
	}

}


/**
* Spawns all weapon-related effects:
* - Muzzle flash at barrel
* - Impact effect at hit location
* - Beam/trace effect between barrel and target
*/
void UWeaponHandlingComponent::PlayWeaponEffects( const FHitResult& HitResult, const FVector& EndEffectLocation, const FName SocketEffectName ) const {
	if ( WeaponMeshComponent ) {
		// Get effect spawn location from weapon socket
		const FTransform SocketLocationTransform = WeaponMeshComponent->GetSocketByName(SocketEffectName)->GetSocketTransform(WeaponMeshComponent);

		// Spawn muzzle flash if set
		if ( MuzzleFlash ) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketLocationTransform, true); }
		// Spawn impact effect at hit location if we hit something
		if ( ImpactParticle && HitResult.bBlockingHit ) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), true );
		}
		// Spawn beam effect between barrel and target if set
		if ( BeamTraceParticle ) {
			if ( UParticleSystemComponent* BeamTrace = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamTraceParticle, SocketLocationTransform) ) {
				BeamTrace->SetVectorParameter(TEXT("Target"), EndEffectLocation);
			}
		}
	}
}
