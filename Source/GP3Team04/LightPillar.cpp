// Fill out your copyright notice in the Description page of Project Settings.


#include "LightPillar.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Bubble.h"

// Sets default values
ALightPillar::ALightPillar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	RootComponent = StaticMeshComponent;
}

// Called when the game starts or when spawned
void ALightPillar::BeginPlay()
{
	Super::BeginPlay();

	MaterialInstance = UMaterialInstanceDynamic::Create(StaticMeshComponent->GetMaterial(0), nullptr);
	StaticMeshComponent->SetMaterial(0, MaterialInstance);
}

// Called every frame
void ALightPillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALightPillar::ChangeColor(::AttractionColor Color)
{
	LightColor = Color;
	FLinearColor ColorCode;

	switch (Color)
	{
	case Red :
		ColorCode = FLinearColor(1.f,0.f,0.f);
		break;
	case Yellow :
		ColorCode = FLinearColor(1.f,1.f,0.f);
		break;
	case Blue :
		ColorCode = FLinearColor(0.f,0.f,1.f);
		break;
	}
	
	MaterialInstance->SetVectorParameterValue(FName(TEXT("BaseColor")), ColorCode);
}

AttractionColor ALightPillar::GetColor()
{
	return  LightColor;
}

void ALightPillar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABubble* Bubble = Cast<ABubble>(OtherActor))
	{
		int ColorIndex = FMath::RoundToInt(Bubble->Size * 3) - 1;
		ColorIndex = FMath::Clamp(ColorIndex, 0, 2);
		AttractionColor NewColor = static_cast<AttractionColor>(ColorIndex);
		
		ChangeColor(NewColor);
	}
}




