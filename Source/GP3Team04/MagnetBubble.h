// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bubble.h"
#include "MagnetBubble.generated.h"

/**
 * 
 */
UCLASS()
class GP3TEAM04_API AMagnetBubble : public ABubble
{
	GENERATED_BODY()

protected:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UFUNCTION()
		void PullFish();

	UFUNCTION()
		void ReleaseFish();
private:
	//The total radius of the magnet
	UPROPERTY(EditAnywhere)
		float SphereRadius = 500.f;

	//The radius around the bubble the fishes will be positioned at.
	UPROPERTY(EditAnywhere)
		float TargetPullRadius = 100.f;

	//Time it takes to drag the fish to the target position
	UPROPERTY(EditAnywhere)
		float DragTime = 1.f;

	//The time it keeps the fishes after it dragged them. When this time runs out the fishes are released
	UPROPERTY(EditAnywhere)
		float HoldTime = 1.f;

	FTimerHandle TimerHandle;
	

	TArray<AFishActor*> FishToPull;
	TArray<FVector> FishStartPositions;

	float Time;
	
};
