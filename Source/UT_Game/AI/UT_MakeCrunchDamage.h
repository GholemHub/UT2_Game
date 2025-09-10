// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UT_MakeCrunchDamage.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API UUT_MakeCrunchDamage : public UBTService
{
	GENERATED_BODY()
	

public:
	UUT_MakeCrunchDamage();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector EnemyActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	int AmmoReload = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float WaitingTime = 5.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	//FBlackboardKeySelector Reload;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
