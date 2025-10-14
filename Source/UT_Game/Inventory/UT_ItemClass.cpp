// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_ItemClass.h"
#include "../Inventory/UT_Picked_Item_Component.h"

// Sets default values
AUT_ItemClass::AUT_ItemClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetIsReplicated(true);

	InventoryItemComp = CreateDefaultSubobject<UUT_Picked_Item_Component>(TEXT("InventoryItemComponent"));

	bReplicates = true;
	SetReplicateMovement(true);


}

// Called when the game starts or when spawned
void AUT_ItemClass::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUT_ItemClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

