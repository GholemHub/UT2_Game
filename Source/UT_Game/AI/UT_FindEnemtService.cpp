// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_FindEnemtService.h"
#include "Perception/AIPerceptionComponent.h"
#include "UT_AIPerceptionComponent.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "AIController.h"


UUT_FindEnemtService::UUT_FindEnemtService()
{
	NodeName = "FindEnemy";
}

void UUT_FindEnemtService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard)
	{
		const auto Controller = OwnerComp.GetAIOwner();
		UUT_AIPerceptionComponent* PerceptionComp = Controller->FindComponentByClass<UUT_AIPerceptionComponent>();
		if (PerceptionComp) {
			UE_LOG(LogTemp, Warning, TEXT("Perceprion Component is created and we are looking for some enemy"));
			BlackBoard->SetValueAsObject(EnemyActorKey.SelectedKeyName, PerceptionComp->GetClosestEnemy());
		}
	}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

