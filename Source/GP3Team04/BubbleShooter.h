// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bubble.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "BubbleShooter.generated.h"


UCLASS()
class GP3TEAM04_API ABubbleShooter : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputMappingContext* FireMappingContext;

	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputAction* UpgradedFireAction;

	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputAction* ChangeOfBubbleAction;

	UPROPERTY(EditAnywhere, Category="Inputs")
		UInputAction* ScrollBubbleAction;

public:	
	// Sets default values for this component's properties
	ABubbleShooter();

	UFUNCTION(BlueprintCallable)
	void AddUpgradedBubble(TSubclassOf<ABubble> Bubble);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
		void BlueprintBeginPlay();

	UFUNCTION(BlueprintImplementableEvent)
		void StartAim(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintImplementableEvent)
		void StopAim(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintImplementableEvent)
		void ChangedBubble(int Index);

	UFUNCTION(BlueprintImplementableEvent)
		void AddedUpgradedBubble(TSubclassOf<ABubble> Bubble);

	UFUNCTION(BlueprintCallable)
		void ChangeBubble(int32 Index);
	
private:
	void Charge(const FInputActionValue& ActionValue, bool bStandardBubble);

	void Fire(const FInputActionValue& ActionValue, bool bStandardBubble);

	UFUNCTION()
	void Charging();

	void ChangeBubble();

	int GetWhatNumberWasPressed();

	void Scroll(const FInputActionValue& ActionValue);

protected:

	UPROPERTY(BlueprintReadWrite)
		AActor* BoatActor;
	
	UPROPERTY(BlueprintReadWrite)
		bool bIsAimingDown = false;

	UFUNCTION(BlueprintImplementableEvent)
		void StartCharging(bool bIsDefaultBubble);

	UFUNCTION(BlueprintImplementableEvent)
		void StopCharging();
	
	UFUNCTION(BlueprintImplementableEvent)
		void Fired();
	

	ABubble* ChargingBubble;

	TSubclassOf<ABubble> ChargingBubbleType;
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<ABubble> SelectedBubble;
	
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Shooting")
		float LifeTime;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Shooting")
		float BubbleBaseSpeed = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Shooting", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float BubbleMaxSpeedDecrease = 0.7f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Shooting")
		float MaxSizeBubble = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Shooting")
		float MinChargeTime = .5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Shooting")
		float ChargeTime = 1;

	UPROPERTY(EditAnywhere, Category="Shooting", meta = (MakeEditWidget = true))
		FVector FirePoint;

	UPROPERTY(EditAnywhere, Category="Shooting")
		TSubclassOf<ABubble> DefaultBubble;


	bool bIsDefaultCharging = false;
	bool bIsUpgradedCharging = false;
	
	UPROPERTY(EditAnywhere, Category="Shooting")
		bool bExplodeProject;
	
	float fTimeOfCharge;

	APlayerCameraManager* CameraManager;

	FVector Velocity;
	FVector LastPosition;

public:
	// Bubbles that can be shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting")
		TArray<TSubclassOf<ABubble>> UpgradedBubbles;

	UPROPERTY(BlueprintReadWrite)
		TArray<float> Timers;

	UPROPERTY(BlueprintReadWrite)
		TArray<float> Cooldowns;

	UPROPERTY(BlueprintReadWrite)
		int CurrentIndex;

	UPROPERTY(BlueprintReadWrite)
		bool bShouldShoot = true;
};
