// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FishingGameInstance.generated.h"

class AFishActor;
class ABubble;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFishCaught, const FFish&);

UENUM(BlueprintType)
enum FishSize
{
	Small        UMETA(DisplayName="Small"),
	Large        UMETA(DisplayName="Large"),
};

UENUM(BlueprintType)
enum FishType
{
	Angelfish,
	Bigeye,
	Clownfish,
	Cowfish,
	Devilray,
	Rockfish,
	Octopus,
	Crab,
	Humpback,
	Jellyfish,
	Lionfish,
	Longnose,
	Pufferfish,
	Seabunny,
	Seahorse,
	GoldFish,
	AnyFish,
};

USTRUCT(BlueprintType)
struct FFish
{
	GENERATED_BODY()

	FFish()
	{
		Size = Small;
		Type = AnyFish;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FishSize> Size;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FishType> Type;
};


UCLASS()
class GP3TEAM04_API UFishingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void CaughtFish(const FFish& Fish);

	virtual void Init() override;
	
public:
	TArray<FFish> CaughtFishies;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<ABubble>> UnlockedBubbles = TArray<TSubclassOf<ABubble>>();

	UPROPERTY(BlueprintReadWrite)
		int Score;

	FOnFishCaught OnFishCaught;
};




