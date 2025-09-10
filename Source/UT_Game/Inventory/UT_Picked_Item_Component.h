// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "UT_Picked_Item_Component.generated.h"


UENUM(BlueprintType)
enum class EItemState : uint8
{
	Dropped     UMETA(DisplayName = "Dropped"),
	Hold		UMETA(DisplayName = "Hold"),
	AmmoEquipp		UMETA(DisplayName = "AmmoEquipp"),
	NON			UMETA(DisplayName = "NON"),
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UT_GAME_API UUT_Picked_Item_Component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUT_Picked_Item_Component();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "State")
	EItemState CurrentItemState = EItemState::Dropped;

	UFUNCTION(BlueprintCallable)
	void ChangeItemState(EItemState NewState, AActor* Player);

	UPROPERTY()
	AActor* OwnerPlayer;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY();
	AActor* Owner;
		
};
