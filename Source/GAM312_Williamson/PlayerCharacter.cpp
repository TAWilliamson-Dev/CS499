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
		USkeletalMeshComponent* PlayerMesh = GetMesh();
		PlayerMesh->SetSkeletalMesh(AlternateMeshAsset);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		//Debug jump counter
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.0f,
			FColor::Cyan,
			FString::Printf(TEXT("You jumped %i times!"), JumpCount)
		);
	}
	else if (PlayerCharacterMovement->IsFalling() && JumpCount < MaxJumpCount) {
		LaunchCharacter(FVector(GetActorForwardVector().X * JumpHeight, GetActorForwardVector().Y * JumpHeight, JumpHeight), true, true);
		
		JumpCount++;
	}
}

void APlayerCharacter::StopJump() {
	//Reset jump counter when player is moving on the ground	
	JumpCount = 0;
}

void APlayerCharacter::Landed(const FHitResult& Hit) {
	this->StopJump();
}

void APlayerCharacter::GetObject(const FInputActionValue& Value) {
	bool BoolValue = Value.Get<bool>();
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2d Axis2DValue = Value.Get<FVector2D>();
	
	AddControllerYawInput(Axis2DValue.X);
	AddControllerPitchInput(-1.0f * Axis2DValue.Y);
}
