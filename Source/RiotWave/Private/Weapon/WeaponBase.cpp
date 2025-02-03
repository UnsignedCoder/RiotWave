// WeaponBase.cpp - Implements the core weapon pickup and initialization logic
//
// This file handles the creation of weapon components and management of weapon pickup behavior.
// It focuses on maintaining a clean separation between weapon existence in the world
// and weapon functionality when equipped by a player.

#include "Weapon/WeaponBase.h"

#include "Components/SphereComponent.h"
#include "Interface/Weapon/WeaponDetectionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/WeaponHandlingComponent.h"

/**
* Sets up the base weapon structure and collision.
* Components are created and configured here to ensure proper editor visualization
* and consistent behavior across all weapon instances.
*/
AWeaponBase::AWeaponBase() {
   // Enable Tick() for potential continuous effects or behavior
   PrimaryActorTick.bCanEverTick = true;

   // Create component hierarchy
   // Scene root provides a clean transform hierarchy base
   DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
   SetRootComponent(DefaultSceneRoot);
   
   // Weapon mesh setup - attached to root for independent weapon positioning
   PickupWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
   PickupWeaponMesh->SetupAttachment(GetRootComponent());
   
   // Collision setup for pickup detection
   WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Weapon Collision"));
   WeaponCollision->SetupAttachment(GetRootComponent());

   // Set collision radius large enough for comfortable pickup
   // but not so large it feels unrealistic
   WeaponCollision->InitSphereRadius(80);
}

/**
* Handles the weapon pickup process when a valid actor overlaps.
* This function orchestrates:
* 1. Interface verification
* 2. Component creation/retrieval
* 3. Mesh attachment
* 4. Effects initialization
* 5. Cleanup of the pickup weapon
*/
void AWeaponBase::OnWeaponCollisionBeginOverlap(
      UPrimitiveComponent* OverlappedComponent,
      AActor* OtherActor,
      UPrimitiveComponent* OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult& SweepResult
      ) {
   // Verify actor can handle weapons through interface
   IWeaponDetectionInterface* WeaponInterface = Cast<IWeaponDetectionInterface>(OtherActor);
   if ( WeaponInterface ) {
      // Get or create the weapon handling component
      UWeaponHandlingComponent* WHComponent = WeaponInterface->GetWeaponHandlingComponent();

      if ( WHComponent ) {
         // Transfer weapon mesh and attach it to the new owner
         WHComponent->AttachComponentMeshToActor(PickupWeaponMesh->GetSkeletalMeshAsset());
         // Allow BP customization of pickup behavior
         OnWeaponPicked(OtherActor);

         // Initialize weapon effects in the handling component
         // Bundled into a struct for cleaner parameter passing
         const FInitWeaponProperties Effects( MuzzleFlash, ImpactParticle, BeamTraceParticle, FireSound, WeaponSocketName, BaseDamage, HeadshotMultiplier, WeaponFIreMontage );
         WHComponent->InitializeWeaponProperties(Effects);

         // Play pickup feedback if sound is set
         if ( PickupSound ) {
            UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
         }

         // Remove pickup actor since weapon is now equipped
         Destroy();
      }
   }
}
            
/**
* Runtime initialization that binds the overlap event.
* Done in BeginPlay rather than constructor because delegate binding
* requires the component to be fully initialized.
*/
void AWeaponBase::BeginPlay() {
   Super::BeginPlay();

   // Bind overlap detection to our pickup handler
   WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnWeaponCollisionBeginOverlap);
}

/**
* Frame update handler. Currently minimal as weapon behavior
* is primarily event-driven. Kept for potential future continuous effects.
*/
void AWeaponBase::Tick(float DeltaTime) {
   Super::Tick(DeltaTime);
}