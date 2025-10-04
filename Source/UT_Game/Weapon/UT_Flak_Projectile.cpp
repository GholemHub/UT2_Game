#include "Components/StaticMeshComponent.h"
#include "../Weapon/UT_Flak_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"


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


           /* DrawDebugSphere(
                GetWorld(), Hit.ImpactPoint, 200.0f, 12,
                FColor::Red, false, 2.0f, 0, 1.5f
            );*/
        }
        else {
            float RandomValue = FMath::FRandRange(Damage - 2.0f, Damage + 2.0f);
            UGameplayStatics::ApplyDamage(
                OtherActor, RandomValue, InstigatorController,
                this, UDamageType::StaticClass()
            );
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
