// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishManager.generated.h"

UCLASS()
class GP3TEAM04_API AFishManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFishManager();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	class UBoxComponent* BoxComponent;
	
	UFUNCTION()
	FVector GetRandomLocation() const;

	UPROPERTY(VisibleAnywhere, Category="Debug")
	TArray<class AFishActor*> FishList;

	UFUNCTION()
	void EnableFishManager();

	UFUNCTION()
	void DisableFishManager();
	
	UFUNCTION(BlueprintImplementableEvent)
	void InRangeOfManager();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Config")
	USceneComponent* Root;

	// Select the types of fish you would like to spawn.
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<TSubclassOf<class AFishActor>> FishClass;

	// Select the types of special fish you would like to spawn.
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class AFishActor> SpecialFishClass;

	// Set the amount of fish you want to spawn in the manager.
	UPROPERTY(EditAnywhere, Category = "Config")
	int32 FishAmount = 10;

	// Set the amount of special fish you want to spawn in the manager.
	UPROPERTY(EditAnywhere, Category = "Config")
	int32 SpecialFishAmount = 2;

	// // Chance of spawning big fish ex. 10 = 10%, 100 = 100%
	// UPROPERTY(EditAnywhere, Category = "Config", meta = (ClampMin = "1.0", ClampMax = "100.0"))
	// float LargeFishChance = 10;

    UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AActor> BoatActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	float SmallFishMinSize;

	UPROPERTY(EditAnywhere, Category = "Config")
	float SmallFishMaxSize;

	UPROPERTY(EditAnywhere, Category = "Config")
	float LargeFishMinSize;

	UPROPERTY(EditAnywhere, Category = "Config")
	float LargeFishMaxSize;

	// Reference to the boat actor in the scene.
	UPROPERTY()
	AActor* BoatActor;
	
	UPROPERTY()
	bool bManagerIsActive = false;
	
	// Change on how far the manager is from the boat for spawning/de-spawning fish.
	UPROPERTY(EditAnywhere, Category = "Config")
	float DistanceToManager = 5000.f;
	
	UFUNCTION()
	void SpawnFish();
};
