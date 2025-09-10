// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UT_Flak_Projectile.h"
#include "UT_Flak.generated.h"

class AUT_GameCharacter;
class UNiagaraSystem;

UCLASS()
class UT_GAME_API AUT_Flak : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUT_Flak();

protected:
	// Lifecycle
	virtual void BeginPlay() override;

public:


	/** Static mesh for the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	class UUT_Picked_Item_Component* InventoryItemComp;

	/** Replicated mesh assignment */
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedMesh)
	UStaticMesh* ReplicatedMesh;

	UFUNCTION()
	void OnRep_ReplicatedMesh();

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Projectile")
	TSubclassOf<AUT_Flak_Projectile> Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Projectile")
	int Ammo = 10;
	UFUNCTION()
	void CanFire();

	/** Niagara muzzle effect */
	UPROPERTY(EditAnywhere, Category = "Weapon|Effects")
	UNiagaraSystem* NiagaraSystem;


	/** Plays muzzle flash effect */
	UFUNCTION()
	void PlayMuzzleFlash();

	/** Adds recoil shake to weapon mesh */
	virtual void MakeWeaponShake();

	/** Resets the fire state and triggers another fire if needed */
	virtual void ResetFire();
	UFUNCTION()
	void TriggerFire();


protected:
	/** Server-side fire logic */
	UFUNCTION(Server, Reliable)
	void ServerFire(FVector_NetQuantizeNormal ShootDirection, FVector_NetQuantize FireOrigin);
	void ServerFire_Implementation(FVector_NetQuantizeNormal ShootDirection, FVector_NetQuantize FireOrigin);

	/** Multicast for all clients to spawn the projectile */
	void Multicast_ActuallyFire(FVector_NetQuantizeNormal ShootDirection, FVector_NetQuantize FireOrigin);

	/** Retrieves the socket location for firing */
	UFUNCTION()
	FVector GetFireOriginFromSocket();

	/** Gets the owning player character */
	AUT_GameCharacter* GetPlayer();

	/** Controls firing rate delay */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FiringSpeed = 0.3f;



	UFUNCTION()
	FVector CalculateShootDirection(const FVector& FireOrigin);

	/** Whether the weapon can currently fire */
	UPROPERTY(ReplicatedUsing = On_bCanFire)
	bool bCanFire = true;

	UPROPERTY(ReplicatedUsing = On_bCanFire)
	bool bHasAmmo = true;

	/** Timer handle for controlling fire rate */
	UPROPERTY(ReplicatedUsing = On_FireRateHandle)
	FTimerHandle FireRateHandle;

	/** Triggered when bCanFire is updated */
	UFUNCTION()
	void On_bCanFire();

	/** Placeholder for any additional fire rate timer behavior */
	UFUNCTION()
	void On_FireRateHandle();

	/** If recoil recovery is pending */
	bool bShouldRecover = false;

	/** Original relative location for shake recovery */
	FVector OriginalRelativeLocation;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
