// Fill out your copyright notice in the Description page of Project Settings.


#include "MurdockAIController.h"
#include "UT_AICharacter.h"
#include "UT_AIPerceptionComponent.h"
#include "BehaviorTree/BlackBoardComponent.h"



AMurdockAIController::AMurdockAIController()
{
	PerceptronComponent = CreateDefaultSubobject<UUT_AIPerceptionComponent>("UT_PerceptionComponent");
	SetPerceptionComponent(*PerceptronComponent);

    SetTeamId(0);
}

ETeamAttitude::Type AMurdockAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    UE_LOG(LogTemp, Warning, TEXT("GetTeamAttitudeTowards123 ::%s"), *Other.GetName());
    // If the other actor has a team agent interface
    if (const IGenericTeamAgentInterface* TeamAgent = Cast<const IGenericTeamAgentInterface>(&Other))
    {
        FGenericTeamId OtherTeamId = TeamAgent->GetGenericTeamId();
        

        if (OtherTeamId == GetGenericTeamId())
        {
            UE_LOG(LogTemp, Warning, TEXT("GetTeamAttitudeTowards ETeamAttitude::Friendly"));
            return ETeamAttitude::Friendly;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("GetTeamAttitudeTowards ETeamAttitude::Hostile"));

            return ETeamAttitude::Hostile;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("GetTeamAttitudeTowards ETeamAttitude::Neutral"));

    return ETeamAttitude::Neutral;
}

void AMurdockAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    const auto UTCharacter = Cast<AUT_AICharacter>(InPawn);
    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(
        DelayHandle,
        [this, UTCharacter]()
        {
            if (UTCharacter)
            {
                RunBehaviorTree(UTCharacter->BechaviorTreeAsset);
            }
        },
        4.0f,
        false
    );
}


void AMurdockAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto AimActor = GetFocusOnActor();

	SetFocus(AimActor);
}

AActor* AMurdockAIController::GetFocusOnActor() const
{
	if (!GetBlackboardComponent()) return nullptr;
	return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(FocusOnKeyName));
}
