// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_AIPerceptionComponent.h"
#include "MurdockAIController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "../UT_GameCharacter.h"



AActor* UUT_AIPerceptionComponent::GetClosestEnemy() const
{
    TArray<AActor*> PercieveActors;
    GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PercieveActors);
    TArray<AActor*> HearingActors;
    GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), HearingActors);
    PercieveActors.Append(HearingActors);

    if (PercieveActors.Num() == 0)
        return nullptr;

    const auto Controller = Cast<AAIController>(GetOwner());
    if (!Controller)
        return nullptr;
    const auto Pawn = Controller->GetPawn();
    if (!Pawn)
        return nullptr;
    float BestDistance = MAX_FLT;
    AActor* BestPawn = nullptr;


    UE_LOG(LogTemp, Warning, TEXT("Perceprion Component is created and we are looking GetClosestEnemy GOOD"));

    for (const auto PercieveActor : PercieveActors)
    {
        auto Player = Cast<AUT_GameCharacter>(PercieveActor);
        if (!Player || Player->UTPlayerState == EUTPlayerState::Death) return nullptr;

        const auto PercievePawn = Cast<APawn>(PercieveActor);
       
            const auto CurrentDistance = (PercieveActor->GetActorLocation() - Pawn->GetActorLocation()).Size();
            if (CurrentDistance < BestDistance)
            {
                BestDistance = CurrentDistance;
                BestPawn = PercieveActor;
            }
    }

    return BestPawn;
}