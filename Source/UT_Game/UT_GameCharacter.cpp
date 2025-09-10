// Copyright Epic Games, Inc. All Rights Reserved.

#include "UT_GameCharacter.h"
#include "Weapon/AmmoClass.h"
#include "UT_GameProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Weapon/UT_WeaponComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerWidget.h"
#include "UT_HUD.h"
#include "Weapon/AI_UT_MurdockWeapon.h"
#include "Inventory/UT_Picked_Item_Component.h"




DEFINE_LOG_CATEGORY(LogTemplateCharacter);

// AUT_GameCharacter

void AUT_GameCharacter::Respawn()
{
	auto DeadCharacter = this;
	if (IsValid(DeadCharacter))
	{
		DeadCharacter->WeaponComponent->DestroyTheWeapon();
		DeadCharacter->Destroy();
	}

	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(GetController()))
	{
		GM->RestartPlayer(GetController());
		UTPlayerState = EUTPlayerState::Alive;
	}
}

AUT_GameCharacter::AUT_GameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	WeaponComponent = CreateDefaultSubobject<UUT_WeaponComponent>(TEXT("UTWeaponComponent"));

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(GetCapsuleComponent());

	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	SpringArm->SetupAttachment(Mesh1P, "head");

	SpringArm->bUsePawnControlRotation = true;     // Rotate arm based on controller
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.0f;

	FirstPersonCameraComponent->SetupAttachment(SpringArm); // Replace "head" with your actual socket/bone name

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AUT_GameCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AUT_GameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUT_GameCharacter::Move);
		EnhancedInputComponent->BindAction(CrouchStartAction, ETriggerEvent::Started, this, &AUT_GameCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchEndAction, ETriggerEvent::Completed, this, &AUT_GameCharacter::EndCrouch);

		//Weapon
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AUT_GameCharacter::SwitchWeapon);
		EnhancedInputComponent->BindAction(FireStartAction, ETriggerEvent::Started, this, &AUT_GameCharacter::HandleFirePressed);
		EnhancedInputComponent->BindAction(FireStartAction, ETriggerEvent::Completed, this, &AUT_GameCharacter::HandleFireReleased);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUT_GameCharacter::Look);

		//Item interaction
		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &AUT_GameCharacter::DropItem);
		EnhancedInputComponent->BindAction(PickUpItemAction, ETriggerEvent::Started, this, &AUT_GameCharacter::PickUpItem);

		
	}
}

void AUT_GameCharacter::EndCrouch()
{
	UnCrouch();
}

void AUT_GameCharacter::StartCrouch()
{
	if (bEquipped) {
		Crouch();
	}
}


void AUT_GameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AUT_GameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		float Sensitivity = 1.0f; // You can make this adjustable
		AddControllerYawInput(LookAxisVector.X * Sensitivity * GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(LookAxisVector.Y * Sensitivity * GetWorld()->GetDeltaSeconds());
	}
}

void AUT_GameCharacter::BeginPlay()
{

	Super::BeginPlay();
	UTPlayerState = EUTPlayerState::Alive;
	if (!WeaponComponent) return;
}

#include "Perception/AISense_Hearing.h"


void AUT_GameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		float NewPitch = GetBaseAimRotation().Pitch;
		AO_PITCH = NewPitch;

		if (!HasAuthority()) // client only
		{
			Server_UpdateAimOffset(NewPitch);
		}

	}
}

void AUT_GameCharacter::Server_UpdateAimOffset_Implementation(float Pitch)
{
	AO_PITCH = Pitch;
}

void AUT_GameCharacter::HandleFirePressed()
{
	if (this->IsPlayerControlled())
	{
		UE_LOG(LogTemp, Error, TEXT("&&& HandleFirePressed"));

		// Broadcast to listeners
		OnFirePressed.Broadcast();
			}
	else {
		
		WeaponComponent->HandleFirePressed();
	}
}

void AUT_GameCharacter::HandleFireReleased()
{
	// Broadcast to listeners
	OnFireReleased.Broadcast();

	// Optional: Stop firing, stop charging, etc.
}


void AUT_GameCharacter::DropItem()
{
	Server_DropItem();
}

AActor* AUT_GameCharacter::CalculateTraceDirection()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	//APlayerController* PlayerController = ;
	if (!GetInstigatorController()) return nullptr;

	FVector WorldLocation;
	FVector WorldDirection;

	// Get screen size
	int32 ScreenWidth, ScreenHeight;
	PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

	// Deproject the center of the screen
	if (PlayerController->DeprojectScreenPositionToWorld(ScreenWidth / 2, ScreenHeight / 2, WorldLocation, WorldDirection))
	{
		FVector Start = WorldLocation;
		FVector End = Start + (WorldDirection * 1000.0f); // 1000 units trace distance

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // Ignore self

		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

		// Draw Debug Line
		FColor LineColor = bHit ? FColor::Green : FColor::Red;

		if (bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *Hit.GetActor()->GetName());
			return Hit.GetActor();
		}
	}
	return nullptr;
}

