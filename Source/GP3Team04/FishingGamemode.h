// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FishingGamemode.generated.h"

/**
 * 
 */
UCLASS()
class GP3TEAM04_API AFishingGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void FishCaught(const TArray<int> &FishValue,const TArray<float> &FishSize);

	UPROPERTY(BlueprintReadWrite)
		int Score;
};
