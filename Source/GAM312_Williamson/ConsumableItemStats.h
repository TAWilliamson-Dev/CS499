// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConsumableItemStats.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAM312_WILLIAMSON_API UConsumableItemStats : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UConsumableItemStats();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float HealthRestore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float StaminaRestore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float HungerRestore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float HealthRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float StaminaRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float HungerRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float SpoilTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	bool PauseHungerDrain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Stats")
	float PauseHungerDrainTime;
};
