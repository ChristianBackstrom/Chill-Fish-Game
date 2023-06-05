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

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABubble> DefaultBubble;
	
	UPROPERTY(EditAnywhere)
	int MaxFish = 5;

	UPROPERTY(EditAnywhere)
	float Range = 300.f;

	UPROPERTY()
	FVector NewLocation;

	UPROPERTY()
	TArray<AActor*> FoundActors;

	UPROPERTY()
	AFishActor* NearestActor;

private:
	TArray<AFishActor*> FishActorsCaught;

protected:
	
	UFUNCTION()
	void TeleportToFish();

	UFUNCTION()
	void LerpProcess();

	UFUNCTION()
	void LerpProcessCompleted();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
