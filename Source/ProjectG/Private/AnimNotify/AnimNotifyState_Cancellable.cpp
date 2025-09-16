// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_Cancellable.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagG.h"


void UAnimNotifyState_Cancellable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor)
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);
		if (ASC)
		{
			ASC->AddLooseGameplayTag(FGameplayTagG::Get().Character_State_AttackCancellable);
		}
	}
}

void UAnimNotifyState_Cancellable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor)
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(FGameplayTagG::Get().Character_State_AttackCancellable);
		}
	}
}
