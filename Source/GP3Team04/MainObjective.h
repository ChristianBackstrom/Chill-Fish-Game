// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainObjective.generated.h"

class AFishingGamemode;
UCLASS()
class GP3TEAM04_API AMainObjective : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainObjective();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Config")
	float ScoreToReach;

	UPROPERTY(EditAnywhere, Category = "Config")
	const TSoftObjectPtr<UWorld> LevelToOpen;

	AFishingGamemode* FishingGameMode;
public:
	bool bMainObjectiveCompleted = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
