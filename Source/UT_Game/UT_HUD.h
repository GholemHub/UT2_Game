// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UT_HUD.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API AUT_HUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PlayerWidgetClass;

	UPROPERTY()
	UUserWidget* PlayerWidget;

	void AddText();
};