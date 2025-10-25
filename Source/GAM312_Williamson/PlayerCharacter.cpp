// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create default sub objects for the player camera, hit decal, and initialize the resource inventory as an array of ints
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));

	PlayerCamera->SetupAttachment(GetMesh(), "head");

	PlayerCamera->bUsePawnControlRotation = true;

	Stats = CreateDefaultSubobject<UStatComponent>("Stats");

	PlayerCharacterMovement = GetCharacterMovement();

	hitDecal = CreateDefaultSubobject<UMaterialInterface>("Hit Marker");

	ResourceInventory.Init(0, 3);

	BuildingArray.SetNum(3);

	isBuilding = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AlternateMeshAsset)
	{
		//Adjust skeletal mesh if alternate mesh is specified.
		USkeletalMeshComponent* PlayerMesh = GetMesh();
		PlayerMesh->SetSkeletalMesh(AlternateMeshAsset);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Manage stamina drain / recharge over time
	if (IsRunning && IsEncumbered) {
		Stats->Stamina -= Stats->SprintStaminaDrain * Stats->EncumberedStaminaModifier * DeltaTime; //Increased stamina drain while inventory is overloaded.
	}
	else if (IsRunning) {
		Stats->Stamina -= Stats->SprintStaminaDrain * DeltaTime; //Drain stamina while sprinting
	}
	else if (Stats->Stamina < Stats->MaxStamina) {
		Stats->Stamina += Stats->StaminaRegenRate * DeltaTime; //Regen stamina while not performing an action that drains stamina over time

		//Player ran out of stamina while sprinting
		if (Stats->SprintDrainStaminaDelay > 0 && Stats->Stamina >= Stats->SprintDrainStaminaDelay) {
			Stats->SprintDrainStaminaDelay = 0;
		}
	}

	//Observe player hunger, 
	if (Stats->Hunger < Stats->MaxHunger) {
		Stats->Hunger += Stats->HungerRate * DeltaTime;
	}

	//Drain health over time if Hunger reaches 0
	if (Stats->Hunger >= Stats->MaxHunger && Stats->Health >= 0) {
		Stats->Health -= Stats->StarveHealthDrain * DeltaTime;
	}

	if (isBuilding) {
		if (spawnedPart) {
			
			spawnedPart->IsMoving = true;

			FVector StartLocation = PlayerCamera->GetComponentLocation();
			FVector Direction = PlayerCamera->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;

			FVector SpawnStartLocation = spawnedPart->GetActorLocation();
			FVector SpawnGroundLevel = SpawnStartLocation - FVector(0, 0, 1000.0f);

			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			Params.AddIgnoredActor(spawnedPart);
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, SpawnStartLocation, SpawnGroundLevel, ECC_Visibility, Params);
			//Don't let the part go lower than the nearest surface
			float DistanceToGround = 0.0f;
			if (bHit && !spawnedPart->IsSnapping) {
				DistanceToGround = (HitResult.ImpactPoint - SpawnStartLocation).Size();
				if (DistanceToGround >= 0.00001f || EndLocation.Z > spawnedPart->GetActorLocation().Z) { //Distance to ground is non-zero, but smaller than this delta value OR player has moved camera up from ground level.
					spawnedPart->SetActorLocation(EndLocation);
				}
				else {
					spawnedPart->SetActorLocation(FVector(EndLocation.X, EndLocation.Y, spawnedPart->GetActorLocation().Z)); //Move along horizontal when making contact with the ground
				}
			}
			else if (spawnedPart->IsMoving) {
					
				FVector WorldCenter = spawnedPart->Mesh->GetComponentLocation();
				FVector SweepExtent = spawnedPart->PartSnapper->GetScaledBoxExtent();
				FCollisionShape BoxCollision = FCollisionShape::MakeBox(SweepExtent);
				FQuat QuatRotation = FQuat{ 0.0f,0.0f,0.0f,0.0f };

				if (GetWorld()->SweepSingleByChannel(HitResult, WorldCenter, WorldCenter, QuatRotation, ECC_Visibility,BoxCollision, Params) && 
					((FMath::Abs(EndLocation.X - spawnedPart->GetActorLocation().X) < 350) && (FMath::Abs(EndLocation.Y - spawnedPart->GetActorLocation().Y) < 350) && (FMath::Abs(EndLocation.Z - spawnedPart->GetActorLocation().Z) < 350))) {
					ABuildingPart* HitPart = Cast<ABuildingPart>(HitResult.GetActor());

					//Distance from top/bottom, right/left, front/back
					TArray Distances = TArray<float>{0.0,0.0,0.0,0.0,0.0,0.0};

					if (HitPart) {
						Distances[0] = (spawnedPart->AttachTop->GetComponentLocation() - HitPart->AttachBottom->GetComponentLocation()).Size();
						Distances[1] = (HitPart->AttachTop->GetComponentLocation() - spawnedPart->AttachBottom->GetComponentLocation()).Size();
						Distances[2] = (spawnedPart->AttachRight->GetComponentLocation() - HitPart->AttachLeft->GetComponentLocation()).Size();
						Distances[3] = (HitPart->AttachRight->GetComponentLocation() - spawnedPart->AttachLeft->GetComponentLocation()).Size();
						Distances[4] = (spawnedPart->AttachFront->GetComponentLocation() - HitPart->AttachBack->GetComponentLocation()).Size();
						Distances[5] = (HitPart->AttachFront->GetComponentLocation() - spawnedPart->AttachBack->GetComponentLocation()).Size();

						float minValueX = 0.0f, minValueY = 0.0f, minValueZ = 0.0f, deltaT = 0.0f;

						if (Distances[0] < Distances[1]) {
							minValueZ = HitPart->AttachBottom->GetComponentLocation().Z;
						}
						else if(Distances[0] > Distances[1]) {
							minValueZ = HitPart->AttachTop->GetComponentLocation().Z;
						}

						if (Distances[2] < Distances[3]) {
							minValueY = HitPart->AttachRight->GetComponentLocation().Y - HitPart->AttachRight->GetRelativeLocation().Y;
						}
						else if(Distances[2] > Distances[3]) {
							minValueY = HitPart->AttachLeft->GetComponentLocation().Y + HitPart->AttachLeft->GetRelativeLocation().Y;
						}
						else {
							minValueY = HitPart->AttachFront->GetComponentLocation().Y + spawnedPart->AttachFront->GetRelativeLocation().Y;
						}

						if (Distances[4] < Distances[5]) {
							minValueX = HitPart->AttachFront->GetComponentLocation().X - HitPart->AttachFront->GetRelativeLocation().X;
						}
						else if(Distances[4] > Distances[5]) {
							minValueX = HitPart->AttachBack->GetComponentLocation().X + HitPart->AttachBack->GetRelativeLocation().X;;
						}
						else {
							minValueX = HitPart->AttachTop->GetComponentLocation().X + HitPart->AttachBottom->GetRelativeLocation().X;
						}
						
						
						if (spawnedPart->MinSnapTime == 0.0f) {
							spawnedPart->SetActorLocation(FVector{ minValueX,minValueY,minValueZ });
							spawnedPart->IsSnapping = true;
							spawnedPart->IsMoving = false;
						}
						else if (spawnedPart->MinSnapTime < 5.0f) {
							spawnedPart->MinSnapTime += 1.0f * DeltaTime;
						}
						else {
							spawnedPart->MinSnapTime = 0.0f;
						}
					}
				}
				else {
					spawnedPart->IsMoving = true;
					spawnedPart->IsSnapping = false;
					spawnedPart->SetActorLocation(EndLocation); //Continue following the cursor if the player is out of range of a snapping point
				}
			}
		}
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	//Clear default mapping context and add desired mapping contexts
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	//Bind Input Events to appropriate functions
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::StartJump);
	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	Input->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
	Input->BindAction(SprintAction, ETriggerEvent::Canceled, this, &APlayerCharacter::StopSprint);
	Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
	Input->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerCharacter::Interact);
	Input->BindAction(EatAction, ETriggerEvent::Completed, this, &APlayerCharacter::EatBerries);
	Input->BindAction(BuildingRotateAction, ETriggerEvent::Completed, this, &APlayerCharacter::RotateBuilding);
	//Input->BindAction(CraftAction, ETriggerEvent::Completed, this, &APlayerCharacter::ToggleCraftUI);

}

