// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UT_ItemClass.generated.h"

UCLASS()
class UT_GAME_API AUT_ItemClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUT_ItemClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoCount = 10;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	class UUT_Picked_Item_Component* InventoryItemComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

};
