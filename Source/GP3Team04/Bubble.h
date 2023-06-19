// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FishActor.h"
#include "GameFramework/Actor.h"
#include "Bubble.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBubblePop);

UCLASS()
class GP3TEAM04_API ABubble : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABubble();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void FishCaught(AFishActor* FishActor);
	
	UFUNCTION(BlueprintImplementableEvent)
		void POP(USoundWave* PopSound);

	virtual void PopBubble();
	
	virtual void PopBubble(AFishActor* FishActor);
	
	void CaptureFish();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void CatchFish(AFishActor* FishActor);
	
private:
	void FloatBubble(AFishActor* FishActor);


public:
	USoundWave* PopSoundEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString BubbleName = "Bubble";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString BubbleDescription = "It's a bubble";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CaptureSpeed = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* BubbleImage;
	
	UPROPERTY(BlueprintReadOnly)
		float FloatDuration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bUseCooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Cooldown = 2.f;
	
	UPROPERTY(BlueprintReadWrite)
		bool bShouldMove = false;

	UPROPERTY(BlueprintReadWrite)
		float MaxLifeTime = 1000;
	
	UPROPERTY(BlueprintReadWrite)
		float LifeTime = 1000;

	UPROPERTY(BlueprintReadWrite)
		float Size;

	UPROPERTY(BlueprintReadWrite)
		float baseSpeed;
	
	UPROPERTY(BlueprintReadWrite)
		float Speed;

	//Increase after half the time of its lifetime has passed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpeedIncrease = 500.f;

	UPROPERTY(BlueprintReadWrite)
		bool bShouldCoolide = false;

	UPROPERTY(BlueprintReadWrite)
		bool bIgnoreSize = false;

	FVector IntertialVelocity;
	
	float Timer;
	
	FOnBubblePop OnBubblePop;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMeshComponent;

protected:
	float Time;

	AFishActor* CaughtFish;

	FVector TargetLocation;
	FVector StartLocation;

	FVector StartScale;
	FVector TargetScale;

	UWorld* World;

	FTimerHandle CatchingHandle;
};
