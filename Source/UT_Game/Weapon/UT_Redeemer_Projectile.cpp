// Fill out your copyright notice in the Description page of Project Settings.


#include "../Weapon/UT_Redeemer_Projectile.h"
#include "Sound/SoundCue.h"



AUT_Redeemer_Projectile::AUT_Redeemer_Projectile()
{

        MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
        MeshComponent->SetSimulatePhysics(true);
        MeshComponent->SetEnableGravity(true);
}

void AUT_Redeemer_Projectile::MakeShot(FVector ShootDirection)
{
    if (MeshComponent) // Ensure MeshComponent exists
    {
        if (ShootDirection.IsNearlyZero())
        {
            ShootDirection = GetActorForwardVector();
        }
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Shot Direction: %s"), *ShootDirection.ToString()));
        FVector Impulse = ShootDirection * 6000.0f; // Adjust impulse strength

        MeshComponent->AddImpulse(Impulse, NAME_None, true); // Apply impulse to the mesh
        Multicast_PlayFireEffects(ShootDirection);
    }
    
    
}
