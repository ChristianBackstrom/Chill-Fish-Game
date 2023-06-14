// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "AuroraBorealis.generated.h"

UCLASS()
class GP3TEAM04_API AAuroraBorealis : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuroraBorealis();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void BubbleBounced(FVector BounceLocation);
	
private:
	UFUNCTION(BlueprintCallable)
		void HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMeshComponent;
};
