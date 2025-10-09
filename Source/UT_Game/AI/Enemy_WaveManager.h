// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UT_Crunch_AICharacter.h"

#include "Enemy_WaveManager.generated.h"

UCLASS()
class UT_GAME_API AEnemy_WaveManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy_WaveManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ENEMY)
	TSubclassOf<AUT_Crunch_AICharacter> Enemy;

	UFUNCTION(BlueprintCallable)
	void MakeWave();
	void SpawnActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
	int TimerCoundDown = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
	int MaxWave = 5;
	FTimerHandle SpawnTimerHandle;
	int CurrentWave = 2;
};
