// Fill out your copyright notice in the Description page of Project Settings.

#include "../Weapon/UT_Flak.h"
#include "../Weapon/UT_Flak_Projectile.h"
#include "../UT_GameCharacter.h"
#include "../AI/UT_AICharacter.h"
#include "../Inventory/UT_Picked_Item_Component.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"


// Sets default values
AUT_Flak::AUT_Flak()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetIsReplicated(true);


	InventoryItemComp = CreateDefaultSubobject<UUT_Picked_Item_Component>(TEXT("InventoryItemComponent"));

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts
void AUT_Flak::BeginPlay()
{
	Super::BeginPlay();
}

void AUT_Flak::OnRep_ReplicatedMesh()
{
}

void AUT_Flak::CanFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Ammo::: %i"), Ammo);
	if (Ammo > 0) {
		bHasAmmo = true;
		UE_LOG(LogTemp, Warning, TEXT("::::::::::TRUE"));
	}
	else {
		bHasAmmo = false;
		UE_LOG(LogTemp, Warning, TEXT("::::::::::false"));
	}
}

#pragma region === Fire Logic ===

void AUT_Flak::ServerFire_Implementation(FVector_NetQuantizeNormal ShootDirection, FVector_NetQuantize FireOrigin)
{

	Multicast_ActuallyFire(ShootDirection, FireOrigin);
}

void AUT_Flak::Multicast_ActuallyFire(FVector_NetQuantizeNormal ShootDirection, FVector_NetQuantize FireOrigin)
{
	CanFire();
	
	if (!bHasAmmo) return;
	Ammo--;
	bCanFire = false;

	UE_LOG(LogTemp, Warning, TEXT("Looking for some fire 11"));
	if (!Projectile || !GetWorld()) return;

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = GetInstigator();

	FTransform SpawnTransform(FRotator::ZeroRotator, FireOrigin);
	AUT_Flak_Projectile* Pr = GetWorld()->SpawnActor<AUT_Flak_Projectile>(Projectile, SpawnTransform, Params);
	UE_LOG(LogTemp, Warning, TEXT("Looking for some fire 1"));

	if (Pr)
	{
		Pr->MakeShot(ShootDirection);
		
		PlayMuzzleFlash();
	}

	GetWorldTimerManager().SetTimer(FireRateHandle, this, &AUT_Flak::ResetFire, FiringSpeed, false);
}

void AUT_Flak::ResetFire()
{
	bCanFire = true;

	AUT_GameCharacter* Player = GetPlayer();
	if (!Player || !Player->HasAuthority() || !Player->WeaponComponent) return;

	if (Player->WeaponComponent->bIsShoot && Player->IsLocallyControlled())
	{
		TriggerFire();
		UE_LOG(LogTemp, Warning, TEXT("ResetFire triggered another Fire."));
	}
}

#pragma endregion

#pragma region === Helpers & Effects ===

FVector AUT_Flak::GetFireOriginFromSocket()
{
	if (!MeshComponent || !MeshComponent->DoesSocketExist(TEXT("Hole"))) {
		UE_LOG(LogTemp, Warning, TEXT("Looking for some fire GetFireOriginFromSocket1"));

		return FVector::ZeroVector;
	}
	UE_LOG(LogTemp, Warning, TEXT("Looking for some fire GetFireOriginFromSocket2"));
	return MeshComponent->GetSocketLocation(TEXT("Hole"));
}

void AUT_Flak::PlayMuzzleFlash()
{
	if (!NiagaraSystem) return;

	FVector SocketLocation = MeshComponent->GetSocketLocation(TEXT("Hole"));
	FRotator SocketRotation = MeshComponent->GetSocketRotation(TEXT("Hole"));

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		NiagaraSystem,
		SocketLocation,
		SocketRotation,
		FVector(1.0f),
		true, true
	);
}

void AUT_Flak::MakeWeaponShake()
{

}

AUT_GameCharacter* AUT_Flak::GetPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("Looking for some fire CalculateShootDirection 111"));
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	
	//APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 1);

	if (InventoryItemComp && InventoryItemComp->OwnerPlayer) {
		APlayerController* PC = Cast<APlayerController>(InventoryItemComp->OwnerPlayer->GetInstigatorController());
		if (PC)
		{
			UE_LOG(LogTemp, Warning, TEXT("Looking for some fire CalculateShootDirection 222"));
			OwnerPawn = PC->GetPawn(); // This is the local client's pawn

		}

	}

	if (!OwnerPawn) return nullptr;

	AUT_GameCharacter* Player = Cast<AUT_GameCharacter>(OwnerPawn);
	UE_LOG(LogTemp, Warning, TEXT("Looking for some fire CalculateShootDirection 333"));

	return Player;
}

void AUT_Flak::TriggerFire()
{
	if (!bCanFire) return;

	//UE_LOG(LogTemp, Warning, TEXT("Looking for some fire bCanFire2"));

	const FVector FireOrigin = GetFireOriginFromSocket();
	const FVector ShootDirection = CalculateShootDirection(FireOrigin);

	if (!ShootDirection.IsNearlyZero())
	{
		
		ServerFire(ShootDirection, FireOrigin);
	}
}

FVector AUT_Flak::CalculateShootDirection(const FVector& FireOrigin)
{
	//UE_LOG(LogTemp, Warning, TEXT("Looking for some fire CalculateShootDirection 000"));

	AUT_GameCharacter* Player = GetPlayer();
	if (!Player) return FVector::ZeroVector;
	

	// AI - aim straight ahead
	if (Cast<AUT_AICharacter>(Player))
	{
		return GetActorForwardVector()* -1;
	}

	// Player - trace from center screen to find target
	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Looking for some fire CalculateShootDirection1"));

		int32 ScreenW, ScreenH;
		PC->GetViewportSize(ScreenW, ScreenH);
		FVector2D ScreenCenter(ScreenW * 0.5f, ScreenH * 0.5f);

		FVector WorldOrigin, WorldDir;
		PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldOrigin, WorldDir);

		FHitResult Hit;
		FVector TraceEnd = FireOrigin + WorldDir * 10000.0f;
		FCollisionQueryParams Params;
	
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, FireOrigin, TraceEnd, ECC_Visibility, Params);
		return bHit ? (Hit.ImpactPoint - FireOrigin).GetSafeNormal() : WorldDir;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Looking for some fire CalculateShootDirection2"));
	return FVector::ZeroVector;
}


#pragma endregion

#pragma region === Replication ===

void AUT_Flak::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUT_Flak, bCanFire);
}

void AUT_Flak::On_bCanFire()
{
	//UE_LOG(LogTemp, Warning, TEXT("[%s] On_bCanFire triggered. bCanFire = %d"), *GetName(), bCanFire);

	AUT_GameCharacter* Player = GetPlayer();
	if (Player && Player->WeaponComponent && Player->WeaponComponent->bIsShoot && Player->IsLocallyControlled())
	{
		TriggerFire();
	}
}

void AUT_Flak::On_FireRateHandle()
{
	
}

#pragma endregion
