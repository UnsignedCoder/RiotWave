// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Controller/EnemyController/EnemyController.h"
#include "Item/ItemBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerCharacter.h"


// Sets default values
AEnemy::AEnemy() :
	MaxHealth(500), Health(MaxHealth) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(RootComponent);
	AgroSphere->InitSphereRadius(300);

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(RootComponent);
	CombatRangeSphere->InitSphereRadius(250);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollision"));
	DamageCollision->SetupAttachment(GetMesh(), FName("Weapon Socket"));
}


// Called when the game starts or when spawned
void AEnemy::BeginPlay() {
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore );

	InitPatrolPoint();

	InitOverlapEvents();

	DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
}


void AEnemy::InitPatrolPoint() {
	EnemyController = Cast<AEnemyController>(GetController());
	
	const FVector WorldSpacePatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);

	const FVector WorldSpacePatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	if (EnemyController) {
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldSpacePatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldSpacePatrolPoint2);

		EnemyController->RunBehaviorTree(BehaviorTree);
	}
}


// Called every frame
void AEnemy::Tick( float DeltaTime ) { Super::Tick(DeltaTime); }


void AEnemy::BulletHit( const FHitResult HitResult ) {	
	if ( ImpactSound ) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, HitResult.ImpactPoint);
	}
	
	if ( ImpactParticle ) {
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), true);
	}
	
}


float AEnemy::TakeDamage( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser ) {
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health - DamageAmount <= 0) {
		Health = 0;

		Death();
	} else {
		Health -= DamageAmount;
	} return DamageAmount;
}


void AEnemy::Death() {
	if (ItemToSpawnOnDeath) {
		const FVector SpawnLocation = GetActorLocation();
		const FVector SpawnScale = FVector(1.0f, 1.0f, 1.0f);
		const FRotator SpawnRotation = GetActorRotation();

		const FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation, SpawnScale);

		GetWorld()->SpawnActor<AItemBase>(ItemToSpawnOnDeath, SpawnTransform);
	}

	Destroy();
}

void AEnemy::InitOverlapEvents() {
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeSphereOnOverlapBegin);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeSphereOnOverlapEnd);

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::DamageCollisionOnOverlapBegin);
	DamageCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::DamageCollisionOnOverlapEnd);
}


void AEnemy::AgroSphereOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) {
	if (!OtherActor) { return; }
	auto* Character = Cast<APlayerCharacter>(OtherActor);
	if (!Character) { return; }
	EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
}

void AEnemy::AgroSphereOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) {}


void AEnemy::CombatRangeSphereOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) {
	if (!OtherActor) { return; }
	auto* Character = Cast<APlayerCharacter>(OtherActor);
	if (!Character) { return; }
	EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsInCombatRange"), true);
	bIsInAttackRange = true;
}


void AEnemy::CombatRangeSphereOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) {
	if(!OtherActor) { return; }
	auto* Character = Cast<APlayerCharacter>(OtherActor);
	if (Character) {
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsInCombatRange"), false);
		bIsInAttackRange = false;
	}
	
}

void AEnemy::PlayAttackMontage() {
	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage, 1.0f);
}

void AEnemy::DamageCollisionOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) {
	DoDamage(OtherActor);
}

void AEnemy::DamageCollisionOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) {
	
}

void AEnemy::ActivateWeaponCollision() {
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void AEnemy::DeactivateWeaponCollision() {
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DoDamage( AActor* OtherActor ) {
	if (!OtherActor) { return; }
	auto* Character = Cast<APlayerCharacter>(OtherActor);
	if (!Character) { return; }
	UGameplayStatics::ApplyDamage(Character, 1500, EnemyController, this, UDamageType::StaticClass());
	if (AttackSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
	}
}
