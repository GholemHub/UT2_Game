#include "Components/StaticMeshComponent.h"
#include "../Weapon/UT_Flak_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AUT_Flak_Projectile::AUT_Flak_Projectile()
{
    bReplicates = true;
    SetReplicateMovement(true);

    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    MeshComponent->SetSimulatePhysics(true); // So it reacts physically
    MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic")); // Common and reliable
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
    MeshComponent->SetNotifyRigidBodyCollision(true); // Enables OnHit

    MeshComponent->OnComponentHit.AddDynamic(this, &AUT_Flak_Projectile::OnHit);

    ParticleSystemComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionEffect"));
    ParticleSystemComp->SetupAttachment(RootComponent);
    ParticleSystemComp->bAutoActivate = false; // Don't play it automatically
}

// Called when the game starts or when spawned
void AUT_Flak_Projectile::BeginPlay()
{
	Super::BeginPlay();
 
    MeshComponent->OnComponentHit.AddUniqueDynamic(this, &AUT_Flak_Projectile::OnHit);

}
#include "../AI/UT_AICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void AUT_Flak_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!HasAuthority() || bHasExploded) return;

    bHasExploded = true; // Add a flag to prevent double execution

    MeshComponent->SetSimulatePhysics(false); // Stop physics first
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Now safe

    AController* InstigatorController = GetInstigatorController();

    if (OtherActor && OtherActor != this && OtherComp)
    {
        if (WeaponIsRadial) {
            float RandomValue = FMath::FRandRange(Damage - 5.0f, Damage + 5.0f);
            UGameplayStatics::ApplyRadialDamage(
                this, RandomValue, Hit.ImpactPoint, 200.f,
                UDamageType::StaticClass(), TArray<AActor*>(),
                this, InstigatorController, true
            );
            //if (OtherComp && OtherComp->IsSimulatingPhysics())
            //{
            //    // Apply physics impulse directly if object has simulating component
            //    FVector ImpactDirection = Hit.ImpactNormal * -1.0f; // Push away from hit surface
            //    float ImpulseStrength = 50000.0f; // tweak this
            //    OtherComp->AddImpulseAtLocation(ImpactDirection * ImpulseStrength, Hit.ImpactPoint);
            //}
            //else
            //{
            //    auto HitCharacter = Cast<AUT_AICharacter>(OtherActor);
            //    if (HitCharacter && HitCharacter->GetCharacterMovement()->MovementMode != MOVE_None)
            //    {

            //        FVector LaunchDir = MeshComponent->GetPhysicsLinearVelocity().GetSafeNormal();
            //        if (LaunchDir.IsNearlyZero())
            //        {
            //            LaunchDir = GetActorForwardVector();
            //        }
            //        LaunchDir.Z = 0.001f;

            //        float KnockbackStrength = 1000.0f; // adjust as needed
            //        FVector Knockback = LaunchDir * KnockbackStrength;

            //        // Apply impulse to enemy movement
            //        HitCharacter->GetCharacterMovement()->AddImpulse(Knockback, true);
            //    }
            //}
            
        }
        else {
            float RandomValue = FMath::FRandRange(Damage - 2.0f, Damage + 2.0f);
            UGameplayStatics::ApplyDamage(
                OtherActor, RandomValue, InstigatorController,
                this, UDamageType::StaticClass()
            );

            //if (OtherComp && OtherComp->IsSimulatingPhysics())
            //{
            //    // Apply physics impulse directly if object has simulating component
            //    FVector ImpactDirection = Hit.ImpactNormal * -1.0f; // Push away from hit surface
            //    float ImpulseStrength = 5000.0f; // tweak this
            //    OtherComp->AddImpulseAtLocation(ImpactDirection * ImpulseStrength, Hit.ImpactPoint);
            //}
            //else
            //{
            //    auto HitCharacter = Cast<AUT_AICharacter>(OtherActor);
            //    if (HitCharacter && HitCharacter->GetCharacterMovement()->MovementMode != MOVE_None)
            //    {
            //      
            //        FVector LaunchDir = MeshComponent->GetPhysicsLinearVelocity().GetSafeNormal();
            //        if (LaunchDir.IsNearlyZero())
            //        {
            //            LaunchDir = GetActorForwardVector();
            //        }
            //        LaunchDir.Z = 0.001f;
            //        UE_LOG(LogTemp, Warning, TEXT("KnockBack:::%s"), *LaunchDir.ToString());
            //            //LaunchDir.Normalize();
            //        UE_LOG(LogTemp, Warning, TEXT("KnockBack22:::%s"), *LaunchDir.ToString());

            //        float KnockbackStrength = 300.0f; // adjust as needed
            //        FVector Knockback = LaunchDir * KnockbackStrength;

            //        // Apply impulse to enemy movement
            //        HitCharacter->GetCharacterMovement()->AddImpulse(Knockback, true);
            //    }

            //}
        }
    }

    Multicast_ExplodeEffects(Hit.ImpactPoint);
    FTimerHandle DestroyTimerHandle;
    // Destroy after delay to allow VFX to play
    GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AUT_Flak_Projectile::DestroyAfterDelay, 2.0f, false);
}


// Called every frame
void AUT_Flak_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (MeshComponent && MeshComponent->GetPhysicsLinearVelocity().SizeSquared() > 0.1f)
    {
        FVector Velocity = MeshComponent->GetPhysicsLinearVelocity();
        FRotator NewRotation = Velocity.Rotation();
        SetActorRotation(NewRotation);
    }
}

void AUT_Flak_Projectile::MakeShot(FVector ShootDirection)
{
    UE_LOG(LogTemp, Warning, TEXT("Projectile prepared with name: %s"), *this->GetName());

    if (!ShootDirection.IsNormalized())
    {
        ShootDirection.Normalize();
    }

    if (MeshComponent)
    {

        FVector Impulse = ShootDirection * FireImpulse; // Increase to a proper force
        FRotator Rotation = ShootDirection.Rotation();
        SetActorRotation(Rotation);
        MeshComponent->AddImpulse(Impulse, NAME_None, true);

        Multicast_PlayFireEffects(ShootDirection);
    }
}

void AUT_Flak_Projectile::DestroyAfterDelay()
{
    //MeshComponent->SetNotifyRigidBodyCollision(false); // Enables OnHit

    Destroy();
}

void AUT_Flak_Projectile::Multicast_ExplodeEffects_Implementation(FVector Location)
{
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Location);
    }

    if (ParticleSystemComp)
    {
        ParticleSystemComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        ParticleSystemComp->ActivateSystem();
    }

    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MeshComponent->SetVisibility(false);
}

void AUT_Flak_Projectile::Multicast_PlayFireEffects_Implementation(FVector ShootDirection)
{
    if (FireSound && GetWorld())
    {
        //GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Sound Fire"));
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, ShootDirection);
    }
}
