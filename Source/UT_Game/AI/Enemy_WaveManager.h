// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UT_Crunch_AICharacter.h"
#include "../Weapon/AI_UT_MurdockWeapon.h"

#include "Enemy_WaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, int, NewCountdown);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ENEMY)
	TSubclassOf<AUT_AICharacter> Enemy2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ENEMY)
	TSubclassOf<AAI_UT_MurdockWeapon> MurdockWeapon;

	UFUNCTION(BlueprintCallable)
	void MakeWave();
	void SpawnActorCranch();
	void SpawnActorMurdock();

	UPROPERTY(ReplicatedUsing = OnRep_TimerCountdown, EditAnywhere, BlueprintReadWrite, Category = Timer)
	int TimerCoundDown = 10;
	
	int TimerCound = 10;
	UFUNCTION()
	void OnRep_TimerCountdown();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
	int MaxWave = 5;
	FTimerHandle SpawnTimerHandle;
	int CurrentWave = 0;

	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnTimerUpdated OnTimerUpdated;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
