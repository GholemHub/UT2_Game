// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "UT_Flak_Projectile.generated.h"

UCLASS()
class UT_GAME_API AUT_Flak_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUT_Flak_Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere, Category = Weapon)
	float FireImpulse = 1000.f;

	virtual void MakeShot(FVector ShootDirection);

	UPROPERTY(EditAnywhere, Category = SOUND)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = SOUND)
	USoundBase* ExplosionSound;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayFireEffects(FVector ShootDirection);

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystemComponent* ParticleSystemComp;
	UFUNCTION()
	void DestroyAfterDelay();

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 5;

	UPROPERTY(EditAnywhere, Category = "Damage")
	bool WeaponIsRadial = true;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ExplodeEffects(FVector Location);
private:
	bool bHasExploded = false;

};
