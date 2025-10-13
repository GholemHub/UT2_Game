// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_AICharacter.h"
#include "Net/UnrealNetwork.h"



AUT_AICharacter::AUT_AICharacter(const FObjectInitializer& ObjInit)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AUT_AICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			GodModeTimer,       // handle
			this,                   // target object
			&AUT_AICharacter::GodModeOff, // function to call
			GodLikeTimer,                   // time in seconds
			true                    // looping (true = repeat)
		);
	}
}

float AUT_AICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bGodMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("*** HITTED with no damage ***"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("*** HITTED with damage ***"));
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}

	return 0.0f;
}

void AUT_AICharacter::GodModeOff()
{
	bGodMode = false;
	GetWorldTimerManager().ClearTimer(GodModeTimer);
}

void AUT_AICharacter::GodModeOn()
{

}

void AUT_AICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUT_AICharacter, AIState);
}


void AUT_AICharacter::On_AIStateReplicate()
{
	OnStateChanged.Broadcast(AIState);

}
