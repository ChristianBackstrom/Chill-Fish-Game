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
	UPROPERTY(EditAnywhere, Category="Config", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float ExplodingMaxSize = .5f;


#pragma region Expanding
	UPROPERTY(EditAnywhere, Category="Expanding")
		float ExpandRadiusMultiply = 3.f;

	UPROPERTY(EditAnywhere, Category="Expanding")
		float ExpandTime = 1.f;

	UPROPERTY(EditAnywhere, Category="Expanding")
		int MaxFish = 10;


	int FishiesCaught = 0;

	FVector BaseScale;

	bool bExpandStarted;

	TArray<AFishActor*> CaughtFish;

#pragma endregion These are the variables used for the expanding bubble

#pragma region Exploding
	UPROPERTY(EditAnywhere, Category="Exploding")
		TSubclassOf<ABubble> BubbleToSpawn;
	
	UPROPERTY(EditAnywhere, Category="Exploding")
		int MaxAmount = 10;

	UPROPERTY(EditAnywhere, Category="Exploding")
		int MinAmount = 5;
#pragma endregion These are the variables used for the exploding bubble
	
	float Time;

protected:

	UFUNCTION()
		void StartExpand(AFishActor* FishActor);

	UFUNCTION(BlueprintImplementableEvent)
		void ExpandStarted();

	
	UFUNCTION()
		void Expand();

	UFUNCTION()
		void ExpandEnded();

	UFUNCTION(BlueprintCallable)
		void Explode();

	UFUNCTION(BlueprintImplementableEvent)
		void Exploded();
	
};