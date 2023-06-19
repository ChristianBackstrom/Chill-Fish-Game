// Fill out your copyright notice in the Description page of Project Settings.


#include "MainObjective.h"

#include "FishingGamemode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMainObjective::AMainObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMainObjective::BeginPlay()
{
	Super::BeginPlay();

	FishingGameMode = Cast<AFishingGamemode>(UGameplayStatics::GetGameMode(GetWorld()));
}

// Called every frame
void AMainObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(FishingGameMode)) return;

	if (FishingGameMode->Score >= ScoreToReach && !bMainObjectiveCompleted)
	{
		PrimaryActorTick.bCanEverTick = false;
		bMainObjectiveCompleted = true;
		ScoreReached();	
	}
}

