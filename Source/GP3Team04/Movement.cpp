// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"

// Sets default values
AMovement::AMovement()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AMovement::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void AMovement::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// There are ways to bind a UInputAction* to a handler function and multiple types of ETriggerEvent that may be of interest.

		// This calls the handler function on the tick when MoveAction triggers, such as when pressing an action button.
		if (IsValid(MoveAction))
		{
			PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovement::Move);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Move Action is not valid"));	
		}

		if (IsValid(LookAction))
		{
			PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMovement::Look);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Move Action is not valid"));	
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not cast playerInputComponent to UEnhancedInputComponent"));	
	}
}

void AMovement::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();
		
			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(InputMappingContext, 0);
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

void AMovement::Move(const FInputActionValue& ActionValue)
{
	// ActionValue.Get<FVector2d>().ToString()
	FVector2d Input = ActionValue.Get<FVector2d>();
	FVector forward = GetActorForwardVector() * Input.Y;
	FVector right = GetActorRightVector() * Input.X;
	
	AddMovementInput(forward + right);
}

void AMovement::Look(const FInputActionValue& ActionValue)
{
	FVector2d Input = ActionValue.Get<FVector2d>();

	AddControllerPitchInput(Input.Y);
	AddControllerYawInput(Input.X);

}




