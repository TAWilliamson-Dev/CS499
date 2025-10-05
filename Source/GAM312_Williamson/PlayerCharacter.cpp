// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	if (Stats->Hunger < Stats->MaxHunger) {
		Stats->Hunger += Stats->HungerRate * DeltaTime;
	}

	//Drain health over time if Hunger reaches 0
	if (Stats->Hunger >= Stats->MaxHunger) {
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
			if (bHit) {
				DistanceToGround = (HitResult.ImpactPoint - SpawnStartLocation).Size();
				if (DistanceToGround >= 0.00001f || EndLocation.Z > spawnedPart->GetActorLocation().Z) { //Distance to ground is non-zero, but smaller than this delta value OR player has moved camera up from ground level.
					spawnedPart->SetActorLocation(EndLocation);
				}
				else {
					spawnedPart->SetActorLocation(FVector(EndLocation.X, EndLocation.Y, spawnedPart->GetActorLocation().Z)); //Move along horizontal when making contact with the ground
				}
			}
			else {
				spawnedPart->SetActorLocation(EndLocation); //Continue following the cursor if no ground plane detected
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

	//if(!IsRunning)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("False")));
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

				HitResource->MaxHarvest = HitResource->MaxHarvest - resourceValue;

				ResourceInventory[HitResource->ItemID] += resourceValue;

				if (hitDecal) {
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);
				}

				Stats->Stamina -= Stats->InteractStaminaCost;

				if (HitResource->MaxHarvest <= 0) {
					HitResource->Destroy();
				}

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("You have %i of the item %s"), ResourceInventory[HitResource->ItemID], *HitResource->resourceName));
			}
		}
	}
	else {
		if (spawnedPart) {
			spawnedPart->IsMoving = false;
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

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("You ate a berry!")));
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("You're out of berries.")));
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
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0));
	}
}

void APlayerCharacter::ToggleCraftUI()
{

}
