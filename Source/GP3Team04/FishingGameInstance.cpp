// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingGameInstance.h"

void UFishingGameInstance::CaughtFish(const FFish& Fish)
{
	CaughtFishies.Add(Fish);
	OnFishCaught.Broadcast(Fish);
}

void UFishingGameInstance::Init()
{
	Super::Init();

	UnlockedBubbles = TArray<TSubclassOf<ABubble>>();
}
