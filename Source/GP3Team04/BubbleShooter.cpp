// Fill out your copyright notice in the Description page of Project Settings.

#include "BubbleShooter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameHud.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
ABubbleShooter::ABubbleShooter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
}


// Called when the game starts
void ABubbleShooter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(DefaultBubble))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, "No default bubble was provided so no mappings will be connected");

		return;
	}

	// Will supply the purchased bubbles to the bubble shooter
	// if (UFishingGameInstance* FishingGameInstance = Cast<UFishingGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	// {
	// 	UpgradedBubbles = FishingGameInstance->UnlockedBubbles;
	// }

	if (UpgradedBubbles.Num() <= 0)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, "The upgraded bubbles list is empty, Default bubble will be used instead");
	}
	else
	{
		CurrentIndex = 0;
		SelectedBubble = UpgradedBubbles[0];
	}


	for (TSubclassOf<ABubble> Bubble : UpgradedBubbles)
	{
		ABubble* Standardbubble = Bubble->GetDefaultObject<ABubble>();

		Cooldowns.Add(Standardbubble->Cooldown);
		Timers.Add(0.f);
	}

	CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(FireMappingContext, 0);

			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
			{
				// Fire
				if (IsValid(FireAction))
				{
					EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABubbleShooter::Charge, true);
					
					EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABubbleShooter::Fire, true);
				}

				if (IsValid(UpgradedFireAction))
				{
					EnhancedInputComponent->BindAction(UpgradedFireAction, ETriggerEvent::Started, this, &ABubbleShooter::Charge, false);
					
					EnhancedInputComponent->BindAction(UpgradedFireAction, ETriggerEvent::Completed, this, &ABubbleShooter::Fire, false);
				}

				if (IsValid(ChangeOfBubbleAction))
				{
					EnhancedInputComponent->BindAction(ChangeOfBubbleAction, ETriggerEvent::Started, this, &ABubbleShooter::ChangeBubble);
				}

				if(IsValid(ScrollBubbleAction))
				{
					EnhancedInputComponent->BindAction(ScrollBubbleAction, ETriggerEvent::Started, this, &ABubbleShooter::Scroll);
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

void ABubbleShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < Timers.Num(); ++i)
	{
		if (Timers[i] <= 0) continue;
		Timers[i] -= DeltaTime;
	}
}


// Removes fire mapping context from the input subsystem
void ABubbleShooter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void ABubbleShooter::Charge(const FInputActionValue& ActionValue, bool bStandardBubble)
{
	if (!bStandardBubble && UpgradedBubbles.Num() <= 0) return;
	
	if (UpgradedBubbles.Num() > 0)
		if (SelectedBubble->GetDefaultObject<ABubble>()->bUseCooldown && !bStandardBubble)
		{
			if (Timers[CurrentIndex] > 0)
			{
				FString string = "There is ";
				string.Append(FString::SanitizeFloat(Timers[CurrentIndex]));
				string.Append(" seconds left of cooldown");
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, string);
				return;
			}
		}
	
	
	if (bIsDefaultCharging || bIsUpgradedCharging) return;
	
	bIsDefaultCharging = bStandardBubble;
	bIsUpgradedCharging = !bStandardBubble;

	
	FTransform Transform;
	FRotator CameraRotation = CameraManager->GetCameraRotation();


	if (!bIsAimingDown)
	{
		StartAim(ActionValue);
	}

	FVector Location = GetActorLocation() + CameraManager->GetCameraRotation().RotateVector(FirePoint);
	
	Transform.SetLocation(Location);
	Transform.SetRotation(CameraRotation.Quaternion());

	TSubclassOf<ABubble> Bubble = bStandardBubble ? DefaultBubble : SelectedBubble;
	
	ChargingBubble = GetWorld()->SpawnActor<ABubble>(Bubble, Transform);
	if (IsValid(ChargingBubble))
	{
		ChargingBubble-> SetActorScale3D(FVector::Zero());
		ChargingBubble-> bShouldMove = false;
		ChargingBubble-> MaxLifeTime = LifeTime;
		ChargingBubble-> LifeTime = LifeTime;
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
		ChargingBubble-> AttachToActor(this, TransformRules);
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate = FTimerDelegate::CreateUObject(this, &ABubbleShooter::Charging);
	
	fTimeOfCharge = UGameplayStatics::GetTimeSeconds(GetWorld());
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, .0001f, true);

	StartCharging();
}


