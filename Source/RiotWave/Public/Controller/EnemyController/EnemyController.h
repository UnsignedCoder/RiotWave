// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class RIOTWAVE_API AEnemyController : public AAIController {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIBehaviour", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIBehaviour", meta=(AllowPrivateAccess = "true"))	
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

public:
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return BehaviorTreeComponent.Get(); }
};
