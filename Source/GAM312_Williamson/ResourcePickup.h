// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConsumableItemStats.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ResourcePickup.generated.h"

UCLASS()
class GAM312_WILLIAMSON_API AResourcePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResourcePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh; //Item world model

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UTextRenderComponent* Name; //Display when item is hovered over in game

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FText tempName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString resourceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float ItemValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float HarvestQuantity; //Default # of item to harvest per hit or pickup

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float HarvestQuantityDelta; //Adjust pickup quantity per hit or pickup. i.e. HarvestQuanity = 3, Delta = 1 so pickup 2 to 4

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	float MaxHarvest; //Amount of resource per node

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	bool IsConsumable; //If true, item can be used to restore Health / Stamina / Hunger

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UConsumableItemStats* ConsumableStats;
};