void APlayerCharacter::Move(const FInputActionValue& Value) {
	FVector2d Axis2DValue = Value.Get<FVector2D>();
	
	AddMovementInput(this->GetActorForwardVector(), Axis2DValue.X);
	AddMovementInput(this->GetActorRightVector(), Axis2DValue.Y);

}

void APlayerCharacter::StartJump(const FInputActionValue& Value) {
	bool BoolValue = Value.Get<bool>();
	float JumpHeight = PlayerCharacterMovement->JumpZVelocity;

	// Character can jump along forward vector if they have enough stamina, up to MaxJumpCount # of times. MaxJumpCount default is 1
	if (Stats->Stamina >= Stats->JumpStaminaDrain && JumpCount < Stats->MaxJumpCount) {
		LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
		JumpCount++;

		Stats->Stamina -= Stats->JumpStaminaDrain;
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit) {
	//Reset jump counter when player makes contact with the ground
	Super::Landed(Hit);
	JumpCount = 0;
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2d Axis2DValue = Value.Get<FVector2D>();
	
	//Setup mouse look
	AddControllerYawInput(Axis2DValue.X);
	AddControllerPitchInput(-1.0f * Axis2DValue.Y);
}

void APlayerCharacter::Sprint(const FInputActionValue& Value) {
	bool BoolValue = Value.Get<bool>();
	
	//Player can sprint if stamina is greater than SprintStaminaDrain, stamina drain and recharge are tracked in OnTick
	if (Stats->Stamina > Stats->SprintStaminaDrain+ Stats->SprintDrainStaminaDelay) {
		PlayerCharacterMovement->MaxWalkSpeed = Stats->SprintSpeed;
		IsRunning = true;
	}
	else {
		this->StopSprint(true);
		//Player ran out of stamina while sprinting, add a minimum stamina recharge to sprint again.
		Stats->SprintDrainStaminaDelay = 25;
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	//Return player movement speed to normal
	PlayerCharacterMovement->MaxWalkSpeed = Stats->WalkSpeed;
	IsRunning = false;
}

void APlayerCharacter::Interact(const FInputActionValue& Value) {
	FHitResult HitResult;
	FVector StartLocation = PlayerCamera->GetComponentLocation();
	FVector Direction = PlayerCamera->GetForwardVector() * 800.0f;
	FVector EndLocation = StartLocation + Direction;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;

	if (!isBuilding) {

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams)) {
			AResourcePickup* HitResource = Cast<AResourcePickup>(HitResult.GetActor());

			if (Stats->Stamina >= Stats->InteractStaminaCost && HitResource) {

				FString hitName = HitResource->resourceName;
				int resourceValue = FMath::RandRange(HitResource->HarvestQuantity - HitResource->HarvestQuantityDelta, HitResource->HarvestQuantity + HitResource->HarvestQuantityDelta);

				//Decrement the hit resource node by the amount given to the player
				HitResource->MaxHarvest = HitResource->MaxHarvest - resourceValue;

				//Increment the quantity of the item by index in the player inventory
				ResourceInventory[HitResource->ItemID] += resourceValue;
				Stats->TotalResources += resourceValue; //Increase total resources harvested for objective tracking. GAM 312 Module 5

				if (hitDecal) {
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);
				}

				Stats->Stamina -= Stats->InteractStaminaCost;

				//Destroy the actor on resource exhaustion
				if (HitResource->MaxHarvest <= 0) {
					HitResource->Destroy();
				}			
			}
		}
	}
	else {
		if (spawnedPart) {
			spawnedPart->IsMoving = false;
			Stats->TotalBuildingParts += 1; //Increment total parts built by player for objective tracking.
		}
		isBuilding = false;
	}
}

