// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterG.h"

#include "AbilitySystem/AbilitySystemComponentG.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/AttributeSetG.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACharacterG::ACharacterG()
{
	PrimaryActorTick.bCanEverTick = false;

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
}

UAbilitySystemComponent* ACharacterG::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ACharacterG::IsInCombat_Implementation()
{
	return bInCombat;
}

void ACharacterG::SetInCombat_Implementation(const bool InCombat)
{
	bInCombat = InCombat;
}

void ACharacterG::UpdateTargets_Implementation()
{
	ICombatInterface::UpdateTargets_Implementation();
}

void ACharacterG::UpdatePrimaryTarget_Implementation()
{
	ICombatInterface::UpdatePrimaryTarget_Implementation();
}

AActor* ACharacterG::GetPrimaryTarget_Implementation()
{
	return PrimaryCombatTarget;
}

void ACharacterG::FacePrimaryTargetWithMotionWarping_Implementation()
{
	if (PrimaryCombatTarget == nullptr || MotionWarpingComponent == nullptr)
	{
		return;
	}
	
	const FVector MyLocation = GetActorLocation();
	const FVector TargetLocation = PrimaryCombatTarget->GetActorLocation();
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);

	// Motion Warping 설정
	FMotionWarpingTarget WarpingTarget;
	WarpingTarget.Name = FName("Target");
	WarpingTarget.Location = TargetLocation;
	WarpingTarget.Rotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

	MotionWarpingComponent->AddOrUpdateWarpTarget(WarpingTarget);
}

FVector ACharacterG::GetProjectileSocketLocation_Implementation(FName InSocketName)
{
	return GetMesh()->GetSocketLocation(InSocketName);
}

void ACharacterG::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterG::InitAbilitySystem()
{
}

void ACharacterG::InitCharacterAbility() const
{
	UAbilitySystemComponentG* ASC = CastChecked<UAbilitySystemComponentG>(AbilitySystemComponent);

	if (StartupAbilities.IsEmpty()) return;

	ASC->InitAbility(StartupAbilities);
}

void ACharacterG::InitCharacterAttributes() const
{
	UAbilitySystemComponentG* ASC = CastChecked<UAbilitySystemComponentG>(AbilitySystemComponent);

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(
		VitalAttributeEffect,
		1.f,
		ContextHandle);
	
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}


