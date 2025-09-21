// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemComponentG.h"

#include "AbilitySystem/AbilityInputID.h"
#include "AbilitySystem/GameplayAbilityG.h"
#include "Net/UnrealNetwork.h"


void UAbilitySystemComponentG::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilitySystemComponentG, AbilityComboCounter);
}

void UAbilitySystemComponentG::OnAbilityActorInfoSet()
{
}

void UAbilitySystemComponentG::InitAbility(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant)
{
	for (const auto& AbilityClass : AbilitiesToGrant)
	{
		/*
		 * FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		 * Spec은 어빌리티의 설계도나 등록 정보와 같다. 이 시점에서 어빌리티 객체(인스턴스)는 생성되지 않는다.
		 * AbilitySpec.Ability는 Spec에 의해 생성된 인스턴스를 가리키는 포인터이다. 이 포인터는 GiveAbility(AbilitySpec)함수가 호출된 후에 채워진다.
		 * FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
			if (const UGameplayAbilityG* AbilityG = Cast<UGameplayAbilityG>(AbilitySpec.Ability))
		 * Cast가 항상 실패한다?(...잘됬음) 
		*/
		
		if (!AbilityClass) continue;

		// 1. Get CDO(Class Default Object)
		const UGameplayAbilityG* DefaultAbility = AbilityClass->GetDefaultObject<UGameplayAbilityG>();
		if (!DefaultAbility)
		{
			// UGameplayAbilityG를 상속받지 않은 일반 어빌리티도 부여할 경우.
			UE_LOG(LogTemp, Warning, TEXT("Skipping non-GameplayAbilityG class: %s"), *AbilityClass->GetName());
			continue;
		}

		// 2. CDO에서 InputID를 읽어와 Spec 생성.
		FGameplayAbilitySpec AbilitySpec(
			AbilityClass,
			1,
			static_cast<int32>(DefaultAbility->Info.AbilityInputID),
			this);

		// 3. 어빌리티 등록
		GiveAbility(AbilitySpec);
		
		// 4. 콤보공격인 경우 AbilityComboCounter에 추가
		if (DefaultAbility->Info.bIsCombo)
		{
			AbilityComboCounter.AddUnique(FAbilityComboInfo(DefaultAbility->Info.AbilityInputID, 0));
		}
	}
}
 
void UAbilitySystemComponentG::AbilityInputIDPressed(const EAbilityInputID& InputID)
{
	switch (InputID)
	{
	case EAbilityInputID::None:{} break;
	case EAbilityInputID::BasicAttack:
		{
			TArray<FGameplayAbilitySpec*> MatchingSpecs;
			for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
			{
				if (Spec.InputID == static_cast<int32>(InputID))
				{
					MatchingSpecs.Add(&Spec);
				}
			}
		
			if (MatchingSpecs.Num() == 0) break;
			
			// 동일한 InputID를 가진 Ability 중에서 발동할 어빌리티 선별

			int32 ComboStep = 0;
			for (auto& Ability : AbilityComboCounter)
			{
				if (Ability.AbilityInputID == InputID)
				{
					ComboStep = Ability.ComboStep;
				}
			}

			FGameplayAbilitySpec* SpecToActivate = MatchingSpecs[ComboStep];
		
			AbilitySpecInputPressed(*SpecToActivate);
			if (SpecToActivate->IsActive())
			{
				InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputPressed,
				SpecToActivate->Handle,
				SpecToActivate->GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey());
			}
			else
			{
				TryActivateAbility(SpecToActivate->Handle);
			}
			break;
		}
	case EAbilityInputID::Test:
		{
			if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID(static_cast<int32>(InputID)))
			{
				//Ability에 입력이 눌렸다는 사실을 알림.
				AbilitySpecInputPressed(*AbilitySpec);
		
				if (AbilitySpec->IsActive())
				{			
					InvokeReplicatedEvent(
						EAbilityGenericReplicatedEvent::InputPressed,
						AbilitySpec->Handle,
						AbilitySpec->GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey());
				}
				else
				{
					TryActivateAbility(AbilitySpec->Handle);
				}
				break;
			}
			break;
		}
		
	default: {} break;
	}	
}

void UAbilitySystemComponentG::AbilityInputIDReleased(const EAbilityInputID& InputID)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID(static_cast<int32>(InputID));
	if (AbilitySpec && AbilitySpec->IsActive())
	{
		// Ability가 종료될 가능성이 있어서 필요한 정보를 미리 지역변수로 저장
		const FGameplayAbilitySpecHandle Handle = AbilitySpec->Handle;
		UGameplayAbility* GameplayAbility = AbilitySpec->GetPrimaryInstance(); //Instanced Per Actor옵션 아니면 null 오류발생함.
		
		FGameplayAbilityActivationInfo ActivationInfo = AbilitySpec->GetPrimaryInstance()->GetCurrentActivationInfo();
		const FPredictionKey PredictionKey = AbilitySpec->GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey();

		// 해제 신호 호출로 Ability가 종료될 수 있다. 
		AbilitySpecInputReleased(*AbilitySpec);

		// WaitInputRelease (블루프린트 함수)가 동작하려면 InvokeReplicatedEvent함수 호출 필요.
		// Ability가 실제로 종료됬는지와 상관없이 저장해둔 안전한 값으로 이벤트를 보냄.
		InvokeReplicatedEvent( EAbilityGenericReplicatedEvent::InputReleased, Handle,PredictionKey);
	}
}

void UAbilitySystemComponentG::AbilityInputIDHeld(const EAbilityInputID& InputID)
{
	//FindAbilitySpecFromInputID는 내부적으로 맵(TMap)을 사용하여 매우 빠르게 검색합니다.
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID(static_cast<int32>(InputID));

	if (AbilitySpec && AbilitySpec->IsActive() == false)
	{
		// 이 함수는 Ability가 입력이 눌린 상태임을 알아야 할때 사용.
		AbilitySpecInputPressed(*AbilitySpec);
		TryActivateAbility(AbilitySpec->Handle);
	}
}

void UAbilitySystemComponentG::SetAbilityComboCounterValue(EAbilityInputID InAbilityInputID, int32 InComboStep)
{
	for (auto& Ability : AbilityComboCounter)
	{
		if (Ability.AbilityInputID == InAbilityInputID)
		{
			Ability.ComboStep = InComboStep;
		}
	}
}
