// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_Crunch_AICharacter.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"



AUT_Crunch_AICharacter::AUT_Crunch_AICharacter()
{
	DanageZoneComponentSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DanageZoneComponentSphere"));
	DanageZoneComponentSphere->SetupAttachment(Mesh1P);

	DanageZoneComponentSphere->SetHiddenInGame(false);
	DanageZoneComponentSphere->OnComponentBeginOverlap.AddDynamic(this, &AUT_Crunch_AICharacter::OnSphereBeginOverlap);
	DanageZoneComponentSphere->OnComponentEndOverlap.AddDynamic(this, &AUT_Crunch_AICharacter::OnSphereEndOverlap);

}


void AUT_Crunch_AICharacter::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	auto Player = Cast<AUT_GameCharacter>(OtherActor);
	auto Crunch = Cast<AUT_Crunch_AICharacter>(OtherActor);
	if (Crunch) return;

	if (Player){
		UE_LOG(LogTemp, Warning, TEXT("******Sphere overlapped with: %s"), *OtherActor->GetName());
		bCanAtack = true;	
	}
}

void AUT_Crunch_AICharacter::OnSphereEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	auto Player = Cast<AUT_GameCharacter>(OtherActor);
	if (Player) {
		UE_LOG(LogTemp, Warning, TEXT("******Sphere Endlapped with: %s"), *OtherActor->GetName());
		bCanAtack = false;
	}
}

void AUT_Crunch_AICharacter::BeginPlay()
{

	Super::BeginPlay();
}

void AUT_Crunch_AICharacter::MakeHit()
{
	UE_LOG(LogTemp, Warning, TEXT("*** Cranch made HIT ***"));
	bIsAttacking = true;
	
	FTimerHandle ImpulseTimerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(
		ImpulseTimerHandle,
		this,
		&AUT_Crunch_AICharacter::ApplyRadialImpulse,
		0.4f,
		false
	);

	//bIsAttacking = !bIsAttacking;
}

void AUT_Crunch_AICharacter::ApplyRadialImpulse()
{
	FName BoneName;

		TypeOfAttack = FMath::RandBool();
		if (TypeOfAttack == 0) {
			BoneName = TEXT("hand_r");
		}
		else {
			BoneName = TEXT("hand_l");
		}
		FVector HandLocation = Mesh1P->GetBoneLocation(BoneName);

		float Radius = 300.f;
	
		TArray<FOverlapResult> Overlaps;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

		bool bHit = GetWorld()->OverlapMultiByChannel(
			Overlaps,
			HandLocation,
			FQuat::Identity,
			ECC_Pawn,   // only overlap pawns (players, AI)
			Sphere
		);

		if (bHit)
		{
			for (auto& Result : Overlaps)
			{
				auto* OtherChar = Cast<AUT_GameCharacter>(Result.GetActor());
				// Add this include at the top of your file, after other includes
         
				if (OtherChar && OtherChar != this)
				{
					if (OtherChar->IsPlayerControlled())
					{
						FVector Direction = (OtherChar->GetActorLocation() - HandLocation).GetSafeNormal();
						FVector Horizontal = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
						FVector Impulse = Horizontal * 1000.f + FVector(0, 0, 500.f);
				
						OtherChar->LaunchCharacter(Impulse, true, true);

						float Damage = 0.f;
						UGameplayStatics::ApplyDamage(
							OtherChar,
							Damage,
							GetController(),
							this,
							UDamageType::StaticClass()
						);
						PlaySpund(AttackSound);
				
						UE_LOG(LogTemp, Warning, TEXT("***** Impulse applied to: %s, Vector: %s"),*OtherChar->GetName(), *Impulse.ToString());
					}
				}
			}
		}
}

void AUT_Crunch_AICharacter::StopHit()
{
	UE_LOG(LogTemp, Warning, TEXT("*** Cranch STOPED HITING ***"));
}

void AUT_Crunch_AICharacter::MakeHitAnimation()
{

}

void AUT_Crunch_AICharacter::PlaySpund_Implementation(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Sound,
			GetActorLocation()
		);
	}
}

void AUT_Crunch_AICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUT_Crunch_AICharacter, bIsAttacking);
	DOREPLIFETIME(AUT_Crunch_AICharacter, bCanAtack);
}


void AUT_Crunch_AICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}