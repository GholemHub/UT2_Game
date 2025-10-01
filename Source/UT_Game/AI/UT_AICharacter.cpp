// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_AICharacter.h"
#include "Net/UnrealNetwork.h"


AUT_AICharacter::AUT_AICharacter(const FObjectInitializer& ObjInit)
{

}

void AUT_AICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUT_AICharacter, AIState);
}

void AUT_AICharacter::BeginPlay()
{


	Super::BeginPlay();

}
