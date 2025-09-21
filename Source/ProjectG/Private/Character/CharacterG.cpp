// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterG.h"

#include "AbilitySystem/AbilitySystemComponentG.h"
#include "AbilitySystem/AttributeSetG.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACharacterG::ACharacterG()
{
	PrimaryActorTick.bCanEverTick = false;

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

void ACharacterG::FacePrimaryTarget_Implementation()
{
	if (PrimaryCombatTarget == nullptr) return;

	const FVector MyLocation = GetActorLocation();
	const FVector TargetLocation = PrimaryCombatTarget->GetTargetLocation();

	// 타겟을 바라보는 회전값 계산
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);

	// 캐릭터의 Yaw값만 수정
	FRotator MyRotation = GetActorRotation();
	MyRotation.Yaw = LookAtRotation.Yaw;
	SetActorRotation(MyRotation);
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


