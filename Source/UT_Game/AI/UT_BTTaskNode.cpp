// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"



UUT_BTTaskNode::UUT_BTTaskNode()
{
	NodeName = "NextLocation";
}

EBTNodeResult::Type UUT_BTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	const auto BlackBoard = OwnerComp.GetBlackboardComponent();

	if (!BlackBoard || !Controller) return EBTNodeResult::Failed;
	const auto Pawn = Controller->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	const auto NavSys = UNavigationSystemV1::GetCurrent(Pawn);
	if (!NavSys || !NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate)) return EBTNodeResult::Failed;

	if (!NavSys) return EBTNodeResult::Failed;
	FNavLocation NavLocation;
	auto Location = Pawn->GetActorLocation();
	if (!SelfCenter) {
		auto CenterActor = Cast<AActor>(BlackBoard->GetValueAsObject(CenterKey.SelectedKeyName));
		if(!CenterActor) return EBTNodeResult::Failed;
		Location = CenterActor->GetActorLocation();
	}
	const auto Found = NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), Radius, NavLocation);
	if(!Found) return EBTNodeResult::Failed;

	BlackBoard->SetValueAsVector(AimLocationKey.SelectedKeyName, NavLocation.Location);

	return EBTNodeResult::Succeeded;
}
