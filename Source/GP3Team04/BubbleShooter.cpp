// Fill out your copyright notice in the Description page of Project Settings.

#include "BubbleShooter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
ABubbleShooter::ABubbleShooter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	SetRootComponent(StaticMeshComponent);
}


// Called when the game starts
void ABubbleShooter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(DefaultBubble))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red,"No default bubble was provided so no mappings will be connected");

		return;
	}

	// // Will supply the purchased bubbles to the bubble shooter
	if (UFishingGameInstance* FishingGameInstance = Cast<UFishingGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		if (FishingGameInstance->UnlockedBubbles.Num() > 0)
		{
			UpgradedBubbles.Empty();
			for (auto Bubble : FishingGameInstance->UnlockedBubbles)
			{
				if (!IsValid(Bubble)) continue;

				UpgradedBubbles.Add(Bubble);
			}
		}
	}

	if (UpgradedBubbles.Num() > 0)
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
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(FireMappingContext, 0);

			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent))
			{
				// Fire
				if (IsValid(FireAction))
				{
					EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this,
					                                   &ABubbleShooter::Charge, true);

					EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this,
					                                   &ABubbleShooter::Fire, true);
				}

				if (IsValid(UpgradedFireAction))
				{
					EnhancedInputComponent->BindAction(UpgradedFireAction, ETriggerEvent::Started, this,
					                                   &ABubbleShooter::Charge, false);

					EnhancedInputComponent->BindAction(UpgradedFireAction, ETriggerEvent::Completed, this,
					                                   &ABubbleShooter::Fire, false);
				}

				if (IsValid(ChangeOfBubbleAction))
				{
					EnhancedInputComponent->BindAction(ChangeOfBubbleAction, ETriggerEvent::Started, this,
					                                   &ABubbleShooter::ChangeBubble);
				}

				if (IsValid(ScrollBubbleAction))
				{
					EnhancedInputComponent->BindAction(ScrollBubbleAction, ETriggerEvent::Started, this,
					                                   &ABubbleShooter::Scroll);
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

	BlueprintBeginPlay();
}

void ABubbleShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < Timers.Num(); ++i)
	{
		if (Timers[i] <= 0) continue;
		Timers[i] -= DeltaTime;
	}

	if (IsValid(BoatActor))
	{
		FVector Location =  BoatActor->GetActorLocation();
		Velocity = LastPosition - Location;
		LastPosition = Location;
	}
}


