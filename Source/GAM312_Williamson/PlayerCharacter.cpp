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

	Stats->Hunger += Stats->HungerRate * DeltaTime;

	//Drain health over time if Hunger reaches 0
	if (Stats->Hunger >= Stats->MaxHunger) {
		Stats->Health -= Stats->StarveHealthDrain * DeltaTime;
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

void APlayerCharacter::GetObject(const FInputActionValue& Value) {
	bool BoolValue = Value.Get<bool>();
	//TODO: Line trace from player to target object, pick up object if line trace hits
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
