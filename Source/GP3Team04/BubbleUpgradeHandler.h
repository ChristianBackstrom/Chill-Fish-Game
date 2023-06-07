// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bubble.h"
#include "GameFramework/Actor.h"
#include "BubbleUpgradeHandler.generated.h"

class ABubbleShooter;
class AFishingGamemode;
class UFishingGameInstance;

USTRUCT(BlueprintType)
struct FFishAmount
{
	GENERATED_BODY()

	FFishAmount()
	{
		Type = Rockfish;
		NeededAmount = 1;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FishType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int NeededAmount;

	bool Completed() const
	{
		return NeededAmount <= 0;
	}
};

USTRUCT(BlueprintType)
struct FObjective
{
	GENERATED_BODY()

	FObjective()
	{
		FishToCatch = TArray<FFishAmount>();
		AvailableBubbles = TArray<TSubclassOf<ABubble>>();
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FFishAmount> FishToCatch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ABubble>> AvailableBubbles;

	bool Completed() const
	{
		for (FFishAmount FishAmount : FishToCatch)
		{
			if (FishAmount.Completed()) continue;
			return false;
		}
		return true;
	}


	void FishCaught(const FFish& Fish)
	{
		FishType Type = Fish.Type;

		for (int i = 0; i < FishToCatch.Num(); ++i)
		{
			if (FishToCatch[i].Type != Type) continue;
			
			FishToCatch[i].NeededAmount--;
		}
	}
};

UCLASS()
class GP3TEAM04_API ABubbleUpgradeHandler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABubbleUpgradeHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere)
	TArray<FObjective> Objectives;

	UPROPERTY(BlueprintReadOnly)
	FObjective CurrentObjective;

	int CurrentIndex = 0;

	AFishingGamemode* FishingGameMode;
	UFishingGameInstance* FishingGameInstance;
	ABubbleShooter* BubbleShooter;

public:
	UFUNCTION(BlueprintImplementableEvent)
		void ObjectiveCompleted(const TArray<TSubclassOf<ABubble>>& Bubbles);

	void FishCaught(const FFish& Fish);

protected:
	UFUNCTION(BlueprintCallable)
	void ChoosenBubble(TSubclassOf<ABubble> Bubble);
};
