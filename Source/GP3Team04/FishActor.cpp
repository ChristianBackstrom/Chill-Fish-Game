#include "FishActor.h"
#include "FishManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AFishActor::AFishActor()
{
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USceneComponent>("Root");
    SetRootComponent(Root);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
    SkeletalMesh->SetupAttachment(Root);

    BubbleMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bubble Mesh");
    BubbleMesh->SetupAttachment(SkeletalMesh);
    BubbleMesh->SetVisibility(false);
    
    SphereCollider = CreateDefaultSubobject<USphereComponent>("Sphere Collider");
    SphereCollider->SetupAttachment(SkeletalMesh);
    SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereCollider->SetCollisionObjectType(ECC_PhysicsBody);
    SphereCollider->SetCollisionObjectType(ECC_WorldDynamic);
    SphereCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
    SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AFishActor::OnOverlapBegin);
    
    FishManager = Cast<AFishManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFishManager::StaticClass()));

    OriginalSwimSpeed = SwimSpeed;

    InitialBoostDuration = BoostDuration;
}

void AFishActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(!bEnabled) return;
    FishMovement(DeltaTime);
    
    if (bBoostActive)
    {
        // Reduce the boost duration
        BoostDuration -= DeltaTime;

        if (BoostDuration <= 0)
        {
            // Boost duration expired, reset swim speed to normal
            SwimSpeed = OriginalSwimSpeed;
            bBoostActive = false;
            BoostDuration = InitialBoostDuration;
        }
    }
}

void AFishActor::FishMovement(float DeltaTime)
{
    if (!bShouldMove)
        return;
    
    FVector CurrentLocation = GetActorLocation();
    FRotator CurrentRotation = GetActorRotation();

    if (!bReachedLocation)
    {
        MoveToRandomLocation(CurrentLocation);
    }
    else
    {
        MoveToNewLocation(RandomLocation, DeltaTime, false);
    }

    InterpolateRotation(CurrentRotation, DeltaTime);
}

void AFishActor::MoveToRandomLocation(const FVector& CurrentLocation)
{
    RandomLocation = FishManager->GetRandomLocation();
    TargetRotation = (RandomLocation - CurrentLocation).Rotation();
    
    bReachedLocation = true;
    OldLocation = CurrentLocation;
}


void AFishActor::MoveToNewLocation(const FVector& TargetLocation, float DeltaTime, bool bBoostedSpeed)
{
    FVector CurrentLocation = GetActorLocation();
    FVector MoveDirection = (TargetLocation - CurrentLocation).GetSafeNormal();

    // Calculate the speed based on whether it's a normal movement or a boosted movement
    float MovementSpeed = bBoostedSpeed ? SwimSpeed * BoostSpeedMultiplier : SwimSpeed;

    FVector NewLocation = CurrentLocation + MoveDirection * MovementSpeed * DeltaTime;

    SetActorLocation(NewLocation);

    const float SquaredDistanceThreshold = 100.f * 100.f;
    if (FVector::DistSquared(CurrentLocation, TargetLocation) < SquaredDistanceThreshold)
    {
        bReachedLocation = false;
    }

    OldLocation = CurrentLocation;
}

void AFishActor::InterpolateRotation(const FRotator& CurrentRotation, float DeltaTime)
{
    if (bReachedLocation)
    {
        float InterpSpeed = 1.0f / TurnTime;
        FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, InterpSpeed * DeltaTime);
        SetActorRotation(NewRotation);
    }
}

void AFishActor::AvoidCollision()
{
    if (!bBoostActive)
    {
        SetActorLocation(OldLocation);
        ApplyStartleBoost();
        bReachedLocation = false;
    }
}

void AFishActor::ApplyStartleBoost()
{
    if (bCanBeScared)
    {
        float RandomChance = FMath::FRandRange(0.f, 1.f);

        if (RandomChance <= StartleChance)
        {
            if (!bBoostActive)
            {
                SwimSpeed *= BoostSpeedMultiplier;
                bBoostActive = true;
                BoostDuration = InitialBoostDuration;
            }
        }
    }
}

void AFishActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    SetActorLocation(OldLocation);
    bReachedLocation = false;

    AvoidCollision();
}

void AFishActor::FishIsCaught()
{
    BubbleMesh->SetVisibility(true);
}
