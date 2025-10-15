// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "BuildingPart.generated.h"

UCLASS()
class GAM312_WILLIAMSON_API ABuildingPart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingPart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UArrowComponent* PivotArrow;

	//Is the player currently "holding" this part.
	UPROPERTY(BlueprintReadWrite)
	bool IsMoving;

	//Is this part trying to snap to another
	UPROPERTY(BlueprintReadWrite)
	bool IsSnapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PartSnapper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* AttachTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* AttachBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* AttachLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* AttachRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* AttachFront;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* AttachBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinSnapTime = 0.0;
};
