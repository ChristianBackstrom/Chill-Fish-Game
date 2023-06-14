// Fill out your copyright notice in the Description page of Project Settings.


#include "NarrativeManager.h"
#include "Dialogue.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANarrativeManager::ANarrativeManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANarrativeManager::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();

	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(MiscMappingContext, 0);

			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
			{
				// Fire
				if (IsValid(NextLineAction))
				{
					EnhancedInputComponent->BindAction(NextLineAction, ETriggerEvent::Started, this,
					                                   &ANarrativeManager::SkipLine);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not cast Local player to UEnhancedInputLocalPlayerSubsystem"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not cast Controller to APlayerController"));
	}
}

void ANarrativeManager::StartDialogue(UDialogue* Dialogue)
{
	if (!IsValid(Dialogue))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Dialogue is a not assigned");

		return;
	}
	
	CurrentLineIndex = -1;
	CurrentDialogue = Dialogue;
	
	DialogueStarted();
	NextLine();
}

void ANarrativeManager::NextLine()
{
	CurrentLineIndex++;
	
	FString Text;
	USoundWave* Sound;

	if (CurrentDialogue->TryGetDialogueData(CurrentLineIndex, Sound, Text))
	{
		TypedAmount = Text.Len();
		CurrentText = Text;
		CurrentSound = Sound;

		StartedNextLine();

		bIsTyping = true;
		
		FTimerHandle TimerHandle;
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ANarrativeManager::UpdateText);
		GetWorldTimerManager().SetTimer(TimerHandle, Delegate, TypeOutSpeed, true, 0);

		return;
	}

	EndDialogue();
}

void ANarrativeManager::UpdateText()
{
	TypedAmount--;
	TypedAmount = FMath::Clamp(TypedAmount, 0, 1000000);
	
	if (bSkipLine && bIsTyping)
	{
		bFinishedTyping = true;
	}

	const FString& SubString = bFinishedTyping ? CurrentText : CurrentText.LeftChop(TypedAmount);

	TextUpdated(SubString);
	
	if (TypedAmount <= 0)
	{
		bFinishedTyping = true;
		bIsTyping = false;
	}
}

void ANarrativeManager::SkipLine(const FInputActionValue& ActionValue)
{
	bSkipLine = true;
	TypedAmount = 0;

	if (bFinishedTyping)
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
		bFinishedTyping = false;
		bSkipLine = false;
		bIsTyping = false;
		EndLine();
		NextLine();
	}
}
