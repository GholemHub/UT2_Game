#include "UT_PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void AUT_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("[SERVER] OnPossess: %s"), *GetNameSafe(InPawn));

	// Server-specific logic here (like inventory setup, replication init, etc.)
}

void AUT_PlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	UE_LOG(LogTemp, Warning, TEXT("[CLIENT] OnRep_Pawn triggered, Pawn: %s"), *GetNameSafe(GetPawn()));


	// This is where clients "feel" possession
	if (IsLocalController() && GetPawn())
	{
		OnPawnAssigned.Broadcast(GetPawn());
		CreateMainHUD();
	}
}

void AUT_PlayerController::CreateMainHUD()
{
	if (!IsLocalController()) return;
	if (MainHUDInstance) return; // Prevent duplicates
	if (!MainHUDClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainHUDClass not assigned in %s!"), *GetName());
		return;
	}

	MainHUDInstance = CreateWidget<UUserWidget>(this, MainHUDClass);
	if (MainHUDInstance)
	{
		MainHUDInstance->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("MainHUD created for %s"), *GetName());
	}
}