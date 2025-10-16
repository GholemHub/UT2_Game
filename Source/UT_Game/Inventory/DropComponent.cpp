// Fill out your copyright notice in the Description page of Project Settings.


#include "DropComponent.h"
#include "../UT_GameCharacter.h"



UDropComponent::UDropComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UDropComponent::BeginPlay()
{
	Super::BeginPlay();

    auto Player = Cast<AUT_GameCharacter>(GetOwner());
    Player->OnAIStateChangedWithParam.AddDynamic(this, &UDropComponent::DropItem);
	// ...
	
}

void UDropComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDropComponent::DropItem(EUTPlayerState NewState)
{
    float RandomValue = FMath::RandRange(0,2);

    if (RandomValue > 1) return;
    //UE_LOG(LogTemp, Warning, TEXT("SpawnActorOnLevel:DROPITEM"));

    if (!Drop)
    {
       //UE_LOG(LogTemp, Warning, TEXT("SpawnActorOnLevel: ActorClass is null!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
       // UE_LOG(LogTemp, Warning, TEXT("SpawnActorOnLevel: No valid world!"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = GetOwner();

    auto Location = GetOwner()->GetActorLocation();
    auto Rotation = GetOwner()->GetActorRotation();

    auto SpawnedActor = World->SpawnActor<AActor>(
        Drop,
        Location,
        Rotation,
        SpawnParams
    );

    if (SpawnedActor)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Spawned actor: %s at %s"), *SpawnedActor->GetName(), *Location.ToString());
    }
    else
    {
       // UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor!"));
    }
}

