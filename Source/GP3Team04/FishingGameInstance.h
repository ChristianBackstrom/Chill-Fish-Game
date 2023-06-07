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
	Rockfish,
	Clownfish,
	Cowfish,
	Octopus,
	Crab,
	Humpback,
	Jellyfish,
	Lionfish,
	Pufferfish,
	Seabunny,
	Bigeye,
	GoldFish,
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

	UPROPERTY(EditAnywhere)
	UTexture2D* FishImage;
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

	FOnFishCaught OnFishCaught;
};




