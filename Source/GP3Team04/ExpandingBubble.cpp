// Fill out your copyright notice in the Description page of Project Settings.


#include "ExpandingBubble.h"

#include "AuroraBorealis.h"
#include "FishingGamemode.h"
#include "Kismet/GameplayStatics.h"

void AExpandingBubble::BeginPlay()
{
	Super::BeginPlay();
}


void AExpandingBubble::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AAuroraBorealis>(OtherActor)) return;
	if (Cast<ABubble>(OtherActor)) return;
	
	if (!bShouldCoolide) return;
	
	if (AFishActor* FishActor = Cast<AFishActor>(OtherActor))
	{
		FishActor->bShouldMove = false;
		bShouldMove = false;
		CaughtFish.Add(FishActor);

		
		if (!bExpandStarted)
		{
			Time = UGameplayStatics::GetTimeSeconds(World);
			bExpandStarted = true;
			BaseScale = GetActorScale3D();
			FTimerHandle TimerHandle;
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &AExpandingBubble::Expand);
			GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.001f, true);
		}

		return;
	}

	ExpandEnded();
}

void AExpandingBubble::Expand()
{
	float TotalTime = UGameplayStatics::GetTimeSeconds(World) - Time;

	if (TotalTime >= 1.f)
	{
		ExpandEnded();
	}
	
	SetActorRelativeScale3D(BaseScale + FVector::OneVector * TotalTime * ExpandRadiusMultiply);
}

void AExpandingBubble::ExpandEnded()
{

	if (UFishingGameInstance* FishingGameInstance = Cast<UFishingGameInstance>(UGameplayStatics::GetGameInstance(World)))
	{
		for (AFishActor* FishActor : CaughtFish)
		{
			FishingGameInstance->CaughtFish(FishActor->Fish);	
		}
	}

	if (AFishingGamemode* FishingGamemode = Cast<AFishingGamemode>(UGameplayStatics::GetGameMode(World)))
	{
		for (AFishActor* FishActor : CaughtFish)
		{
			FishingGamemode->FishCaught(TArray<int> { 1 }, TArray<float> { (float) FishActor->Fish.Size + 1 });
		}
	}

	for (AFishActor* FishActor : CaughtFish)
	{
		FishActor->Destroy();
	}
		
	PopBubble();
}



