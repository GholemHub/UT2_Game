// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION()
	void SetHealthText(float Health);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText_1;
};
