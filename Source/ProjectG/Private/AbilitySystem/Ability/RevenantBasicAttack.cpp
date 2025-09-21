// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/RevenantBasicAttack.h"

#include "GameplayTagG.h"
#include "AbilitySystem/AbilitySystemComponentG.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actor/Projectile.h"
#include "Character/PlayerCharacter.h"


void URevenantBasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 타겟 검색
	if (ActorInfo->AvatarActor.Get()->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_UpdateTargets(ActorInfo->AvatarActor.Get());
		ICombatInterface::Execute_UpdatePrimaryTarget(ActorInfo->AvatarActor.Get());
		TargetActor = ICombatInterface::Execute_GetPrimaryTarget(ActorInfo->AvatarActor.Get());
		ICombatInterface::Execute_FacePrimaryTarget(ActorInfo->AvatarActor.Get());
	}
	
	// 쿨타임, 소모자원 확인. 
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// ASC 확인
	UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(GetAbilitySystemComponentFromActorInfo_Ensured());
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// #include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
	// CreatePlayMontageAndWaitProxy : 비동기작업, Proxy(대리자) 객체 생성, 델리게이트(OnCompleted, OnInterrupted) 가능.
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Info.AnimMontage);
	
	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 콜백 설정 
	// 캐릭터가 애니메이션 
	MontageTask->OnCompleted.AddDynamic(this, &URevenantBasicAttack::OnComboEnd);
	MontageTask->OnInterrupted.AddDynamic(this, &URevenantBasicAttack::OnComboSave);
	//MontageTask->OnCancelled.AddDynamic(this, &URevenantBasicAttack::OnComboEnd);
	
	/** 발사체 생성 발사 */
	// 스폰 타이밍 설정, AnimNotify로 이벤트 보내기
	// #include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
	UAbilityTask_WaitGameplayEvent* WaitFireEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTagG::Get().Ability_BasicAttack);
	WaitFireEventTask->EventReceived.AddDynamic(this, &URevenantBasicAttack::OnFireEvent);
	WaitFireEventTask->ReadyForActivation();
	
	// 콤보 카운트 증가.
	int32 NextComboCount = (Info.ComboStep + 1) % Info.MaxComboStep;
	ASC->SetAbilityComboCounterValue(Info.AbilityInputID, NextComboCount);
	
	// 마지막으로 애니메이션 실행.
	MontageTask->ReadyForActivation(); 
}

void URevenantBasicAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled)
	{
		UE_LOG(LogTemp, Warning, TEXT("RevenantBasicAttack was canceled!!!!"));
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URevenantBasicAttack::OnComboSave()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URevenantBasicAttack::OnComboEnd()
{
	// 공격을 받거나 다른 행동을 취했을때.
	ResetCombo();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URevenantBasicAttack::ResetCombo()
{
	if (UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(GetAbilitySystemComponentFromActorInfo_Ensured()))
	{
		ASC->SetAbilityComboCounterValue(Info.AbilityInputID, 0); 
	}
}

void URevenantBasicAttack::OnFireEvent(FGameplayEventData InEventData)
{
	//서버에서만 스폰
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	if (!OwnerCharacter || !OwnerCharacter->Implements<UCombatInterface>())
	{
		return;
	}

	// TODO: Projectile Spawn 설정 다시
	if (TargetActor@@@@@@@@@@@@@@@@@)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetProjectileSocketLocation(OwnerCharacter, SpawnSocketName);
		FRotator Rotation = (TargetActor->GetActorLocation() - SocketLocation).Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());		
		

		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		// 발사체에 DamageEffectParams 설정.
		FDamageEffectParams ProjectileDamageEffectParams = MakeDamageEffectParamsFromClassDefaults(TargetActor, FGameplayTagG::Get().Ability_BasicAttack);
		Projectile->SetDamageEffectParams(ProjectileDamageEffectParams);

		Projectile->FinishSpawning(SpawnTransform);
	
		// 다음 Fire 이벤트를 받기위해 테스크 재생성.
		UAbilityTask_WaitGameplayEvent* WaitFireEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTagG::Get().Ability_BasicAttack);
		WaitFireEventTask->EventReceived.AddDynamic(this, &URevenantBasicAttack::OnFireEvent);
		WaitFireEventTask->ReadyForActivation();
	}
}
