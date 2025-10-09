// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_WaveManager.h"
#include "UT_Crunch_AICharacter.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemy_WaveManager::AEnemy_WaveManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy_WaveManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,       // handle
		this,                   // target object
		&AEnemy_WaveManager::MakeWave, // function to call
		1.0f,                   // time in seconds
		true                    // looping (true = repeat)
	);
}

// Called every frame
void AEnemy_WaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy_WaveManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AEnemy_WaveManager::MakeWave()
{
	if (!IsValid(this))
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	if (CurrentWave > MaxWave)
	{
		UE_LOG(LogTemp, Warning, TEXT("All waves completed!"));
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	if (TimerCoundDown <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning Wave %d"), CurrentWave);

		// Spawn N enemies based on current wave
		//for (int32 i = 0; i < CurrentWave; i++)
		{
			SpawnActor();
			SpawnActor();
			SpawnActor();
		}

		// Prepare for next wave
		CurrentWave++;
		TimerCoundDown = 10; // 10 seconds break before next wave
	}
	else
	{
		TimerCoundDown--;
		UE_LOG(LogTemp, Warning, TEXT("Next wave in %d seconds"), TimerCoundDown);
	}
}

void AEnemy_WaveManager::SpawnActor()
{
	if (!Enemy) return;
	UWorld* World = GetWorld();
	if (!World) return;

	FVector Origin = GetActorLocation();
	FVector RandomOffset = UKismetMathLibrary::RandomUnitVector() * 100.f;
	FVector SpawnLocation = Origin + RandomOffset;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	AUT_Crunch_AICharacter* SpawnedEnemy = World->SpawnActor<AUT_Crunch_AICharacter>(
		Enemy,            // The enemy class (must be TSubclassOf<AUT_Crunch_AICharacter>)
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
}

