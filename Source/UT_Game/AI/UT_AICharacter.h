// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../UT_GameCharacter.h"

#include "UT_AICharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, int, NewState);

/**
 * 
 */
UCLASS()
class UT_GAME_API AUT_AICharacter : public AUT_GameCharacter
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, Category = "State")
    FOnStateChanged OnStateChanged;
    UFUNCTION()
    void On_AIStateReplicate();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = On_AIStateReplicate,  Category = State)
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

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BechaviorTreeAsset;

	virtual void BeginPlay() override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
    int GodLikeTimer = 5;
private:

  
    FTimerHandle GodModeTimer;

    UFUNCTION()
    void GodModeOff();

    UFUNCTION()
    void GodModeOn();

    bool bGodMode = true;
    

};
