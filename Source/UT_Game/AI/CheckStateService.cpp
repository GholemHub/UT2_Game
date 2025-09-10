// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckStateService.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "AIController.h"
#include "UT_AICharacter.h"

UCheckStateService::UCheckStateService()
{
	NodeName = "CheckState";
}

void UCheckStateService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!GetWorld()) return;
	const auto BlackBoard = OwnerComp.GetBlackboardComponent();

	if (BlackBoard)
	{
		auto* ControlledPawn = Cast<AUT_AICharacter>(OwnerComp.GetAIOwner()->GetPawn());
		if (!ControlledPawn) return;
			
			UE_LOG(LogTemp, Warning, TEXT(" @@@@ UCheckStateService3 %i"), ControlledPawn->AIState);
				BlackBoard->SetValueAsInt(StateKey.SelectedKeyName, ControlledPawn->AIState);
			
			
	}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
