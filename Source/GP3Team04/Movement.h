// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Movement.generated.h"

UCLASS()
class GP3TEAM04_API AMovement : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputAction* LookAction;
	
public:
	// Sets default values for this character's properties
	AMovement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void PawnClientRestart() override;

private:
	void Move(const FInputActionValue& ActionValue);
	void Look(const FInputActionValue& ActionValue);

};
