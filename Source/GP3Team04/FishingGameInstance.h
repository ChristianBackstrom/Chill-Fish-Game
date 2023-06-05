// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FishingGameInstance.generated.h"

class ABubble;

UENUM(BlueprintType)
enum FishSize
{
	Small        UMETA(DisplayName="Small"),
	Large        UMETA(DisplayName="Large"),
};

UENUM(BlueprintType)
enum FishType
{
	Clownfish,
	Cowfish,
	Octopus,
	Crab,
	Humpbackfish,
	Jellyfish,
	Lionfish,
	Pufferfish,
	Seabunny,
	Smallfish,
	Upgradefish,
};

USTRUCT(BlueprintType)
struct FFish
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<FishSize> Size;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<FishType> Type;	
};


UCLASS()
class GP3TEAM04_API UFishingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void CaughtFish(const FFish& Fish);
	
public:
	TArray<FFish> CaughtFishies;

	TArray<TSubclassOf<ABubble>> UnlockedBubbles;

	UPROPERTY(BlueprintReadWrite)
		int Score;
};




