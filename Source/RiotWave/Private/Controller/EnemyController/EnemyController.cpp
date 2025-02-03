// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/EnemyController/EnemyController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Enemy.h"


// Sets default values
AEnemyController::AEnemyController() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

// Called when the game starts or when spawned
void AEnemyController::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyController::Tick( float DeltaTime ) { Super::Tick(DeltaTime); }


void AEnemyController::OnPossess( APawn* InPawn ) {
	Super::OnPossess(InPawn);
	if (!InPawn) {
		return;
	}

	if (AEnemy* Enemy = Cast<AEnemy>( InPawn )) {
		if (Enemy->GetBehaviorTree()) {
			BlackboardComponent->InitializeBlackboard(*Enemy->GetBehaviorTree()->BlackboardAsset);
			
		}
	}
}

