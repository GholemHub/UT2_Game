// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/UT_ItemClass.h"
#include "GameFramework/Actor.h"
#include "AmmoClass.generated.h"

UCLASS()
class UT_GAME_API AAmmoClass : public AUT_ItemClass
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoClass();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	int AmmoType = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
