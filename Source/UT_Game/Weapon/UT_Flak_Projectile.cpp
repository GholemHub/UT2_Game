#include "Components/StaticMeshComponent.h"
#include "../Weapon/UT_Flak_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../AI/UT_AICharacter.h"




// Sets default values
AUT_Flak_Projectile::AUT_Flak_Projectile()
{
    bReplicates = true;
    SetReplicateMovement(true);

    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // блокуй лише світ
    MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // або ігноруй
    MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // персонажі — тільки оверлап
    MeshComponent->SetGenerateOverlapEvents(true);

    //MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AUT_Flak_Projectile::OnOverlap);

    //MeshComponent->OnComponentHit.AddDynamic(this, &AUT_Flak_Projectile::OnHit);

    ParticleSystemComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionEffect"));
    ParticleSystemComp->SetupAttachment(RootComponent);
    ParticleSystemComp->bAutoActivate = false; // Don't play it automatically

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void AUT_Flak_Projectile::BeginPlay()
{
    Super::BeginPlay();

    MeshComponent->OnComponentHit.AddUniqueDynamic(this, &AUT_Flak_Projectile::OnHit);
    MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AUT_Flak_Projectile::OnOverlap);
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

    UE_LOG(LogTemp, Error, TEXT("OnHit :: END"));
    // Destroy after delay to allow VFX to play
    GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AUT_Flak_Projectile::DestroyAfterDelay, 2.0f, false);
}

void AUT_Flak_Projectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this)
        return;

    FVector ExplosionLocation = GetActorLocation(); // default fallback

    if (bFromSweep)
    {
        // Sometimes SweepResult.Location is valid when moving fast
        ExplosionLocation = SweepResult.Location;
    }
    else if (OtherComp)
    {
        // If we overlapped a mesh or capsule, use its position
        ExplosionLocation = OtherComp->GetComponentLocation();
    }
    else
    {
        ExplosionLocation = OtherActor->GetActorLocation();
    }

    UE_LOG(LogTemp, Error, TEXT("OnOverlap :: END :: %s"), *ExplosionLocation.ToString());

    // Apply damage
    AController* InstigatorController = GetInstigatorController();

    
    if (WeaponIsRadial) {
        float RandomValue = FMath::FRandRange(Damage - 5.0f, Damage + 5.0f);
        UGameplayStatics::ApplyRadialDamage(
            this, RandomValue, ExplosionLocation, 200.f,
            UDamageType::StaticClass(), TArray<AActor*>(),
            this, InstigatorController, true
        );
    }
    else {
        float RandomValue = FMath::FRandRange(Damage - 2.0f, Damage + 2.0f);
        UGameplayStatics::ApplyDamage(
            OtherActor, RandomValue, InstigatorController,
            this, UDamageType::StaticClass()
        );
    }
    

    //UGameplayStatics::ApplyDamage(OtherActor, Damage, InstigatorController, this, UDamageType::StaticClass());

    // Visuals
    Multicast_ExplodeEffects(ExplosionLocation);

    FTimerHandle DestroyTimerHandle;
    GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AUT_Flak_Projectile::DestroyAfterDelay, 1.0f, false);
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
    UE_LOG(LogTemp, Warning, TEXT("Projectile prepared with name FLAKK: %s"), *this->GetName());

    if (!ShootDirection.IsNormalized())
    {
        ShootDirection.Normalize();
    }

    SetActorRotation(ShootDirection.Rotation());

    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
        ProjectileMovement->Activate(true);
    }
    Multicast_PlayFireEffects(ShootDirection);
}

void AUT_Flak_Projectile::DestroyAfterDelay()
{
    //MeshComponent->SetNotifyRigidBodyCollision(false); // Enables OnHit

    Destroy();
}

void AUT_Flak_Projectile::Multicast_ExplodeEffects_Implementation(FVector Location)
{
    // 1. Зупини ProjectileMovement
    if (ProjectileMovement)
    {
        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->Deactivate(); // повністю відключає компонент
    }

    // 2. Відтворюємо звук вибуху
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Location);
    }

    // 3. Частинки вибуху
    if (ParticleSystemComp)
    {
        // Відірви, щоб ефект не рухався разом зі снарядом
        ParticleSystemComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        ParticleSystemComp->SetWorldLocation(Location);
        ParticleSystemComp->ActivateSystem();
    }

    // 4. Вимкни MeshComponent, щоб не було колізій
    if (MeshComponent)
    {
        MeshComponent->SetSimulatePhysics(false);
        MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        MeshComponent->SetVisibility(false);
    }
}


void AUT_Flak_Projectile::Multicast_PlayFireEffects_Implementation(FVector ShootDirection)
{
    if (FireSound && GetWorld())
    {
        //GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Sound Fire"));
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, ShootDirection);
    }
}