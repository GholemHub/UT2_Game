// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_MakeCrunchDamage.h"
#include "UT_Crunch_AICharacter.h"

#include "../UT_GameCharacter.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "AIController.h"


UUT_MakeCrunchDamage::UUT_MakeCrunchDamage()
{
	NodeName = "Cranch Hit";
}

void UUT_MakeCrunchDamage::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto BlackBoard = OwnerComp.GetBlackboardComponent();
	const auto Controller = OwnerComp.GetAIOwner();

	auto HasAim = BlackBoard && BlackBoard->GetValueAsObject(EnemyActorKey.SelectedKeyName);
	if (Controller) {
		auto Player = Cast<AUT_Crunch_AICharacter>(Controller->GetPawn());
		if (Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fire *** %f"), DeltaSeconds);
			/*if (Player->bCanAtack == true) {
				Player->bIsAttacking = true;
			}*/
			HasAim ? Player->MakeHit() : Player->StopHit();
		}
	}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
