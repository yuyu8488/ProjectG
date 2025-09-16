// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/RevenantBasicAttack.h"

#include "GameplayTagG.h"
#include "AbilitySystem/AbilitySystemComponentG.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Actor/Projectile.h"
#include "Character/PlayerCharacter.h"


void URevenantBasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UE_LOG(LogTemp, Log, TEXT("ActivateAbility [%s]"), *GetName());

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
	

	//TODO: 애니메이션 진행중>> 특정 시간 이상일때, 이동키를 누르면 이동이 되게끔 하기. 
	// 특정 타임(Window태그가 없을때) >> 애니메이션을 종료하고 이동이 가능하게끔. (현재 몽타주가 RootMotion으로 설정되어있음)

	// #include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
	//UAbilityTask_WaitInputPress* InputTask = UAbilityTask_WaitInputPress::Create


	//	UAbilityTask_WaitInputPress::CreateWaitInputPressProxy(this, false); // bTestInitialState=false
	//if (InputTask)
	//{
	//	// 입력이 감지되면 OnInputPressed 함수가 호출됩니다.
	//	InputTask->OnPress.AddDynamic(this, &UMyGameplayAbility::OnInputPressed);
	//	InputTask->ReadyForActivation();
	//}


	/** 발사체 생성 발사 */
	// 스폰 타이밍 설정, AnimNotify 에서 이벤트 보내기(blueprint 설정)
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
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URevenantBasicAttack::OnComboSave()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URevenantBasicAttack::OnComboEnd()
{
	// 공격을 받거나 다른 행동을 취했을때.
	ResetCombo();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
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
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	if (!OwnerCharacter || !OwnerCharacter->Implements<UCombatInterface>())
	{
		return;
	}
	const FTransform SocketTransform = ICombatInterface::Execute_GetProjectileSocketTransform(OwnerCharacter, SpawnSocketName);
	
	if (TargetActor)
	{
		//타겟을 향하는 방향 계산 -> Rotation
		const FVector Direction = (TargetActor->GetActorLocation() - SocketTransform.GetLocation()).GetSafeNormal();
		const FRotator SpawnRotation = Direction.Rotation();

		// Projectile Spawn rule
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = OwnerCharacter;
		SpawnParameters.Instigator = OwnerCharacter;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Projectile Spawn 
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass,
		SocketTransform.GetLocation(),
		SpawnRotation,
		SpawnParameters);
		
		// 발사체에 DamageEffectParmas 설정.
		FDamageEffectParams ProjectileDamageEffectParams = MakeDamageEffectParamsFromClassDefaults(TargetActor, FGameplayTagG::Get().Ability_BasicAttack);
		Projectile->SetDamageEffectParams(ProjectileDamageEffectParams);
	
		// 다음 Fire 이벤트를 받기위해 태스크 재생성.
		UAbilityTask_WaitGameplayEvent* WaitFireEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTagG::Get().Ability_BasicAttack);
		WaitFireEventTask->EventReceived.AddDynamic(this, &URevenantBasicAttack::OnFireEvent);
		WaitFireEventTask->ReadyForActivation();
	}
}
