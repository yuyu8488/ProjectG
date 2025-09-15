// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyCharacter.h"

#include "AbilitySystem/AbilitySystemComponentG.h"
#include "AbilitySystem/AttributeSetG.h"
#include "Components/WidgetComponent.h"
#include <UI/Widget/UserWidgetG.h>



// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentG>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAttributeSetG>("AttributeSet");

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("HealthBarWidget");
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetActive(false);
	
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystem();

	// HealthBarWidgetComponent 초기화
	HealthBarWidgetComponent->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	if (UUserWidgetG* HealthBarWidget = Cast<UUserWidgetG>(HealthBarWidgetComponent->GetUserWidgetObject()))
	{
		HealthBarWidget->AssignWidgetController(this);
	}

	if (AbilitySystemComponent && AttributeSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Cast<UAttributeSetG>(AttributeSet)->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) 
			{
				// HP 변화에 따라 상태 업데이트

				// 데미지 처음 적용시 HealthBarWidgetComponent 활성화하게 코드 변경
				if (Data.NewValue < Data.OldValue)
				{
					// 체력이 감소했을 때 HealthBarWidget 활성화
					OnEnemyHealthChanged.Broadcast(Data.NewValue);
					HealthBarWidgetComponent->SetActive(true);
					HealthBarWidgetComponent->GetWidget()->SetVisibility(ESlateVisibility::Visible);
				}

				// 딱 0이되었을때도 HealthBarWidgetComponent 비활성화되게 변경
				if (Data.NewValue <= 0.01f)
				{
					// 체력이 0 이하가 되었을 때 HealthBarWidget 비활성화
					HealthBarWidgetComponent->SetActive(false);
					HealthBarWidgetComponent->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
				}
			});
	}
}

void AEnemyCharacter::UpdateTargets_Implementation()
{
	Super::UpdateTargets_Implementation();
}

void AEnemyCharacter::UpdatePrimaryTarget_Implementation()
{
	Super::UpdatePrimaryTarget_Implementation();
}

void AEnemyCharacter::InitAbilitySystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
	InitCharacterAbility();
	InitCharacterAttributes();	
}
