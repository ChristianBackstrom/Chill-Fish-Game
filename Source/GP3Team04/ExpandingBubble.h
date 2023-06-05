// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bubble.h"
#include "ExpandingBubble.generated.h"

/**
 * 
 */
UCLASS()
class GP3TEAM04_API AExpandingBubble : public ABubble
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere)
		float ExpandRadiusMultiply = 3.f;

	UPROPERTY(EditAnywhere)
		float ExpandTime = 1.f;

	float Time;

	FVector BaseScale;

	bool bExpandStarted;

	TArray<AFishActor*> CaughtFish;

	UFUNCTION()
		void Expand();

	UFUNCTION()
		void ExpandEnded();
};
