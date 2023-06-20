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
	PrimaryActorTick.bCanEverTick = false;
	
}

// Called when the game starts or when spawned
void ABubbleUpgradeHandler::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentIndex = 0;
	if (Objectives.Num() > 0)
		CurrentObjective = Objectives[CurrentIndex];
	
	FishingGameMode = Cast<AFishingGamemode>(UGameplayStatics::GetGameMode(GetWorld()));
	FishingGameInstance = Cast<UFishingGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	BubbleShooter = Cast<ABubbleShooter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABubbleShooter::StaticClass()));

	if (IsValid(FishingGameInstance))
		FishingGameInstance->OnFishCaught.AddUObject(this, &ABubbleUpgradeHandler::FishCaught);

	BlueprintBeginplay();
}

void ABubbleUpgradeHandler::ChoosenBubble(TSubclassOf<ABubble> Bubble)
{
	BubbleShooter->AddUpgradedBubble(Bubble);
	FishingGameInstance->UnlockedBubbles.AddUnique(Bubble);
}

void ABubbleUpgradeHandler::FishCaught(const FFish& Fish)
{
	CurrentObjective.FishCaught(Fish);

	UpdateObjective();
	
	if (CurrentObjective.Completed())
	{
		TArray<TSubclassOf<ABubble>> Bubbles = CurrentObjective.AvailableBubbles;
		TArray<TSubclassOf<ABubble>> ValidBubbles = Bubbles;
		for (TSubclassOf<ABubble> Bubble : Bubbles)
		{
			if (BubbleShooter->UpgradedBubbles.Contains(Bubble))
			{
				ValidBubbles.Remove(Bubble);
			}
		}

		Bubbles = ValidBubbles;
		
		CurrentIndex++;
		if (Bubbles.Num() > 0)
			ObjectiveCompleted(Bubbles);
		

		if (CurrentIndex >= Objectives.Num())
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, "NO MORE OBJECTIVES");

			return;
		}

		CurrentObjective = Objectives[CurrentIndex];
		NextObjective(CurrentObjective);
	}
}