void ABubbleShooter::Fire(const FInputActionValue& ActionValue, bool bStandardBubble)
{
	if (!IsValid(ChargingBubble))
	{
		bIsDefaultCharging = false;    
		bIsUpgradedCharging = false;
		if (!bIsAimingDown)                    
		{                                      
			StopAim(ActionValue);              
		}                                      
	}
	
	if (bStandardBubble && !bIsDefaultCharging) return;

	if (!bStandardBubble && !bIsUpgradedCharging) return;

	if (ChargingBubble->bUseCooldown && StartTime > 0.f) return;

	GetWorldTimerManager().ClearAllTimersForObject(this);
	
	bIsDefaultCharging = false;
	bIsUpgradedCharging = false;
	
	if (!bIsAimingDown)
	{
		StopAim(ActionValue);
	}

	StopCharging();
	if (UGameplayStatics::GetTimeSeconds(GetWorld()) - fTimeOfCharge < MinChargeTime)
	{
		ChargingBubble->Destroy();
		return;
	}

	float Size = FMath::Clamp((UGameplayStatics::GetTimeSeconds(GetWorld()) - fTimeOfCharge) / ChargeTime, 0, 1);

	if (bUseRaycastSystem)
	{

		FHitResult Hit;
		FVector Start = CameraManager->GetCameraLocation();
		FVector End = Start + CameraManager->GetActorForwardVector() * (BubbleBaseSpeed * BubbleMaxSpeedDecrease / Size * LifeTime);
		
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

		End = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
		
		GetWorld()->LineTraceSingleByChannel(Hit, ChargingBubble->GetActorLocation(), End, ECC_Camera);

		End = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
		
		ChargingBubble->SetActorRotation((End - ChargingBubble->GetActorLocation()).Rotation());
	}

	if (IsValid(ChargingBubble))
	{
		ChargingBubble-> bShouldMove = true;
		ChargingBubble->Size = Size;
		ChargingBubble->Speed = BubbleBaseSpeed * BubbleMaxSpeedDecrease / Size;
		ChargingBubble->baseSpeed = ChargingBubble->Speed;
		ChargingBubble->bShouldCoolide = true;

		FDetachmentTransformRules DetachmentTransformRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		ChargingBubble->DetachFromActor(DetachmentTransformRules);
		
		Fired();//Tytus added this

		if (ChargingBubble->bUseCooldown)
			Timers[CurrentIndex] = Cooldowns[CurrentIndex];
	}
}

void ABubbleShooter::Charging()
{
	FVector Scale = FVector::One();
	float Time = (UGameplayStatics::GetTimeSeconds(GetWorld()) - fTimeOfCharge) / ChargeTime;
	Scale *= FMath::Clamp(Time, 0, 1) * MaxSizeBubble;
	Scale += FVector::One() / 30 * FMath::Sin((UGameplayStatics::GetTimeSeconds(GetWorld()) - fTimeOfCharge) * 5);


	if (IsValid(ChargingBubble))
	{
		ChargingBubble->SetActorRelativeScale3D(Scale);

		// FVector Location = CameraManager->GetCameraLocation() + CameraManager->GetCameraRotation().RotateVector(FirePoint);
		// ChargingBubble->SetActorLocation(Location + CameraManager->GetActorForwardVector() * Scale.Size());
		
		ChargingBubble->SetActorRotation(CameraManager->GetCameraRotation());
	}
}

void ABubbleShooter::ChangeBubble(int32 Index)
{
	if (UpgradedBubbles.Num() <= 0) return;
	
	if(Index < 0)
	{
		Index = UpgradedBubbles.Num() - 1;
	}
	
	
	SelectedBubble = UpgradedBubbles[Index];
	CurrentIndex = Index;

	ChangedBubble(CurrentIndex);
}

int ABubbleShooter::GetWhatNumberWasPressed()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	if(PlayerController->WasInputKeyJustPressed("One")) return 1;
	if(PlayerController->WasInputKeyJustPressed("Two")) return 2;
	if(PlayerController->WasInputKeyJustPressed("Three")) return 3;
	if(PlayerController->WasInputKeyJustPressed("Four")) return 4;
	if(PlayerController->WasInputKeyJustPressed("Five")) return 5;
	if(PlayerController->WasInputKeyJustPressed("Six")) return 6;
	if(PlayerController->WasInputKeyJustPressed("Seven")) return 7;
	if(PlayerController->WasInputKeyJustPressed("Eight")) return 8;
	if(PlayerController->WasInputKeyJustPressed("Nine")) return 9;
	
	return 0;
}

void ABubbleShooter::Scroll(const FInputActionValue& ActionValue)
{
	if (UpgradedBubbles.Num() <= 0) return;
	
	float Input = ActionValue.Get<float>();

	int32 IntInput = FMath::RoundToInt32(Input);
	
	int32 NextIndex = (CurrentIndex + IntInput ) % UpgradedBubbles.Num();
	ChangeBubble(NextIndex);
}

void ABubbleShooter::ChangeBubble()
{
	if (UpgradedBubbles.Num() <= 0) return;
	
	int Number = GetWhatNumberWasPressed();
	
	Number = FMath::Clamp(Number - 1, 0, UpgradedBubbles.Num() - 1);

	SelectedBubble = UpgradedBubbles[Number];
	CurrentIndex = Number;

	ChangedBubble(CurrentIndex);
}

void ABubbleShooter::AddUpgradedBubble(TSubclassOf<ABubble> Bubble)
{
	ABubble* InstantiatedBubble = Bubble.GetDefaultObject();

	UpgradedBubbles.Add(Bubble);
	Timers.Add(0.f);
	Cooldowns.Add(InstantiatedBubble->Cooldown);

	AddedUpgradedBubble(Bubble);

	if (UpgradedBubbles.Num() == 1)
	{
		CurrentIndex = 0;
		SelectedBubble = UpgradedBubbles[CurrentIndex];
	}
}
