// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include <EnhancedInputSubsystemInterface.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "ResourcePickup.h"
#include "StatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GAM312_WILLIAMSON_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void StartJump(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Sprint(const FInputActionValue& Value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);

	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	UStatComponent* Stats;

	UPROPERTY(BlueprintReadOnly, Category = "Jump")
	int JumpCount;

	UPROPERTY(BlueprintReadOnly)
	bool IsRunning;

	UPROPERTY(BlueprintReadOnly)
	bool IsJumping;

	UPROPERTY(BlueprintReadOnly)
	bool IsEncumbered;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleDefaultsOnly)
	UCharacterMovementComponent* PlayerCharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assets")
	USkeletalMesh* AlternateMeshAsset;

	UPROPERTY(EditAnywhere, Category = "HitMarker")
	UMaterialInterface* hitDecal;

	/*
		Enhanced input mapping context and actions, can be broken off into separate data asset.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;
	class UInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly)
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* MenuAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	UInputAction* ObjectiveUIAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InventoryUIAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* HotbarUISelectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int> ResourceInventory;
};
