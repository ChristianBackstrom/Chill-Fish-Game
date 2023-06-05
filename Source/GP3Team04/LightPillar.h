// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightPillar.generated.h"

UENUM(BlueprintType)
enum AttractionColor
{
	Blue = 0        UMETA(DisplayName="Red"),
	Yellow = 1      UMETA(DisplayName="Green"),
	Red = 2       UMETA(DisplayName="Blue"),
};
 
UCLASS()
class GP3TEAM04_API ALightPillar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightPillar();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ChangeColor(AttractionColor Color);
	AttractionColor GetColor();

private:
	UFUNCTION(BlueprintCallable)
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:

	TEnumAsByte<AttractionColor> LightColor;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
		UMaterialInstanceDynamic* MaterialInstance;
};
