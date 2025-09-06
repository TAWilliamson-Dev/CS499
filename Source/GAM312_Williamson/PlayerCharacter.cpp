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

	if (IsRunning && IsEncumbered) {
		Stamina -= SprintStaminaDrain * EncumberedStaminaModifier * DeltaTime;
	}
	else if (IsRunning) {
		Stamina -= SprintStaminaDrain * DeltaTime;
	}
	else if (Stamina < MaxStamina) {
		Stamina += StaminaRegenRate * DeltaTime;
	}


}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

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

	if (!PlayerCharacterMovement->IsFalling() &&  JumpCount < MaxJumpCount) {
		LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
		JumpCount++;
	}
	else if (PlayerCharacterMovement->IsFalling() && JumpCount < MaxJumpCount) {
		//If double jump or higher is available (Controlled via MaxJumpCount), adjust jump direction along forward vector.
		LaunchCharacter(FVector(GetActorForwardVector().X * JumpHeight, GetActorForwardVector().Y * JumpHeight, JumpHeight), true, true);
		JumpCount++;
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit) {
	//Reset jump counter when player makes contact with the ground
	Super::Landed(Hit);
	JumpCount = 0;
}

void APlayerCharacter::GetObject(const FInputActionValue& Value) {
	bool BoolValue = Value.Get<bool>();
	//Line trace from player to target object, pick up object if line trace hits
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
	
	if (Stamina > 0) {
		PlayerCharacterMovement->MaxWalkSpeed = SprintSpeed;
		IsRunning = true;
	}
	else {
		this->StopSprint(true);
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	PlayerCharacterMovement->MaxWalkSpeed = WalkSpeed;
	IsRunning = false;
}
