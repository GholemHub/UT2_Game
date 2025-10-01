// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UT_AICharacter.h"

#include "Sound/SoundCue.h"   // if you use Sound Cue
#include "Sound/SoundBase.h"
#include "UT_Crunch_AICharacter.generated.h"

/**
 * 
 */
UCLASS()
class UT_GAME_API AUT_Crunch_AICharacter : public AUT_AICharacter
{
	GENERATED_BODY()
public:
	AUT_Crunch_AICharacter();

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		if (const AController* C = GetController())
		{
			if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(C))
			{
				return TeamAgent->GetGenericTeamId();
			}
		}
		return FGenericTeamId(0); // default
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* DanageZoneComponentSphere;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime);

	virtual void MakeHit() override;
	virtual void StopHit() override;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite)
	bool bIsAttacking = false;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bCanAtack = false;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

protected:
	void ApplyRadialImpulse();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* AttackSound;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TypeOfAttack = 0;


private:


	UFUNCTION(BlueprintCallable)
	void MakeHitAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void PlaySpund(USoundBase* Sound);
};
