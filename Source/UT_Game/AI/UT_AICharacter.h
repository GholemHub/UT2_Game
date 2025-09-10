// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../UT_GameCharacter.h"

#include "UT_AICharacter.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API AUT_AICharacter : public AUT_GameCharacter
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
    int AIState = 2;
	AUT_AICharacter(const FObjectInitializer& ObjInit);
    AUT_AICharacter() { }

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BechaviorTreeAsset;

	virtual void BeginPlay() override;

};
