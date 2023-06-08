// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogueManager.generated.h"

class UDialogue;
UCLASS()
class GP3TEAM04_API ADialogueManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADialogueManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TArray<UDialogue*> Dialogues;

	int DialogueIndex = 0;

	int CurrentIndex;

	UDialogue* CurrentDialogue;

protected:
	UFUNCTION(BlueprintCallable)
		void StartDialogue(UDialogue* Dialogue);

	// UFUNCTION(BlueprintCallable)
	// 	void NextLine(FString Text, USoundCue* SoundCue);

	UFUNCTION(BlueprintImplementableEvent)
		void DialogueStarted();
};
