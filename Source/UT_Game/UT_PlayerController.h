// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"


#include "UT_PlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnAssigned, APawn*, NewPawn);

/**
 * 
 */
UCLASS()
class UT_GAME_API AUT_PlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	
	virtual void OnPossess(APawn* InPawn) override;     // Server only
	virtual void OnRep_Pawn() override;                 // Client only

	// UI reference
	UPROPERTY()
	UUserWidget* MainHUDInstance;

	// Widget class to assign in Blueprint
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainHUDClass;

	// Function to spawn UI
	void CreateMainHUD();

public:
	UPROPERTY(BlueprintAssignable)
	FOnPawnAssigned OnPawnAssigned;


};
