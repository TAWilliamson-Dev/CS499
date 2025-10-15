// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPart.h"

// Sets default values
ABuildingPart::ABuildingPart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	PivotArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Pivot Arrow"));
	PartSnapper = CreateDefaultSubobject<UBoxComponent>(TEXT("Part Snapper"));
	AttachTop = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Top"));
	AttachBottom = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Bottom"));
	AttachLeft = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Left"));
	AttachRight = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Right"));
	AttachFront = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Front"));
	AttachBack = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Back"));

	RootComponent = PivotArrow;
	Mesh->SetupAttachment(PivotArrow);

	//Setup attachments for default part snapping detection box collider and arrows for origin points to attach to
	PartSnapper->SetupAttachment(PivotArrow);
	AttachTop->SetupAttachment(PartSnapper);
	AttachBottom->SetupAttachment(PartSnapper);
	AttachLeft->SetupAttachment(PartSnapper);
	AttachRight->SetupAttachment(PartSnapper);
	AttachFront->SetupAttachment(PartSnapper);
	AttachBack->SetupAttachment(PartSnapper);
}

// Called when the game starts or when spawned
void ABuildingPart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuildingPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

