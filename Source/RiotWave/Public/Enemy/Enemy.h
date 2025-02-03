// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/DamageInterface.h"
#include "Enemy.generated.h"

class AItemBase;
class UBoxComponent;
class USphereComponent;
class AEnemyController;
class UBehaviorTree;

UCLASS()
class RIOTWAVE_API AEnemy : public ACharacter, public IDamageInterface {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

public:
	void Death();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitPatrolPoint();

	
	void InitOverlapEvents();

	UFUNCTION()
	void AgroSphereOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );


	UFUNCTION()
	void AgroSphereOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );


	UFUNCTION()
	void CombatRangeSphereOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );


	UFUNCTION()
	void CombatRangeSphereOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	UFUNCTION(BlueprintCallable) 
	void PlayAttackMontage();

	UFUNCTION()
	void DamageCollisionOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
	void DamageCollisionOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponCollision();

	void DoDamage( AActor* OtherActor );
	
protected:
	UPROPERTY(EditAnywhere, Category = "Enemy Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere, Category = "Enemy Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> AttackSound;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	virtual void BulletHit( FHitResult HitResult ) override;

	virtual float TakeDamage( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser ) override;
private:
	
private:
	UPROPERTY(EditAnywhere, Category = "Enemy Properties|AI", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties|AI", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties|AI", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;
	
	UPROPERTY()
	TObjectPtr<AEnemyController> EnemyController;

	UPROPERTY(VisibleAnywhere, Category = "Enemy Properties|Combat", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> AgroSphere;

	UPROPERTY(VisibleAnywhere, Category = "Enemy Properties|Combat", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> CombatRangeSphere;

	UPROPERTY(VisibleAnywhere, Category = "Enemy Properties|Combat", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsInAttackRange;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties|Combat", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(VisibleAnywhere, Category = "Enemy Properties|Combat", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> DamageCollision;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties|Collectable", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AItemBase> ItemToSpawnOnDeath;

public:
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
