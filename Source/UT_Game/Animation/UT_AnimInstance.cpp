// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_AnimInstance.h"
#include "../UT_GameCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"



void UUT_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwnerPawn = TryGetPawnOwner();
    if (!OwnerPawn) return;

    // Cast the pawn to your custom character class
    AUT_GameCharacter* Character = Cast<AUT_GameCharacter>(OwnerPawn);
    if (!Character) return;

    AO_PITCH = Character->GetAO_Pitch();
}
