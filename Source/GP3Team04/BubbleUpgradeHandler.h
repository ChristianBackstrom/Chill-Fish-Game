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
struct FObjective
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Score;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<ABubble>> AvailableBubbles;
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		TArray<FObjective> Objectives;

	AFishingGamemode* FishingGameMode;
	UFishingGameInstance* FishingGameInstance;
	ABubbleShooter* BubbleShooter;
	
	FObjective CurrentObjective;
	int CurrentIndex = 0;
public:
	UFUNCTION(BlueprintImplementableEvent)
		void ObjectiveCompleted(const TArray<TSubclassOf<ABubble>> &Bubbles);

protected:

	UFUNCTION(BlueprintCallable)
		void ChoosenBubble(TSubclassOf<ABubble> Bubble);

	bool bCheckForScore = true;
};
