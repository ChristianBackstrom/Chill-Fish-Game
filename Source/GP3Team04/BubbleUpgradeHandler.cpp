// Fill out your copyright notice in the Description page of Project Settings.


#include "BubbleUpgradeHandler.h"

#include "BubbleShooter.h"
#include "FishingGamemode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABubbleUpgradeHandler::ABubbleUpgradeHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABubbleUpgradeHandler::BeginPlay()
{
	Super::BeginPlay();

	if (Objectives.Num() > 0)
		CurrentObjective = Objectives[0];
	
	FishingGameMode = Cast<AFishingGamemode>(UGameplayStatics::GetGameMode(GetWorld()));
	FishingGameInstance = Cast<UFishingGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	BubbleShooter = Cast<ABubbleShooter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABubbleShooter::StaticClass()));
}

// Called every frame
void ABubbleUpgradeHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(FishingGameMode) || !bCheckForScore) return;
	
	if (FishingGameMode->Score >= CurrentObjective.Score)
	{
		bCheckForScore = false;
		TArray<TSubclassOf<ABubble>> Bubbles = CurrentObjective.AvailableBubbles;

		for (TSubclassOf<ABubble> Bubble : Bubbles)
		{
			if (BubbleShooter->UpgradedBubbles.Contains(Bubble))
			{
				Bubbles.Remove(Bubble);
			}
		}
		if (Bubbles.Num() > 0)
			ObjectiveCompleted(Bubbles);
		if (CurrentIndex >= Objectives.Num() - 1) return;
		CurrentIndex++;
		CurrentObjective = Objectives[CurrentIndex];
	}
}

void ABubbleUpgradeHandler::ChoosenBubble(TSubclassOf<ABubble> Bubble)
{
	BubbleShooter->AddUpgradedBubble(Bubble);
	FishingGameInstance->UnlockedBubbles.AddUnique(Bubble);
}


