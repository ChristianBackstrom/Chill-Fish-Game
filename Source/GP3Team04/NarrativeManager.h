

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NarrativeManager.generated.h"

UCLASS()
class GP3TEAM04_API ANarrativeManager : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Inputs")
	UInputMappingContext* MiscMappingContext;

	UPROPERTY(EditAnywhere, Category="Inputs")
	UInputAction* NextLineAction;
	
public:	
	// Sets default values for this actor's properties
	ANarrativeManager();

	UPROPERTY(BlueprintReadWrite)
	bool bIsSoundPlaying = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDialogueActive;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// The time it takes between each letter
	UPROPERTY(EditAnywhere)
	float TypeOutSpeed;

	UPROPERTY(BlueprintReadOnly)
	USoundWave* CurrentSound;


	int CurrentLineIndex;

	UDialogue* CurrentDialogue;
	

private:
	UPROPERTY(EditAnywhere)
		float TimeDelayAfterLine = 3.f;
	
	UWorld* World;

	FString CurrentText;
	
	int TypedAmount = 100;

	
	bool bFinishedTyping = false;

	bool bIsTyping = false;

	float Time;
	bool TimerStarted = false;

protected:
	UFUNCTION(BlueprintCallable)
		void StartDialogue(UDialogue* Dialogue);

	UFUNCTION(BlueprintCallable)
		void NextLine();

	UFUNCTION()
		void UpdateText();

	UFUNCTION(BlueprintImplementableEvent)
		void StartedNextLine();

	UFUNCTION(BlueprintImplementableEvent)
		void EndLine();

	UFUNCTION(BlueprintImplementableEvent)
		void EndDialogue();

	UFUNCTION(BlueprintImplementableEvent)
		void DialogueStarted();

	UFUNCTION(BlueprintImplementableEvent)
		void TextUpdated(const FString& Text);

	UFUNCTION()
		void SkipLine(const FInputActionValue& ActionValue);

	UFUNCTION()
		void DelayAfterLineDone();
};
