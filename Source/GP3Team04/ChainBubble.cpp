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

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	
	World->SweepMultiByObjectType(HitResults, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, CollisionShape);


	float Length = 100000000.f;
	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (AFishActor* FishActor = Cast<AFishActor>(HitActor))
		{
			float distance = FVector::Distance(HitActor->GetActorLocation(), GetActorLocation());

			if (FishActorsCaught.Contains(FishActor)) continue;

			if (distance < Length && FishActor->bEnabled)
			{
				Length = distance;
				NearestActor = FishActor;
			}
		}
	}

	if (NearestActor)
	{
		FishActorsCaught.AddUnique(NearestActor);
		
		StartLocation = GetActorLocation();
		StartScale = GetActorScale3D();
		
		TargetLocation = NearestActor->BubbleMesh->GetComponentLocation();
		TargetScale = NearestActor->BubbleMesh->GetComponentScale();
		NearestActor->bShouldMove = false;
		
		Time = World->GetTimeSeconds();
		FTimerHandle TimerHandle;
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &AChainBubble::LerpProcess);
		GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.001f, true);
		return;
	}

	PopBubble();
}

void AChainBubble::LerpProcess()
{
	float CurrentTime = World->GetTimeSeconds();
	float ElapsedTime = CurrentTime - Time;
	float LerpAlpha = FMath::Clamp(ElapsedTime / LerpDuration, 0.0f, 1.0f);
	LerpAlpha = UKismetMathLibrary::Ease(0, 1.f, LerpAlpha, EEasingFunc::EaseInOut);

	FVector LerpedLocation = FMath::Lerp(StartLocation, TargetLocation, LerpAlpha);
	FVector LerpedScale = FMath::Lerp(StartScale, TargetScale, LerpAlpha);
	
	SetActorLocation(LerpedLocation);
	SetActorScale3D(LerpedScale);

	if (LerpAlpha >= 1.0f)
	{
		ABubble* Bubble = World->SpawnActor<ABubble>(DefaultBubble, NearestActor->BubbleMesh->GetComponentLocation(), NearestActor->BubbleMesh->GetComponentRotation());
		Bubble->CaptureSpeed = 0.0000001f;
		Bubble->SetActorScale3D(GetActorScale3D());
		Bubble->bIgnoreSize = true;
		
		Bubble->CatchFish(NearestActor);

		int index = FMath::Clamp(ChainSounds.Num() - 1 - MaxFish, 0, 100000);
		if (index < ChainSounds.Num())
		{
			Bubble->CaughtSoundEffect = ChainSounds[index];
		}
		
		GetWorldTimerManager().ClearAllTimersForObject(this);
		LerpProcessCompleted();
	}
}

void AChainBubble::LerpProcessCompleted()
{
	MaxFish--;
	NearestActor = nullptr;

	if (MaxFish > 0)
	{
		TeleportToFish();
		return;
	}

	PopBubble();
}

void AChainBubble::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
	if (Cast<ABubble>(OtherActor)) return;
	if (!bShouldCoolide) return;

	bShouldCoolide = false;
	bShouldMove = false;


	if (AFishActor* FishActor = Cast<AFishActor>(OtherActor))
	{
		TeleportToFish();
		return;
	}

	PopBubble();
}
