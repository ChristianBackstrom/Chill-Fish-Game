// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FishingGameInstance.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FishActor.generated.h"


class UBoxComponent;
class UBehaviorTree;
class AFishManager;
class USphereComponent;
UCLASS()
class GP3TEAM04_API AFishActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFishActor();
    // Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void FishIsCaught();
	
	UPROPERTY()
	AFishManager* FishManager;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMesh;

	// Add materials here that you want the fish to have.
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<UMaterial*> InstanceMaterial;

	FFish Fish;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TEnumAsByte<FishType> TypeOfFish;
	
	UPROPERTY()
	FVector OldLocation;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldMove = true;

	UPROPERTY()
	bool bEnabled = true;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BubbleMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* SphereCollider;

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	// Fish initial swim speed.
	UPROPERTY(EditAnywhere, Category = "Config")
	float SwimSpeed = 100.f;

	// Fish turn speed, 0.5 for default. Change as you like.
	UPROPERTY(EditAnywhere, Category = "Config")
	float TurnTime = 0.5f;

	// Chance of getting startled 0.1 = 10% chance. 1 = 100% chance.
	UPROPERTY(EditAnywhere, Category = "Config", meta = (ClampMin = "0.0", ClampMax = "1"))
	float StartleChance = 0.01f;

	// Self explanatory. Multiplies the current fish speed with the value you give it.
	UPROPERTY(EditAnywhere, Category = "Config")
	float BoostSpeedMultiplier = 2.f;

	// How long the boost should last. Set this duration to be equal to the Initial Boost Duration. 
	UPROPERTY(EditAnywhere, Category = "Config")
	float BoostDuration = 5.0f;

	// Set this duration to be equal to the Boost Duration.
	UPROPERTY(VisibleAnywhere)
	float InitialBoostDuration = 5.f;

	// If set to true, the fish will have a chance to get scared.
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bCanBeScared = true;
	
	UPROPERTY()
	float CollisionCheckRadius = 100.f;

	float OriginalSwimSpeed;
	
	UPROPERTY()
    FVector RandomLocation;
    	
	UPROPERTY()
	FRotator TargetRotation;
    
	UPROPERTY()
    bool bReachedLocation = false;
	
	UPROPERTY()
	bool bIsSpeedBoostActive = false;

	UPROPERTY()
	bool bBoostActive = false;
	
	UFUNCTION()
	void FishMovement(float DeltaTime);
	
	UFUNCTION()
	void MoveToRandomLocation(const FVector& CurrentLocation);

	UFUNCTION()
	void MoveToNewLocation(const FVector& TargetLocation, float DeltaTime, bool bBoostedSpeed);

	UFUNCTION()
	void InterpolateRotation(const FRotator& CurrentRotation, float DeltaTime);

	UFUNCTION()
	void AvoidCollision();

	UFUNCTION()
	void ApplyStartleBoost();

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
