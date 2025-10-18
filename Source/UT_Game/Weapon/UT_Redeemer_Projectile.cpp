// Fill out your copyright notice in the Description page of Project Settings.


#include "../Weapon/UT_Redeemer_Projectile.h"
#include "Sound/SoundCue.h"


void AUT_Redeemer_Projectile::MakeShot(FVector ShootDirection)
{
    UE_LOG(LogTemp, Warning, TEXT("Projectile prepared with name: %s :: %f"), *this->GetName(), InitialSpeed);

    if (!ShootDirection.IsNormalized())
    {
        ShootDirection.Normalize();
    }

    SetActorRotation(ShootDirection.Rotation());

    if (ProjectileMovement)
    {
        ProjectileMovement->InitialSpeed = InitialSpeed;
        ProjectileMovement->MaxSpeed = InitialSpeed; // important
        ProjectileMovement->Velocity = ShootDirection * InitialSpeed;
        ProjectileMovement->Activate(true);
    }

    Multicast_PlayFireEffects(ShootDirection);
}

