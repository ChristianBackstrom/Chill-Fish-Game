// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "Dialogue.generated.h"

USTRUCT()
struct FDialogueData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Text;
	
	UPROPERTY(EditAnywhere)
	USoundCue* SoundCue;
};

/**
 * 
 */
UCLASS()
class GP3TEAM04_API UDialogue : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FDialogueData> DialogueDatas;

	bool TryGetDialogueData(int Index, USoundCue*& SoundCue, FString& Text);
};
