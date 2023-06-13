// Fill out your copyright notice in the Description page of Project Settings.


#include "Bubble.h"

#include "AuroraBorealis.h"
#include "FishActor.h"
#include "FishingGameInstance.h"
#include "FishingGamemode.h"
#include "FishManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABubble::ABubble()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	RootComponent = StaticMeshComponent;
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ABubble::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ABubble::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
}

// Called every frame
void ABubble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bShouldMove) return;
	
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * DeltaTime * Speed, true);
	LifeTime -= DeltaTime;

	if (MaxLifeTime / 2.f >= LifeTime)
	{
		Speed = baseSpeed + SpeedIncrease;
	}
	

	if (LifeTime <= 0)
	{
		PopBubble();
	}
}

void ABubble::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ABubble>(OtherActor)) return;
	
	if (!bShouldCoolide) return;
	
	bShouldCoolide = false;
	
	if (AFishActor* FishActor = Cast<AFishActor>(OtherActor))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, "Hit Fish");

		if (!FishActor->bShouldMove)
		{
			PopBubble();
			return;
		}
		
		CatchFish(FishActor);
		
		return;
	}

	PopBubble();
}

void ABubble::CatchFish(AFishActor* FishActor)
{
	bShouldMove = false;
		
	if (UFishingGameInstance* FishingGameInstance = Cast<UFishingGameInstance>(UGameplayStatics::GetGameInstance(World)))
	{
		FFish Fish = FishActor->Fish;

		if (Fish.Size.GetValue() / 2.f > Size && !bIgnoreSize)
		{
			PopBubble();
			return;
		}
		
		if (AFishingGamemode* FishingGamemode = Cast<AFishingGamemode>(UGameplayStatics::GetGameMode(World)))
		{
			FishingGamemode->FishCaught(TArray<int> { 1 }, TArray<float> { (float)Fish.Size.GetValue() + 1 });
		}
		FishingGameInstance->CaughtFish(Fish);
	}

	this->CaughtFish = FishActor;
	Time = UGameplayStatics::GetTimeSeconds(World);
	StartLocation = GetActorLocation();
	StartScale = GetActorScale3D();
		
	TargetLocation = FishActor->BubbleMesh->GetComponentLocation();
	TargetScale = FishActor->BubbleMesh->GetComponentScale();
		
	FTimerHandle CatchingHandle;
	FTimerDelegate CatchingDelegate;
	CatchingDelegate.BindUObject(this, &ABubble::CaptureFish);
	GetWorldTimerManager().SetTimer(CatchingHandle, CatchingDelegate, 0.001f, true);
	
	FishActor->SetActorEnableCollision(false);
	FishActor->bEnabled = false;
	FishActor->bShouldMove = false;
}

void ABubble::PopBubble()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "POPS");
	GetWorldTimerManager().ClearAllTimersForObject(this);
	POP();
	OnBubblePop.Broadcast();
	this->Destroy();
}

void ABubble::PopBubble(AFishActor* Fish)
{
	Fish->FishManager->FishList.Remove(Fish);
	Fish->Destroy();
	PopBubble();
}

void ABubble::FloatBubble(AFishActor* Fish)
{
	FVector Up = FVector::UpVector * .02f;
	Fish->AddActorWorldOffset(Up);
	AddActorWorldOffset(Up);
}

void ABubble::CaptureFish()
{
	float TotalTime = (UGameplayStatics::GetTimeSeconds(World) - Time) / CaptureSpeed;
	TotalTime = FMath::Clamp(TotalTime, 0.f, 1.f);

	FVector Location = FMath::Lerp(StartLocation, TargetLocation, TotalTime);
	FVector Scale = FMath::Lerp(StartScale, TargetScale, UKismetMathLibrary::Ease(0, 1.f, TotalTime, EEasingFunc::EaseInOut));

	SetActorLocation(Location);
	SetActorScale3D(Scale);

	if (TotalTime >= 1.f)
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
		
		//Timer for Pop handlign
		FTimerHandle TimerHandle;
		FTimerDelegate PopDelegate;
		PopDelegate.BindUObject(this, &ABubble::PopBubble, CaughtFish);
		GetWorldTimerManager().SetTimer(TimerHandle, PopDelegate, 0.1f, false, FloatDuration);

		//Timer for floating up handling
		FTimerDelegate FloatDelegate;
		FTimerHandle FloatHandle;
		FloatDelegate.BindUObject(this, &ABubble::FloatBubble, CaughtFish);
		GetWorldTimerManager().SetTimer(FloatHandle, FloatDelegate, 0.0001f, true);

		FishCaught(CaughtFish);
		CaughtFish->FishIsCaught();
		StaticMeshComponent->SetVisibility(false);
	}
}


