// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_FireService.h"
#include "../UT_GameCharacter.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "AIController.h"


UUT_FireService::UUT_FireService()
{
	NodeName = "Fire";
}

bool UUT_FireService::Reloading()
{
	if (bIsReloading)
	{
		// Already reloading, no firing allowed
		return false;
	}

	if (CurrentFire > AmmoReload)
	{
		FTimerHandle ImpulseTimerHandle;
		bIsReloading = true;

		GetWorld()->GetTimerManager().SetTimer(
			ImpulseTimerHandle,
			this,
			&UUT_FireService::ReloadTimer,
			WaitingTime,
			false
		);
		UE_LOG(LogTemp, Warning, TEXT("Reloading started ###..."));
		return false;
	}
	else {
		CurrentFire++;
		UE_LOG(LogTemp, Warning, TEXT("CurrentFire ### %i"), CurrentFire);
		return true;
	}
}

void UUT_FireService::ReloadTimer()
{
	CurrentFire = 0;
	bIsReloading = false;
	UE_LOG(LogTemp, Warning, TEXT("### Reloading finished, ammo reset"));
}

void UUT_FireService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto BlackBoard = OwnerComp.GetBlackboardComponent();
	const auto Controller = OwnerComp.GetAIOwner();

	auto HasAim = BlackBoard && BlackBoard->GetValueAsObject(EnemyActorKey.SelectedKeyName);
	if (Controller) {
		auto Player = Cast<AUT_GameCharacter>(Controller->GetPawn());
		if (Player)
		{
				const auto WeaponComponent = Player->WeaponComponent;
				if (WeaponComponent) {

					if (Reloading())
					{
						UE_LOG(LogTemp, Warning, TEXT("Fire stop ### %i"), HasAim);
						HasAim ? WeaponComponent->HandleFirePressed() : WeaponComponent->HandleFireReleased();
						WeaponComponent->bIsShoot = false;
					}
					
				}
				
		}
	}
	
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
