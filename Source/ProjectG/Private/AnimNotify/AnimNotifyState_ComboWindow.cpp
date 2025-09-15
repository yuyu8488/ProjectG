// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_ComboWindow.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilitySystemComponentG.h"
#include "Character/CharacterG.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	/** 현재 콤보 확인
	* 캐릭터의 ASC의 AbilityComboCounter에서 태그로 단계확인 / 태그는 Info->AbilityInputID
	*
	* 캐릭터의 ASC의 변수를 수정하려면, AbilitySystemComponentG를 알아야하는데, 굳이 알게하고 싶지 않은데.... 
	* 
	* Ability_BasicAttack_ComboWindow의 태그는 애니메이션 몽타주에서 NotifyState Begin에서 부여하고, End에서 제거한다.
	*/
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (ACharacterG* OwnerCharacter = Cast<ACharacterG>(OwnerActor))
		{
			if (UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(OwnerCharacter->GetAbilitySystemComponent()))
			{
				ASC->AddLooseGameplayTag(WindowTag);
			}
		}
	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (ACharacterG* OwnerCharacter = Cast<ACharacterG>(OwnerActor))
		{
			if (UAbilitySystemComponentG* ASC = Cast<UAbilitySystemComponentG>(OwnerCharacter->GetAbilitySystemComponent()))
			{
				ASC->RemoveLooseGameplayTag(WindowTag);
			}
		}
	}
}
