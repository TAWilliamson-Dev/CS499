// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcePickup.h"

// Sets default values
AResourcePickup::AResourcePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	RootComponent = Mesh; // Set it as the root component

	Name = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Item Name"));

	Name->SetupAttachment(Mesh);

	ItemWeight = 0;

	ItemValue = 0;

	ItemID = 0;

	IsConsumable = false;

	ConsumableStats = CreateDefaultSubobject<UConsumableItemStats>(TEXT("Consumable Stats"));
}

// Called when the game starts or when spawned
void AResourcePickup::BeginPlay()
{
	Super::BeginPlay();

	tempName = tempName.FromString(resourceName);

	Name->SetText(tempName);
}

// Called every frame
void AResourcePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

