// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UT_AICharacter.h"
#include "UT_Murdock_AICharacter.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API AUT_Murdock_AICharacter : public AUT_AICharacter
{
	GENERATED_BODY()
public:

	AUT_Murdock_AICharacter();

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		if (const AController* C = GetController())
		{
			if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(C))
			{
				return TeamAgent->GetGenericTeamId();
			}
		}
		return FGenericTeamId(0); // default
	}
};
