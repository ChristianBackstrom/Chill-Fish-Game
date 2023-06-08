// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueManager.h"

#include "Dialogue.h"

// Sets default values
ADialogueManager::ADialogueManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ADialogueManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADialogueManager::StartDialogue(UDialogue* Dialogue)
{
	CurrentIndex = 0;
	CurrentDialogue = Dialogue;

	DialogueStarted();
	USoundCue* SoundCue;
	FString Text;

	if (CurrentDialogue->TryGetDialogueData(CurrentIndex, SoundCue, Text))
	{
	}
}

