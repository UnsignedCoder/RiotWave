// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class USphereComponent;

UCLASS()
class RIOTWAVE_API AItemBase : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
	void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	UFUNCTION(BlueprintImplementableEvent)
	void ItemPicked();

	void DropItem();

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void DIsablePhysics();
public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<USceneComponent> DefaultRootScene;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(EditAnywhere, Category = "Item", BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<USoundBase> DropSound;
	
	UPROPERTY(EditAnywhere, Category = "Item", BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<USoundBase> PickupSound;

	
};
