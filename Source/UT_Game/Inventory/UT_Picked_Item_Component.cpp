// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_Picked_Item_Component.h"
#include "../Weapon/UT_Flak.h"
#include "Blueprint/UserWidget.h"
#include "../UT_GameCharacter.h"

// Sets default values for this component's properties
UUT_Picked_Item_Component::UUT_Picked_Item_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UUT_Picked_Item_Component::BeginPlay()
{ 
	Owner = GetOwner();
}


// Called every frame
void UUT_Picked_Item_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#include "../Weapon/AmmoClass.h"


void UUT_Picked_Item_Component::ChangeItemState(EItemState NewState, AActor* Player)
{
	CurrentItemState = NewState;
	OwnerPlayer = Player;
	if (!Owner) return;
	if (NewState == EItemState::AmmoEquipp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemState is AmmoEquipp"));
		auto Item = Cast<AAmmoClass>(GetOwner());
		if (Item)
		{
			APlayerController* PC = Cast<APlayerController>(Player->GetInstigatorController());
			if (PC)
			{
				AUT_GameCharacter* OwningPawn = Cast<AUT_GameCharacter>(PC->GetPawn()); // This is the local client's pawn

				if (OwningPawn) {
					if (!OwningPawn->WeaponComponent->Weapon) return;
					OwningPawn->WeaponComponent->Weapon->Ammo += Item->AmmoCount;
					Item->Destroy();
				}
			}	
		}
	}
	if (NewState == EItemState::Hold)
	{
		UStaticMeshComponent* MeshComp = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
		UE_LOG(LogTemp, Warning, TEXT("ItemState is Hold"));

		auto Item = Cast<AUT_Flak>(GetOwner());
		if (Item) 
		{
			APlayerController* PC = Cast<APlayerController>(Player->GetInstigatorController());
			if (PC)
			{
				AUT_GameCharacter* OwningPawn = Cast<AUT_GameCharacter>(PC->GetPawn()); // This is the local client's pawn
			
				if (OwningPawn) {
					OwningPawn->WeaponComponent->EquipNewWeapon(Item);
					OwningPawn->bEquipped = true;
					UE_LOG(LogTemp, Warning, TEXT("WeaponbEquipped"));
					
				}
			}
		}
	}
	if (NewState == EItemState::Dropped)
	{
		AUT_GameCharacter* OwningPawn;

		APlayerController* PC = Cast<APlayerController>(Player->GetInstigatorController());
		if (PC){
			//UE_LOG(LogTemp, Warning, TEXT("Looking for some fire CalculateShootDirection 222"));
			OwningPawn = Cast<AUT_GameCharacter>(PC->GetPawn()); // This is the local client's pawn

			if (OwningPawn) {
				UE_LOG(LogTemp, Warning, TEXT("ItemState is Dropped"));
				OwningPawn->WeaponComponent->Server_UnEquipWeapon();
				OwningPawn->bEquipped = false;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("ItemState is Dropped111"));
	}
}

void UUT_Picked_Item_Component::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