void APlayerCharacter::EatBerries(const FInputActionValue& Value)
{
	//ItemID = 0 : Berries
	if (ResourceInventory[0] > 0) {
		ResourceInventory[0] -= 1;
		if (Stats->Hunger - 5 < Stats->MaxHunger) {
			Stats->Hunger -= 5;
		}
	}
}

void APlayerCharacter::UpdateResources(float woodAmount, float stoneAmount, FString buildingObject) {
	if (woodAmount <= ResourceInventory[1]) {
		if (stoneAmount <= ResourceInventory[2]) {
			ResourceInventory[1] = ResourceInventory[1] - woodAmount;
			ResourceInventory[2] = ResourceInventory[2] - stoneAmount;

			if (buildingObject == "Wall") {
				BuildingArray[0] += 1;
			}

			if (buildingObject == "Floor") {
				BuildingArray[1] += 1;
			}

			if (buildingObject == "Ceiling") {
				BuildingArray[2] += 1;
			}
		}
	}
}

void APlayerCharacter::SpawnBuilding(int buildingID, bool& isSuccess) {
	if (!isBuilding) {
		if (BuildingArray[buildingID] >= 1) {
			isBuilding = true;
			FActorSpawnParameters SpawnParams;
			FVector StartLocation = PlayerCamera->GetComponentLocation();
			FVector Direction = PlayerCamera->GetForwardVector() * 400.f;
			FVector EndLocation = StartLocation + Direction;
			FRotator myRot(0, 0, 0);

			BuildingArray[buildingID] -= 1;

			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);

			isSuccess = true;
		}
		else {
			isSuccess = false;
		}
	}
}

void APlayerCharacter::RotateBuilding() {
	if (isBuilding) {
		spawnedPart->Mesh->SetRelativeRotation(FRotator(0.0f,spawnedPart->Mesh->GetRelativeRotation().Yaw + 90.0f, 0.0f));
	}
}

void APlayerCharacter::ToggleCraftUI()
{

}
