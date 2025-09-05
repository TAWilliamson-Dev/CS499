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
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

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

	Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartJump);
}

void APlayerCharacter::Move(const FInputActionValue& Value) {
	FVector2d Axis2DValue = Value.Get<FVector2D>();


}

void APlayerCharacter::StartJump(const FInputActionValue& Value) {
	bool BoolValue = Value.Get<bool>();

}

void APlayerCharacter::StopJump() {

}

void APlayerCharacter::GetObject(const FInputActionValue& Value) {
	bool BoolValue = Value.Get<bool>();
}