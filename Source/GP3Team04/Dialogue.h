// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "Dialogue.generated.h"

USTRUCT(BlueprintType)
struct FDialogueData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Text;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundWave* SoundCue;
};

/**
 * 
 */
UCLASS()
class GP3TEAM04_API UDialogue : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDialogueData> DialogueDatas;

	bool TryGetDialogueData(int Index, USoundWave*& SoundCue, FString& Text);
};
