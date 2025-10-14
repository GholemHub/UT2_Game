// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Inventory/UT_ItemClass.h"
#include "FirstAidKit_Item.generated.h"

UCLASS()
class UT_GAME_API AFirstAidKit_Item : public AUT_ItemClass
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFirstAidKit_Item();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
