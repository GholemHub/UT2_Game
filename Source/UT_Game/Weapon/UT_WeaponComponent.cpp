#include "../Weapon/UT_WeaponComponent.h"
#include "../Weapon/UT_Flak.h"
#include "../UT_GameCharacter.h"
#include "../AI/UT_AICharacter.h"

#include "Engine/Engine.h"
#include "AIController.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"

// Constructor
UUT_WeaponComponent::UUT_WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UUT_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	auto AIOwnerCharacter = Cast<AUT_AICharacter>(GetOwner());
	if (!OwnerCharacter) return;
	if (AIOwnerCharacter) {
		if (AIOwnerCharacter->HasAuthority())
		{
			FActorSpawnParameters SpawnParams;

			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SpawnLocation = FVector(0.f, 0.f, 100.f);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			auto SpawnedActor = GetWorld()->SpawnActor<AUT_Flak>(
				AIOwnerCharacter->WeaponComponent->WeaponClasses,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

			if (SpawnedActor) {
				SpawnedActor->SetActorEnableCollision(false);
				SpawnedActor->MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				EquipNewWeapon(SpawnedActor);
			}
		}
	}

	if (AUT_GameCharacter* Player = Cast<AUT_GameCharacter>(GetOwner()))
	{
		Player->OnFirePressed.AddDynamic(this, &UUT_WeaponComponent::HandleFirePressed);
		Player->OnFireReleased.AddDynamic(this, &UUT_WeaponComponent::HandleFireReleased);
	}
}

// === EQUIPPING LOGIC ===

void UUT_WeaponComponent::EquipWeapon()
{
	
	UE_LOG(LogTemp, Warning, TEXT("%s :: EquipWeapon() 2"), *GetOwner()->GetName());
	if (Weapon && IsValid(Weapon))
	{
		if (Weapon && Weapon->MeshComponent)
		{
			Weapon->MeshComponent->SetVisibility(true);
		}
	}

	AUT_GameCharacter* Player = Cast<AUT_GameCharacter>(GetOwner());
	if (!Player) return;
	if (Weapon){
		Weapon->SetOwner(GetOwner());
		UE_LOG(LogTemp, Warning, TEXT("%s :: EquipWeapon() 3 :: %s"), *GetOwner()->GetName(), *Weapon->GetName());
	}
	
	if (Player->HasAuthority())
	{
		SpawnAndAttachWeapon(Player);
		OnRep_Weapon();
	}
}
#include "../Inventory/UT_Picked_Item_Component.h"

void UUT_WeaponComponent::EquipNewWeapon(AUT_Flak* NewWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("%s :: EquipNewWeapon(:: %s) 1 "), *GetOwner()->GetName(), *NewWeapon->GetName());

	if (!GetOwner()) return;
	AUT_GameCharacter* Player = Cast<AUT_GameCharacter>(GetOwner());
	if (!Player) return;

	if (Player->HasAuthority())
	{
		SpawnAndAttachNewWeapon(NewWeapon);
		OnRep_Weapon();
	}
}

void UUT_WeaponComponent::Server_UnEquipWeapon_Implementation()
{
	UnEquipWeapon();
}

void UUT_WeaponComponent::UnEquipWeapon()
{
	if (!Weapon) return;

	UStaticMeshComponent* MeshComp = Weapon->MeshComponent;
	if (!MeshComp) return;

	MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Weapon->SetActorEnableCollision(true);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);

	// Reset replicated reference
	Weapon = nullptr;
}

void UUT_WeaponComponent::SpawnAndAttachNewWeapon(AUT_Flak* NewWeapon)
{
	AUT_GameCharacter* Player = Cast<AUT_GameCharacter>(GetOwner());
	if (!Player || !NewWeapon) return;

	WeaponClasses = NewWeapon->GetClass();

	const FName SocketName = TEXT("hand_rSocket");
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	NewWeapon->AttachToComponent(Player->Mesh1P, AttachmentRules, SocketName);

	Weapon = NewWeapon;

	Weapon->SetActorEnableCollision(false);

	Weapon->MeshComponent->SetSimulatePhysics(false);
	Weapon->MeshComponent->SetEnableGravity(false);
	EquipWeapon();  // Spawns and attaches the new one
}

