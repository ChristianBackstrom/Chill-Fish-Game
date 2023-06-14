// Fill out your copyright notice in the Description page of Project Settings.


#include "AuroraBorealis.h"
#include "DrawDebugHelpers.h"
#include "Bubble.h"

// Sets default values
AAuroraBorealis::AAuroraBorealis()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMeshComponent->SetCollisionProfileName(TEXT("Trigger"));
	StaticMeshComponent->OnComponentHit.AddDynamic(this, &AAuroraBorealis::HitMesh);
}

// Called when the game starts or when spawned
void AAuroraBorealis::BeginPlay()
{
	Super::BeginPlay();
}

void AAuroraBorealis::HitMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ABubble* Bubble = Cast<ABubble>(OtherActor))
	{
		
		const FVector ReflectedVector = FMath::GetReflectionVector(Bubble->GetActorForwardVector(), Hit.Normal);
		
		Bubble->SetActorRotation(ReflectedVector.Rotation());

		BubbleBounced(Hit.ImpactPoint);
	}
}


