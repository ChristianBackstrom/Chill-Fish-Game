// Fill out your copyright notice in the Description page of Project Settings.


#include "FishManager.h"
#include "FishActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFishManager::AFishManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("Box Collider");
	BoxComponent->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AFishManager::BeginPlay()
{
	Super::BeginPlay();
    
    SpawnFish();
    DisableFishManager();
}

void AFishManager::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    BoatActor = UGameplayStatics::GetActorOfClass(GetWorld(), BoatActorClass);

    if(BoatActor)
    {
        float Magnitude = FVector::Distance(BoatActor->GetActorLocation(), GetActorLocation());

        if(Magnitude <= DistanceToManager && !bManagerIsActive)
        {
            EnableFishManager();
            return;
        }
    
        if(Magnitude > DistanceToManager && bManagerIsActive)
        {
            DisableFishManager();
        }
    }
}

//Gets a random location in the box collider set in the world.
FVector AFishManager::GetRandomLocation() const
{
	FVector BoxExtent = BoxComponent->GetScaledBoxExtent();
	FVector BoxOrigin = BoxComponent->GetComponentLocation();
	FBox Box(BoxOrigin - BoxExtent, BoxOrigin + BoxExtent);
	
	FVector RandomPoint = FMath::RandPointInBox(Box);
	return RandomPoint;	
}

void AFishManager::EnableFishManager()
{
    bManagerIsActive = true;
    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);

    for (AFishActor* Fish : FishList)
    {
        Fish->SetActorEnableCollision(true);
        Fish->SetActorHiddenInGame(false);
        Fish->bEnabled = true;
    }
}

void AFishManager::DisableFishManager()
{
    bManagerIsActive = false;
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);
    
    for (AFishActor* Fish : FishList)
    {
        if (!IsValid(Fish))
        {
            FishList.Remove(Fish);
            continue;
        }
        
        Fish->SetActorEnableCollision(false);
        Fish->SetActorHiddenInGame(true);
        Fish->bEnabled = false;
    }
}

void AFishManager::SpawnFish()
{
    FVector BoxExtent = BoxComponent->GetScaledBoxExtent();
    FVector BoxOrigin = BoxComponent->GetComponentLocation();

    int32 AmountOfSpecialFish = 0;

    for (int32 i = 0; i < FishAmount; i++)
    {
        TSubclassOf<AFishActor> FishType;

        if (AmountOfSpecialFish < SpecialFishAmount && SpecialFishClass)
        {
            FishType = SpecialFishClass;
        }
        else
        {
            FishType = FishClass[FMath::RandRange(0, FishClass.Num() - 1)];
        }

        /*
        int32 RandomSize = FMath::RandRange(1, 3);

        int32 RandomValue = FMath::RandRange(0, 100);

         if (RandomValue < LargeFishChance)
         {
             RandomSize = Large;
         }
         else
         {
             RandomSize = Small;
         }
        */
        
        FVector SpawnLocation;

        // Attempt to find a valid spawn location
        bool bFoundValidLocation = false;
        const int32 MaxAttempts = 10;
        int32 Attempts = 0;

        while (!bFoundValidLocation && Attempts < MaxAttempts)
        {
            SpawnLocation = FMath::RandPointInBox(FBox(BoxOrigin - BoxExtent, BoxOrigin + BoxExtent));

            // Check if the spawn location is obstructed by any other object in the world
            FCollisionQueryParams CollisionParams;
            FCollisionObjectQueryParams ObjectQueryParams;
            ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
            ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

            FHitResult HitResult;
            if (!GetWorld()->SweepSingleByObjectType(HitResult, SpawnLocation, SpawnLocation, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeSphere(10.f), CollisionParams))
            {
                bFoundValidLocation = true;
            }

            Attempts++;
        }

        // If a valid spawn location was not found, skip spawning this fish
        if (!bFoundValidLocation)
        {
            continue;
        }

        // Check if the spawn location is too close to any existing fish
        bool bTooClose = false;
        for (AFishActor* Fish : FishList)
        {
            if (FVector::Dist(SpawnLocation, Fish->GetActorLocation()) < 20.f)
            {
                bTooClose = true;
                break;
            }
        }

        // If the spawn location is too close to any existing fish, skip spawning this fish
        if (bTooClose)
        {
            continue;
        }

        // Spawn the fish
        AFishActor* NewFish = GetWorld()->SpawnActor<AFishActor>(FishType, SpawnLocation, FRotator::ZeroRotator);
        NewFish->FishManager = this;
        //NewFish->Fish.Size = static_cast<FishSize>(RandomSize);

        FVector Size = FVector::One();

        float SmallFishSize = FMath::RandRange(SmallFishMinSize, SmallFishMaxSize);
        float LargeFishSize = FMath::RandRange(LargeFishMinSize, LargeFishMaxSize);
        
        switch (NewFish->Fish.Size)
        {
            case Small:
                Size = FVector(SmallFishSize);
                break;
            case Large:
                Size = FVector(LargeFishSize);
                break;
        }
        NewFish->SetActorScale3D(Size);

        // Set a random material on each fish.
        if (NewFish->InstanceMaterial.Num() > 0)
        {
            int MaterialType = FMath::RandRange(0, NewFish->InstanceMaterial.Num() - 1);

            NewFish->SkeletalMesh->SetMaterial(0, NewFish->InstanceMaterial[MaterialType]);
        }
        
        NewFish->FishManager = this;
        NewFish->OldLocation = SpawnLocation;
        FishList.Add(NewFish);
        AmountOfSpecialFish++;
    }
}