void UUT_WeaponComponent::SpawnAndAttachWeapon(ACharacter* Owner)
{
	UE_LOG(LogTemp, Warning, TEXT("%s :: SpawnAndAttachWeapon() 1 :: "), *GetOwner()->GetName());

	AUT_GameCharacter* Player = Cast<AUT_GameCharacter>(Owner);
	if (!Player || !Player->Mesh1P) return;

	const FName SocketName = TEXT("hand_rSocket");

	if (Weapon && Weapon->MeshComponent)
	{
		Weapon->MeshComponent->SetVisibility(true);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s :: SpawnAndAttachWeapon() 2 ::"), *GetOwner()->GetName());

	if (Weapon)
	{
		Weapon->SetReplicates(true);
		Weapon->SetReplicateMovement(true);

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		Weapon->AttachToComponent(Player->Mesh1P, AttachmentRules, SocketName);

		Weapon->SetActorEnableCollision(false);

		Weapon->MeshComponent->SetSimulatePhysics(false);
		Weapon->MeshComponent->SetEnableGravity(false);

		if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
		{
			OnRep_Weapon(); // force reattachment if not auto-triggered
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%s :: SpawnAndAttachWeapon() 3 "), *GetOwner()->GetName());

}
// === REPLICATION ===

void UUT_WeaponComponent::OnRep_Weapon()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter && Weapon)
	{
		AttachWeaponToSocket(OwnerCharacter);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Weapon replicated on client"));
	}
}

void UUT_WeaponComponent::OnRep_CurrentWeaponIndex()
{
	/*if (GetOwner()->HasAuthority())
	{
		if (Weapon && Weapon->MeshComponent)
		{
			Weapon->MeshComponent->SetVisibility(true);
		}
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter && GetOwner()->HasAuthority())
	{
		EquipWeapon();
	}*/

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("SwitchWeapon 3"));
}

void UUT_WeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UUT_WeaponComponent, CurrentWeaponIndex);
	DOREPLIFETIME(UUT_WeaponComponent, Weapon);
	DOREPLIFETIME(UUT_WeaponComponent, bIsShoot);
}

// === WEAPON SWITCHING ===

void UUT_WeaponComponent::SwitchWeapon()
{
	//if (GetOwnerRole() < ROLE_Authority)
	//{
	//	Server_SwitchWeapon();
	//}
	//else
	//{
	//	if (CurrentWeaponIndex + 1 <= WeaponClasses.Num()) {
	//		CurrentWeaponIndex = (CurrentWeaponIndex + 1) % WeaponClasses.Num();
	//			UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponIndex ### %i"),CurrentWeaponIndex);

	//		EquipWeapon();
	//	}
	//	else {
	//		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Error with SwitchWeapon"));
	//	}
	//}
}

void UUT_WeaponComponent::Server_SwitchWeapon_Implementation()
{
	//CurrentWeaponIndex = (CurrentWeaponIndex + 1) % WeaponClasses.Num();
	//EquipWeapon();

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("SwitchWeapon 2"));
}

// === FIRING ===

void UUT_WeaponComponent::HandleFirePressed()
{
	UE_LOG(LogTemp, Error, TEXT("&&& HandleFirePressed2 WC"));

	UE_LOG(LogTemp, Warning, TEXT("Looking for some fire 000"));

	if (!Weapon) return;

	// Fire weapon from its socket origin (handles AI or player internally)
	Weapon->TriggerFire();
	UE_LOG(LogTemp, Warning, TEXT("Looking for some fire 2"));
	bIsShoot = true;
}


void UUT_WeaponComponent::HandleFireReleased()
{
	//UE_LOG(LogTemp, Warning, TEXT("Fire stop ### "));

	bIsShoot = false;
}

void UUT_WeaponComponent::Server_SetShooting_Implementation(bool bShooting)
{
	bIsShoot = bShooting;

	if (bShooting && Weapon)
	{
		// GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UUT_WeaponComponent::FireWeapon, true, 0.0f);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

// === HELPERS ===

void UUT_WeaponComponent::AttachWeaponToSocket(ACharacter* Owner)
{
	if (!Weapon) return;

	AUT_GameCharacter* Player = Cast<AUT_GameCharacter>(Owner);
	if (!Player || !Player->Mesh1P) return;

	const FName SocketName = TEXT("hand_rSocket");

	if (!Player->Mesh1P->DoesSocketExist(SocketName)) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	Weapon->AttachToComponent(Player->Mesh1P, AttachmentRules, SocketName);
}

void UUT_WeaponComponent::DestroyTheWeapon()
{
	if (Weapon)
	{
		//Weapon->Destroy();
	}
}

// === TICK ===

void UUT_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Optionally add per-frame logic here
}
