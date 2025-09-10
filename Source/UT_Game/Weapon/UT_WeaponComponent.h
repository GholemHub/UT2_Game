// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UT_WeaponComponent.generated.h"

class AUT_Flak;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UT_GAME_API UUT_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UUT_WeaponComponent();

	// Tick
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// === Weapon Handling ===

	/** Equip weapon from weapon classes list */
	void EquipWeapon();
	void EquipNewWeapon(AUT_Flak* NewWeapon);
	UFUNCTION(Server, Reliable)
	void Server_UnEquipWeapon();
	void UnEquipWeapon();

	/** Switch to next weapon */
	UFUNCTION(BlueprintCallable)
	void SwitchWeapon();

	/** Destroy the currently equipped weapon */
	void DestroyTheWeapon();

	/** Called when user begins firing */
	UFUNCTION()
	void HandleFirePressed();



	/** Called when user releases fire input */
	UFUNCTION()
	void HandleFireReleased();
	
	/** True if player is holding fire */

	UPROPERTY(Replicated)
	bool bIsShoot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AUT_Flak> WeaponClasses;

	/** Currently equipped weapon instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Weapon, Category = "Weapon")
	AUT_Flak* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<AUT_Flak*> SpawnedWeapons;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Spawns and attaches the weapon to the owner's mesh */
	UFUNCTION()
	void SpawnAndAttachWeapon(ACharacter* Owner);
	UFUNCTION()
	void SpawnAndAttachNewWeapon(AUT_Flak* NewWeapon);
	/** Attaches the weapon to character socket */
	UFUNCTION()
	void AttachWeaponToSocket(ACharacter* Owner);

	// === Replication Hooks ===

	/** Called when Weapon is replicated */
	UFUNCTION()
	void OnRep_Weapon();

	/** Called when weapon index is replicated */
	UFUNCTION()
	void OnRep_CurrentWeaponIndex();

	// === Server RPCs ===

	UFUNCTION(Server, Reliable)
	void Server_SwitchWeapon();

	UFUNCTION(Server, Reliable)
	void Server_SetShooting(bool bShooting);

	// === Properties ===

	/** All available weapon classes for this character */


	/** Currently selected weapon index */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponIndex)
	int32 CurrentWeaponIndex = 0;



	/** Timer used for automatic fire logic */
	FTimerHandle FireTimerHandle;
};
