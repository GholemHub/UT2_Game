// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Weapon/UT_WeaponComponent.h"
#include "Logging/LogMacros.h"
#include "PlayerWidget.h"
#include "GenericTeamAgentInterface.h"

#include "UT_GameCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirePressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireReleased);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageAplyed);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageAplyed, AActor*, DamagedActor, float, DamageAmount);


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EUTPlayerState : uint8
{
	Death			UMETA(DisplayName = "Death"),
	Alive			UMETA(DisplayName = "Alive"),
	NON			UMETA(DisplayName = "NON"),

};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIStateChangedWithParam, EUTPlayerState, NewState);


UCLASS(config=Game)
class AUT_GameCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:


	UFUNCTION(Client, Reliable)
	void Client_UpdateDamageUI(float DamageAmount);

	UPROPERTY(BlueprintAssignable, Category = "AI")
	FOnAIStateChangedWithParam OnAIStateChangedWithParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	EUTPlayerState UTPlayerState = EUTPlayerState::NON;

	UFUNCTION()
	void Respawn();

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		if (const AController* C = GetController())
		{
			if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(C))
			{
				return TeamAgent->GetGenericTeamId();
			}
		}
		return FGenericTeamId(1); // default
	}

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* FireStartAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* CrouchStartAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* CrouchEndAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FOnFirePressed OnFirePressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FOnFireReleased OnFireReleased;

	UPROPERTY(BlueprintAssignable, Category = "Damage")
	FOnDamageAplyed OnDamageAplyed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* DropItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* PickUpItemAction;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;


	AUT_GameCharacter();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	UUT_WeaponComponent* WeaponComponent;
	
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// APawn interface
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float DamageUI;
	UFUNCTION(BlueprintCallable)

	void UpdateDamageFunctionTemp(float DamageAmmount);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool bEquipped = false;
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void UpdateUIDamage(AActor* Actor, float DamageAmount);

	UFUNCTION()
	void HandleFirePressed();

	UFUNCTION()
	void HandleFireReleased();

	UFUNCTION()
	void StartCrouch();

	UFUNCTION()
	void EndCrouch();
	
	UFUNCTION()
	void DropItem();

	UFUNCTION()
	void PickUpItem();

	UFUNCTION(BlueprintCallable)
	void SwitchWeapon();

	
	UFUNCTION(Server, Reliable)
	void Server_UpdateAimOffset(float Pitch);

	float GetAO_Pitch() const { return AO_PITCH; }
	float GetHealth() const { return Health; } 
	UFUNCTION(Server, Reliable)
	void Server_DropItem();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DropItem();
protected:

	virtual void MakeHit();
	virtual void StopHit();

	UFUNCTION(Server, Reliable)
	void Server_SwitchWeapon();

	UFUNCTION(Server, Reliable)
	void Server_PickUpItem(AActor* TargetItem);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PickUpItem(AActor* TargetItem);



	UPROPERTY(Replicated)
	float AO_PITCH;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PossessedBy(AController* NewController) override;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health = 100.0f;

	

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.0f;
	UFUNCTION(BlueprintCallable)
	void Die();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Ragdoll();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerWidget> PlayerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UPlayerWidget* PlayerWidgetInstance;

	UFUNCTION()
	AActor* CalculateTraceDirection();

	
};

