// PlayerCharacter.cpp - Implements the core player character functionality for RiotWave
// 
// This file handles the initialization and setup of the player character's component hierarchy
// and establishes the relationship between camera, movement, and weapon systems.

#include "Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/WeaponHandlingComponent.h"

/**
 * Constructor handles component initialization and hierarchy setup.
 * Components are created here instead of BeginPlay() to ensure they exist
 * before Blueprint construction scripts run and to support editor preview.
 */
APlayerCharacter::APlayerCharacter() {
	// Enable Tick() for continuous updates. This is necessary for smooth
	// camera movement and potential animation blending.
	PrimaryActorTick.bCanEverTick = true;

	// Spring arm setup provides smooth camera behavior and collision handling.
	// Attached to root to ensure it moves with the character's collision capsule.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(GetRootComponent());
	// Allow controller rotation to drive spring arm for responsive camera movement
	SpringArm->bUsePawnControlRotation = true;

	// Camera setup for first-person perspective
	// Attached to spring arm to benefit from its smooth motion and collision handling
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	FirstPersonCamera->SetupAttachment(SpringArm);
	// Disable additional camera rotation as spring arm handles this
	FirstPersonCamera->bUsePawnControlRotation = false;

	// Disable character rotation from controller input
	// This creates a more natural FPS feel where the character's body
	// doesn't instantly snap to match camera rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Player mesh setup for first-person view
	// Attached to camera to ensure arms/weapon always align with player view
	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Player Mesh");
	PlayerMesh->SetupAttachment(FirstPersonCamera);
}

/**
 * Lazy initialization of the WeaponHandlingComponent.
 * This approach:
 * 1. Saves memory by only creating the component when weapons are actually used
 * 2. Allows for runtime creation and registration of the component
 * 3. Maintains a single instance through subsequent calls
 * 
 * @return Pointer to the WeaponHandlingComponent, creating it if necessary
 */
UWeaponHandlingComponent* APlayerCharacter::GetWeaponHandlingComponent() {
	if ( !WeaponHandlingComponent ) {
		// Create and register component only when first requested
		WeaponHandlingComponent = NewObject<UWeaponHandlingComponent>(this, UWeaponHandlingComponent::StaticClass(), TEXT("Weapon Handling Component"));
		WeaponHandlingComponent->RegisterComponent();
		return WeaponHandlingComponent;
	}
	return WeaponHandlingComponent;
}

/**
 * Called when gameplay begins. Currently minimal as most initialization
 * is handled in constructor for editor preview support.
 * Kept for potential future runtime-only initialization needs.
 */
void APlayerCharacter::BeginPlay() { Super::BeginPlay(); }

/**
 * Frame update function. Currently minimal as component Tick functions
 * handle most continuous behavior. Kept for potential future 
 * character-specific update needs.
 */
void APlayerCharacter::Tick( float DeltaTime ) { Super::Tick(DeltaTime); }
void APlayerCharacter::Death() {
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		PlayerController->DisableInput(PlayerController);
	} GetPlayerMesh()->SetVisibility(false);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
}

float APlayerCharacter::TakeDamage( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser ) {
	if (Health - DamageAmount <= 0) {
		Health = 0;
		Death();
		//Destroy();
	} else {
		Health -= DamageAmount;

		if (ImpactSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
		}
	} return DamageAmount;
}
