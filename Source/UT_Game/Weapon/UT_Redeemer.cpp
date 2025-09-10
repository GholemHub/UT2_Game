// Fill out your copyright notice in the Description page of Project Settings.


#include "../Weapon/UT_Redeemer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "../UT_GameCharacter.h"


//void AUT_Redeemer::Fire(FVector_NetQuantize FireOrigin)
//{
//
//	UE_LOG(LogTemp, Warning, TEXT("LOG2"));
//
//	if (!bCanFire) return;
//
//	APlayerController* PC = GetWorld()->GetFirstPlayerController();
//	if (!PC) return;
//
//	int32 ScreenWidth, ScreenHeight;
//	PC->GetViewportSize(ScreenWidth, ScreenHeight);
//
//	FVector2D ScreenCenter(ScreenWidth * 0.5f, ScreenHeight * 0.5f);
//	FVector WorldLocation, WorldDirection;
//	PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection);
//
//	FVector FirePointLocation = GetFireOriginFromSocket();
//	FVector EndTrace = FirePointLocation + (WorldDirection * 10000.0f);
//
//	FHitResult HitResult;
//	FCollisionQueryParams TraceParams;
//	TraceParams.AddIgnoredActor(this);
//	TraceParams.AddIgnoredActor(GetOwner());
//
//	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, FirePointLocation, EndTrace, ECC_Visibility, TraceParams);
//
//	FVector ShootDirection = bHit ? (HitResult.ImpactPoint - FirePointLocation).GetSafeNormal() : WorldDirection;
//
//	if (!ShootDirection.IsNearlyZero())
//	{
//		ServerFire(ShootDirection, FireOrigin);
//	}
//}
