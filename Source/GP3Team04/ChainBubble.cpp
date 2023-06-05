// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainBubble.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void AChainBubble::TeleportToFish()
{
	TArray<FHitResult> HitResults;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart + FVector::UpVector * Range;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(Range);

	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, FQuat::Identity,
	                                            ECC_GameTraceChannel1, CollisionShape);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor != this && HitActor->IsA<AFishActor>())
			{
				NearestActor = HitActor;
				break;
			}
		}

		if (NearestActor)
		{
			FVector FishLocation = NearestActor->GetActorLocation();


			TargetLocation = FishLocation;
			StartLocation = GetActorLocation();

			SetActorTickEnabled(true);

			Time = GetWorld()->GetTimeSeconds();
			FTimerHandle TimerHandle;
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &AChainBubble::LerpProcess);
			GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.001f, true);
		}
	}
}

void AChainBubble::LerpProcess()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - Time;
	float LerpAlpha = FMath::Clamp(ElapsedTime / LerpDuration, 0.0f, 1.0f);
	LerpAlpha = UKismetMathLibrary::Ease(0, 1.f, LerpAlpha, EEasingFunc::EaseInOut);

	FVector NewLerpLocation = FMath::Lerp(StartLocation, TargetLocation, LerpAlpha);
	SetActorLocation(NewLerpLocation);

	if (LerpAlpha >= 1.0f)
	{
		ABubble* Bubble = GetWorld()->SpawnActor<ABubble>(ABubble::StaticClass(), NearestActor->GetActorLocation(), FRotator::ZeroRotator);
		
		GetWorldTimerManager().ClearAllTimersForObject(this);
		SetActorTickEnabled(false);
		LerpProcessCompleted();
	}
}

void AChainBubble::LerpProcessCompleted()
{
	MaxFish--;

	if (MaxFish > 0)
	{
		TeleportToFish();
		return;
	}

	Destroy();
}

void AChainBubble::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
	if (!bShouldCoolide) return;

	bShouldCoolide = false;

	if (Cast<ABubble>(OtherActor)) return;

	if (AFishActor* FishActor = Cast<AFishActor>(OtherActor))
	{
		TeleportToFish();
	}
}
