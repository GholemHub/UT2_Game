// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UT_FindEnemtService.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API UUT_FindEnemtService : public UBTService
{
	GENERATED_BODY()
public:

	UUT_FindEnemtService();

protected: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector EnemyActorKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
