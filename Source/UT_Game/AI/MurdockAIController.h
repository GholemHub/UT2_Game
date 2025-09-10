// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"

#include "MurdockAIController.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API AMurdockAIController : public AAIController
{
	GENERATED_BODY()
public:
	AMurdockAIController();

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
	void SetTeamId(int NewId) { TeamID = FGenericTeamId(NewId); }
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName FocusOnKeyName = "EnemyActor";
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class UUT_AIPerceptionComponent* PerceptronComponent;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	AActor* GetFocusOnActor() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	FGenericTeamId TeamID;

};
