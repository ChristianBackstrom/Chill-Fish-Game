// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bubble.h"
#include "ChainBubble.generated.h"

/**
 * 
 */
UCLASS()
class GP3TEAM04_API AChainBubble : public ABubble
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	float LerpDuration;
	
	UPROPERTY()
	int MaxFish = 5;

	UPROPERTY()
	float Range = 10000.f;

	UPROPERTY()
	FVector NewLocation;

	UPROPERTY()
	TArray<AActor*> FoundActors;

	UPROPERTY()
	AActor* NearestActor;
	
	UFUNCTION()
	void TeleportToFish();
	void LerpProcess();
	void LerpProcessCompleted();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
