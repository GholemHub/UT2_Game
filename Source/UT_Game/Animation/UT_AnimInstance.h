// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UT_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API UUT_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float AO_PITCH;
};
