// Fill out your copyright notice in the Description page of Project Settings.


#include "MagnetBubble.h"

#include "Kismet/GameplayStatics.h"

class AFishActor;

void AMagnetBubble::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ABubble>(OtherActor)) return;
	
	if (!bShouldCoolide) return;
	bShouldCoolide = false;
	bShouldMove = false;

	// create tarray for hit results
	TArray<FHitResult> OutHits;
	
	// start and end locations
	FVector SweepLocation = GetActorLocation();

	// create a collision sphere
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(SphereRadius);
	
	if (!GetWorld()->SweepMultiByChannel(OutHits, SweepLocation, SweepLocation, FQuat::Identity, ECC_WorldStatic, MyColSphere))
		return;
	
	for (FHitResult OutHit : OutHits)
	{
		if (AFishActor* FishActor = Cast<AFishActor>(OutHit.HitObjectHandle.FetchActor()))
		{
			FishActor->bShouldMove = false;
			FishActor->SetActorEnableCollision(false);
			FishToPull.Add(FishActor);
			FishStartPositions.Add(FishActor->GetActorLocation());
		}
	}
	
	FishCaught(nullptr);

	Time = UGameplayStatics::GetTimeSeconds(GetWorld());

	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &AMagnetBubble::PullFish);
	
	GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 0.01f, true);
}

void AMagnetBubble::PullFish()
{
	float TotalTime = FMath::Clamp((UGameplayStatics::GetTimeSeconds(GetWorld()) - Time) / DragTime, 0.f, 1.f);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::White, FString::SanitizeFloat(TotalTime));

	if (TotalTime >= 1.f)
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
		
		Time = UGameplayStatics::GetTimeSeconds(GetWorld());
		FTimerHandle TimerHandle;
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &AMagnetBubble::ReleaseFish);

		GetWorldTimerManager().SetTimer(TimerHandle, Delegate, .001f, false, 1.f);
		
		return;
	}

	for (int i = 0; i < FishToPull.Num(); i++)
	{
		FVector Target = FishStartPositions[i] - GetActorLocation();
		Target = GetActorLocation() + Target.GetSafeNormal() * TargetPullRadius;
		FVector Location = FMath::Lerp(FishStartPositions[i], Target, TotalTime);
		FishToPull[i]->SetActorLocation(Location);
	}
}

void AMagnetBubble::ReleaseFish()
{
	float TotalTime = FMath::Clamp((UGameplayStatics::GetTimeSeconds(GetWorld()) - Time) / HoldTime, 0.f, 1.f);

	if (TotalTime >= 1.f)
	{
		for (AFishActor* Fish : FishToPull)
		{
			Fish->bShouldMove = true;
			Fish->SetActorEnableCollision(true);
		}
		PopBubble();
	}
}