// Removes fire mapping context from the input subsystem
void ABubbleShooter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void ABubbleShooter::Charge(const FInputActionValue& ActionValue, bool bStandardBubble)
{
	if (!bShouldShoot) return;
	if (!bStandardBubble && UpgradedBubbles.Num() <= 0) return;

	if (UpgradedBubbles.Num() > 0)
	{
		if (SelectedBubble->GetDefaultObject<ABubble>()->bUseCooldown && !bStandardBubble)
		{
			if (Timers[CurrentIndex] > 0) return;
		}
	}


	if (bIsDefaultCharging || bIsUpgradedCharging) return;

	bIsDefaultCharging = bStandardBubble;
	bIsUpgradedCharging = !bStandardBubble;



	FRotator CameraRotation = CameraManager->GetCameraRotation();


	if (!bIsAimingDown)
	{
		StartAim(ActionValue);
	}

	FVector Location = GetActorLocation() + CameraManager->GetCameraRotation().RotateVector(FirePoint);

	FTransform Transform;
	Transform.SetLocation(Location);
	Transform.SetRotation(CameraRotation.Quaternion());

	TSubclassOf<ABubble> Bubble = bStandardBubble ? DefaultBubble : SelectedBubble;

	ChargingBubble = GetWorld()->SpawnActor<ABubble>(Bubble, Transform);
	if (IsValid(ChargingBubble))
	{
		ChargingBubbleType = Bubble;
		ChargingBubble->SetActorScale3D(FVector::Zero());
		ChargingBubble->bShouldMove = false;
		ChargingBubble->MaxLifeTime = LifeTime;
		ChargingBubble->LifeTime = LifeTime;
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
		ChargingBubble->AttachToActor(this, TransformRules);
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate = FTimerDelegate::CreateUObject(this, &ABubbleShooter::Charging);

	fTimeOfCharge = UGameplayStatics::GetTimeSeconds(GetWorld());
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, .0001f, true);

	StartCharging(bIsDefaultCharging);
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

	if (IsValid(ChargingBubble))
	{
		ChargingBubble->bShouldMove = true;
		ChargingBubble->Size = Size;
		ChargingBubble->Speed = BubbleBaseSpeed * BubbleMaxSpeedDecrease / Size;
		ChargingBubble->baseSpeed = ChargingBubble->Speed;
		ChargingBubble->bShouldCoolide = true;
		Velocity = -Velocity;
		ChargingBubble->IntertialVelocity = Velocity;

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1 , 10.f, FColor::White, Velocity.ToString());

		// Detach from bubble shooter
		FDetachmentTransformRules DetachmentTransformRules =
			FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		ChargingBubble->DetachFromActor(DetachmentTransformRules);

		Fired();
		if (ChargingBubble->bUseCooldown)
		{
			int Index = UpgradedBubbles.Find(ChargingBubbleType);

			if (Index >= 0)
				Timers[Index] = Cooldowns[Index];
		}
		
		TArray<FHitResult> HitResults;
		FVector TraceStart = ChargingBubble->GetActorLocation();
		FVector TraceEnd = TraceStart + FVector::UpVector * Size * 100.f;
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(Size * 50.f);
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	
		GetWorld()->SweepMultiByObjectType(HitResults, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, CollisionShape);
		
		if (HitResults.Num() > 0)
		{
			AFishActor* NearestFish = Cast<AFishActor>(HitResults[0].GetActor());
			float Length = 10000000.f;
			for (const FHitResult& HitResult : HitResults)
			{
				AActor* HitActor = HitResult.GetActor();
				if (AFishActor* FishActor = Cast<AFishActor>(HitActor))
				{
					float distance = FVector::Distance(HitActor->GetActorLocation(), GetActorLocation());
					
					if (distance < Length)
					{
						Length = distance;
						NearestFish = FishActor;
					}
				}
			}

			if (IsValid(NearestFish))
				ChargingBubble->CatchFish(NearestFish);
		}
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
		ChargingBubble->SetActorRotation(CameraManager->GetCameraRotation());
	}
}

void ABubbleShooter::ChangeBubble(int32 Index)
{
	if (UpgradedBubbles.Num() <= 0) return;

	if (Index < 0)
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

	if (PlayerController->WasInputKeyJustPressed("One")) return 1;
	if (PlayerController->WasInputKeyJustPressed("Two")) return 2;
	if (PlayerController->WasInputKeyJustPressed("Three")) return 3;
	if (PlayerController->WasInputKeyJustPressed("Four")) return 4;
	if (PlayerController->WasInputKeyJustPressed("Five")) return 5;
	if (PlayerController->WasInputKeyJustPressed("Six")) return 6;
	if (PlayerController->WasInputKeyJustPressed("Seven")) return 7;
	if (PlayerController->WasInputKeyJustPressed("Eight")) return 8;
	if (PlayerController->WasInputKeyJustPressed("Nine")) return 9;

	return 0;
}

void ABubbleShooter::Scroll(const FInputActionValue& ActionValue)
{
	if (UpgradedBubbles.Num() <= 0) return;

	float Input = ActionValue.Get<float>();

	int32 IntInput = FMath::RoundToInt32(Input);

	int32 NextIndex = (CurrentIndex + IntInput) % UpgradedBubbles.Num();
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

	if (UpgradedBubbles.Contains(Bubble)) return;

	UpgradedBubbles.Add(Bubble);
	Timers.Add(0.f);
	Cooldowns.Add(InstantiatedBubble->Cooldown);

	AddedUpgradedBubble(Bubble);

	if (UFishingGameInstance* FishingGameInstance = Cast<UFishingGameInstance>(
		UGameplayStatics::GetGameInstance(GetWorld())))
	{
		for (auto UnlockedBubble : UpgradedBubbles)
		{
			FishingGameInstance->UnlockedBubbles.AddUnique(UnlockedBubble);
		}
	}

	if (UpgradedBubbles.Num() == 1)
	{
		CurrentIndex = 0;
		SelectedBubble = UpgradedBubbles[CurrentIndex];
	}
}