void AUT_GameCharacter::PickUpItem()
{
	auto TriceActor = CalculateTraceDirection();
	if (!TriceActor) return;

	Server_PickUpItem(TriceActor);

}


void AUT_GameCharacter::SwitchWeapon()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("SwitchWeapon 1"));

	if (HasAuthority())
	{
		// We are already the server
		Server_SwitchWeapon();
	}
	else
	{
		// Tell server we want to switch
		Server_SwitchWeapon();
	}
}

void AUT_GameCharacter::Multicast_DropItem_Implementation()
{
	if (!WeaponComponent || !WeaponComponent->Weapon) return;

	auto ItemComponent = WeaponComponent->Weapon->FindComponentByClass<UUT_Picked_Item_Component>();
	if (!ItemComponent) return;

	ItemComponent->ChangeItemState(EItemState::Dropped, this);
}

void AUT_GameCharacter::Server_PickUpItem_Implementation(AActor* TargetItem)
{
	Multicast_PickUpItem(TargetItem);
}

void AUT_GameCharacter::Multicast_PickUpItem_Implementation(AActor* TargetItem)
{
	auto Weapon = Cast<AUT_Flak>(TargetItem);
	if (Weapon) {
		if (bEquipped == true) return;

		if (!TargetItem) return;

		auto ItemComponent = TargetItem->FindComponentByClass<UUT_Picked_Item_Component>();
		if (!ItemComponent) return;

		ItemComponent->ChangeItemState(EItemState::Hold, this);
	}

	auto Ammo = Cast<AAmmoClass>(TargetItem);
	if (Ammo) {
		auto ItemComponent = TargetItem->FindComponentByClass<UUT_Picked_Item_Component>();
		if (!ItemComponent) return;

		ItemComponent->ChangeItemState(EItemState::AmmoEquipp, this);
	}
}


void AUT_GameCharacter::Server_DropItem_Implementation()
{
	Multicast_DropItem();
}

float AUT_GameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		return 0.0f;
	}

	float DamageApplied = FMath::Min(Health, DamageAmount);
	Health -= DamageApplied;

	//PlayerWidgetInstance->SetHealthText(Health);
	if (Health <= 0.0f)
	{
		Die();
	}

// How to add here the widget

	
	return DamageApplied;
}

void AUT_GameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		if (WeaponComponent)
		{
			//WeaponComponent->EquipWeapon();
		}
	}
}



void AUT_GameCharacter::Die()
{
	UTPlayerState = EUTPlayerState::Death;

	AController* PlayerController = GetController();
	DropItem();

	// Disable input/movement
	GetCharacterMovement()->DisableMovement();
	DetachFromControllerPendingDestroy();

	Multicast_Ragdoll();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (HasAuthority() && PlayerController)
	{
		FTimerHandle RespawnTimer;
		
		GetWorld()->GetTimerManager().SetTimer(
			RespawnTimer,
			this,
			&AUT_GameCharacter::Respawn,
			5.0f,   // delay in seconds
			false   // false = run once, true = loop
		);

		GetWorldTimerManager().SetTimer(RespawnTimer, FTimerDelegate::CreateLambda([PlayerController, DeadCharacter = this]()
			{
				// Destroy old actor before respawning
				if (IsValid(DeadCharacter))
				{
					DeadCharacter->WeaponComponent->DestroyTheWeapon();
					DeadCharacter->Destroy();  
				}

				if (AGameModeBase* GM = UGameplayStatics::GetGameMode(PlayerController))
				{
					GM->RestartPlayer(PlayerController);
				}
			}), 5.0f, false);
	}
	if (!GetWorld()) return;
	OnAIStateChangedWithParam.Broadcast(UTPlayerState);
}

void AUT_GameCharacter::Multicast_Ragdoll_Implementation()
{
	if (Mesh1P)
	{
		// Stop all animations
		Mesh1P->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		Mesh1P->bPauseAnims = true;

		// Enable ragdoll
		Mesh1P->SetSimulatePhysics(true);
		Mesh1P->SetCollisionProfileName(TEXT("Ragdoll"));
	}
}


void AUT_GameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUT_GameCharacter, AO_PITCH);
	DOREPLIFETIME(AUT_GameCharacter, Health);
	DOREPLIFETIME(AUT_GameCharacter, bEquipped);

}

void AUT_GameCharacter::Server_SwitchWeapon_Implementation()
{
	if (WeaponComponent)
	{
		WeaponComponent->SwitchWeapon();
	}
}

void AUT_GameCharacter::MakeHit()
{
	UE_LOG(LogTemp, Warning, TEXT("*********"));

}

void AUT_GameCharacter::StopHit()
{
	UE_LOG(LogTemp, Warning, TEXT("*********"));
}
