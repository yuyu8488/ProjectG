// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerControllerG.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagG.h"
#include "AbilitySystem/AbilitySystemComponentG.h"
#include "Character/PlayerCharacter.h"
#include "Input/EnhancedInputComponentG.h"
#include "Player/PlayerStateG.h"

APlayerControllerG::APlayerControllerG()
{
	bReplicates = true;
	SetShowMouseCursor(true);
}

void APlayerControllerG::BeginPlay()
{
	Super::BeginPlay();

	//check(InputMappingContext);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void APlayerControllerG::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponentG* EnhancedInputComponentG = CastChecked<UEnhancedInputComponentG>(InputComponent);
	EnhancedInputComponentG->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerControllerG::Move);
	EnhancedInputComponentG->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerControllerG::Look);
	EnhancedInputComponentG->BindAbilityAction(InputConfig,this,
		&APlayerControllerG::AbilityActionInputPressed,
		&APlayerControllerG::AbilityActionInputReleased,
		&APlayerControllerG::AbilityActionInputHeld);
}

void APlayerControllerG::AbilityActionInputPressed(EAbilityInputID AbilityInputID)
{
	//UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	//ASC->AbilityInputIDPressed(AbilityInputID);
	
	APlayerStateG* PS = GetPlayerState<APlayerStateG>();
	if (PS)
	{
		UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(PS->GetAbilitySystemComponent());
		if (ASC)
		{
			ASC->AbilityInputIDPressed(AbilityInputID);
		}
	}
}

void APlayerControllerG::AbilityActionInputReleased(EAbilityInputID AbilityInputID)
{
	APlayerStateG* PS = GetPlayerState<APlayerStateG>();
	if (PS)
	{
		UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(PS->GetAbilitySystemComponent());
		if (ASC)
		{
			ASC->AbilityInputIDReleased(AbilityInputID);
		}
	}
}

void APlayerControllerG::AbilityActionInputHeld(EAbilityInputID AbilityInputID)
{
	APlayerStateG* PS = GetPlayerState<APlayerStateG>();
	if (PS)
	{
		UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(PS->GetAbilitySystemComponent());
		if (ASC)
		{
			ASC->AbilityInputIDHeld(AbilityInputID);
		}
	}
}

void APlayerControllerG::Move(const struct FInputActionValue& Value)
{
	APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>();
	const FVector2D Input = Value.Get<FVector2D>();
	
	if (!ControlledCharacter || Input.IsNearlyZero()) return;

	// 1. 월드 공간기준 방향 벡터 계산
	const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	// 2. 입력방향 계산후 Character의 LastInputSet
	const FVector WorldSpaceInputDirection = (Forward * Input.Y + Right * Input.X).GetSafeNormal();
	ControlledCharacter->SetLastInputDirection(WorldSpaceInputDirection);

	// 애초에 공격몽타주가 
	// if (ControlledCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTagG::Get().Character_State_Attacking)) return;
	
	// 3. 이동 처리
	ControlledCharacter->AddMovementInput(Forward, Input.Y);
	ControlledCharacter->AddMovementInput(Right, Input.X);
}

void APlayerControllerG::Look(const struct FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}
