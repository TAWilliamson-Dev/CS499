// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAM312_WILLIAMSON_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	int MaxJumpCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina;

	//Character current health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	//Health auto-regen rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float StarveHealthDrain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger")
	float Hunger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger")
	float MaxHunger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger")
	float HungerRate;

	//Speed of character running / sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float SprintSpeed;

	//Multiplier on stamina costs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float EncumberedStaminaModifier;

	//Stamina per second of character running / sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float SprintStaminaDrain;

	//Speed of normal walking / jogging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float WalkSpeed;

	//Speed of crouch walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float CrouchWalkSpeed;

	//Amount of stamina spent per jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float JumpStaminaDrain;

	//Stamina must regenerate by this additional amount of running out due to sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float SprintDrainStaminaDelay;
};
