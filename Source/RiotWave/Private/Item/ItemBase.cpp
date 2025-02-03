// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBase.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"


// Sets default values
AItemBase::AItemBase() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultRootScene = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(DefaultRootScene);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(GetRootComponent());

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Collision Sphere");
	CollisionSphere->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay() {
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnOverlapEnd);

	DropItem();	
}

// Called every frame
void AItemBase::Tick( float DeltaTime ) { Super::Tick(DeltaTime); }


/**
 * Called when another actor begins to overlap with this item.
 * If the overlapping actor is a player character, the item is picked up,
 * a sound is played, and the item is destroyed.
 *
 * @param OverlappedComponent The component that was overlapped.
 * @param OtherActor The actor that caused the overlap event.
 * @param OtherComp The specific component of the other actor that caused the overlap.
 * @param OtherBodyIndex The index of the other body.
 * @param bFromSweep Whether the overlap was caused by a sweep.
 * @param SweepResult The result of the sweep, if applicable.
 */
void AItemBase::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) {
	// Check if the overlapping actor is a player character
	if ( Cast<APlayerCharacter>(OtherActor) ) {
		// Call the ItemPicked event
		void ItemPicked();

		// Play the pickup sound at the item's location
		if (PickupSound) {
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}
		// Destroy the item
		Destroy();
	}
}

void AItemBase::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) {}


void AItemBase::DropItem() {
	// Play the drop sound at the item's location
	if (DropSound) {
		UGameplayStatics::PlaySoundAtLocation(this, DropSound, GetActorLocation());
	}

	Mesh->SetSimulatePhysics(true);
	
	FVector RandomImpulseDirection = GetActorLocation() + FVector(FMath::RandRange(-33, 89), FMath::RandRange(-73, 167), FMath::RandRange(12, 258));
	RandomImpulseDirection.Normalize();

	const float RandomStrength = FMath::RandRange(329.0f, 400.8f);

	Mesh->AddImpulse(RandomImpulseDirection * RandomStrength);
}

void AItemBase::NotifyHit( class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit ) {
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	FTimerHandle DropTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DropTimerHandle, this, &AItemBase::DIsablePhysics, 4.0f, false);
}

void AItemBase::DIsablePhysics() {
	Mesh->SetSimulatePhysics(false);
}
