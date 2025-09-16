// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"

#include "AbilitySystem/AbilitySystemComponentG.h"
#include "Camera/CameraComponent.h"
#include "Character/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerControllerG.h"
#include "Player/PlayerStateG.h"
#include "UI/MainHUD.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetUsingAbsoluteRotation(false);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bDoCollisionTest = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true; //캐릭터 이동방향으로 자동 회전
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = false; // 캐릭터 XY평면 고정
	GetCharacterMovement()->bSnapToPlaneAtStart = true; 
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// ASC->InitAbilityActorInfo,
	// 서버에서 초기화
	InitAbilitySystem();
	
	/** ASC->InitAbility */
	InitCharacterAbility();
	InitCharacterAttributes();
}

void APlayerCharacter::OnRep_PlayerState()
{
	// 멀티환경에서 클라이언트도 AbilitySystemComponent를 초기화가 되도록 
	Super::OnRep_PlayerState();

	InitAbilitySystem();
}

void APlayerCharacter::UpdateTargets_Implementation()
{
	// 성능을 위해 이전에 찾은 목록은 비움? 
	CombatTargets.Empty();

	// 주변 일정반경에 있는 모든 적을 찾음
	TArray<AActor*> FoundEnemyActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery3);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		TargetSearchRadius,
		ObjectTypes,
		AEnemyCharacter::StaticClass(),
		ActorsToIgnore,
		FoundEnemyActors);

	CombatTargets = FoundEnemyActors;
}

void APlayerCharacter::UpdatePrimaryTarget_Implementation()
{	
	AActor* BestTarget = nullptr;
	float BestScore = -1.f;


	//TODO: 이동입력이 없을떄? PrimaryTarget 선정하는 부분 다시 고려하기. PrimaryTarget이 있는경우 무조건 타겟으로 되게 하는게 좋을거 같다. 
	if (LastInputDirection == FVector::ZeroVector)
	{
		//const FVector NormalizedForwardDirection = GetActorForwardVector().GetSafeNormal();
		const FVector NormalizedInputDirection = LastInputDirection.GetSafeNormal();

		for (AActor* Enemy : CombatTargets)
		{
			if (!IsValid(Enemy)) continue;

			const FVector DirectionToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			const float DistanceToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).Size();

			// 캐릭터 정면 방향 설정한 각도 내 있는 적만 고려 x
			// 이동 인풋방향 설정한 각도 내 있는 적 고려.
			const float AlignmentDot = FVector::DotProduct(NormalizedInputDirection, DirectionToEnemy.GetSafeNormal());
			const float AngleInRadians = FMath::DegreesToRadians(TargetSearchFieldOfView / 2.f); // <135도---|----135도> 중심으로 부터 최대 각도 
			const float AlignmentForTargeting = FMath::Cos(AngleInRadians);
			if (AlignmentDot < AlignmentForTargeting) continue;

			// 거리가 멀어질수록 1에서 0에 가까워지는 점수 계산
			// 최종 점수 계산 (거리 + 입력방향) * (Primary 타겟인경우)
			const float DistanceScore = 1.f - (DistanceToEnemy / TargetSearchRadius);
			const float EnemyScore = (Enemy == PrimaryCombatTarget) ?
			((DistanceScore * TargetSearchDistanceWeight) + (AlignmentDot * TargetSearchAlignmentWeight)) * (1 + TargetSearchPrimaryTargetWeight)
				: (DistanceScore * TargetSearchDistanceWeight) + (AlignmentDot * TargetSearchAlignmentWeight);

			if (EnemyScore > BestScore)
			{
				BestScore = EnemyScore;
				BestTarget = Enemy;
			}

			{			
				FString DebugText = FString::Printf(
					TEXT("Distance Score: %.2f\n Align Score: %.2f\n Score: %.2f"),
					DistanceScore,AlignmentDot,EnemyScore);
				FColor DebugTextColor = FColor::White;
				if (Enemy == PrimaryCombatTarget.Get())
				{
					DebugText += TEXT("\n** PRIMARY TARGET **");
					DebugTextColor =FColor::Red; 
				}
				
				DrawDebugString(
					GetWorld(),
					Enemy->GetActorLocation() + FVector(0.f,0.f, 100.f),
					DebugText,
					nullptr,
					DebugTextColor,
					3.0f,
					false,
					1.5f);
			}
		}
		PrimaryCombatTarget = BestTarget;

		
	}
	
	// 2.이동 입력이 있을때, 점수 기반으로 우선타겟을 찾는다.
	else
	{
		// 입력방향은 Controller에서 월드 공간으로 계산했음 -> 정규화
		const FVector NormalizedInputDirection = LastInputDirection.GetSafeNormal(); 
	
		for (AActor* Enemy : CombatTargets)
		{ 
			if (!IsValid(Enemy)) continue;

			const FVector DirectionToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			const float DistanceToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).Size();

			// 입력 방향으로 설정한 각도에 있는 적만 고려
			// 두 벡터의 크기가 모두 1일때, 내적의 결과는 두 벡터가 이른 각도의 코사인 값과 같다. (결과는 -1, 1 사잇값 / 그래프를 보면 알수 있다 / -90도~90도 양수 >> 정면)
			const float AlignmentDot = FVector::DotProduct(NormalizedInputDirection, DirectionToEnemy.GetSafeNormal());
			const float AngleInRadians = FMath::DegreesToRadians(TargetSearchFieldOfView / 2.f); // <135도---|----135도> 중심으로 부터 최대 각도 
			const float AlignmentForTargeting = FMath::Cos(AngleInRadians);
			if (AlignmentDot < AlignmentForTargeting) continue; 

			// 거리가 멀어질수록 1에서 0에 가까워지는 점수 계산
			// 최종 점수 계산 (거리 + 입력방향) * (타겟인경우)
			const float DistanceScore = 1.f - (DistanceToEnemy / TargetSearchRadius);
			const float EnemyScore = (Enemy == PrimaryCombatTarget) ?
			((DistanceScore * TargetSearchDistanceWeight) + (AlignmentDot * TargetSearchAlignmentWeight)) * (1 + TargetSearchPrimaryTargetWeight)
				:(DistanceScore * TargetSearchDistanceWeight) + (AlignmentDot * TargetSearchAlignmentWeight);

			{			
				FString DebugText = FString::Printf(
					TEXT("Distance Score: %.2f\n Align Score: %.2f\n Score: %.2f"),
					DistanceScore,AlignmentDot,EnemyScore);
				FColor DebugTextColor = FColor::White;
				if (Enemy == PrimaryCombatTarget)
				{
					DebugText += TEXT("\n** PRIMARY TARGET **");
					DebugTextColor =FColor::Red; 
				}
				
				DrawDebugString(
					GetWorld(),
					Enemy->GetActorLocation() + FVector(0.f,0.f, 100.f),
					DebugText,
					nullptr,
					DebugTextColor,
					3.0f,
					false,
					1.5f);
			}

			/*====================점수 테스트용 텍스트=================*/
			/*{			
				FString DebugText = FString::Printf(
					TEXT("Distance Score: %.2f\n Align Score: %.2f\n Score: %.2f"),
					DistanceScore,AlignmentDot,EnemyScore);
				FColor DebugTextColor = FColor::White;
				if (Enemy == PrimaryCombatTarget)
				{
					DebugText += TEXT("\n** PRIMARY TARGET **");
					DebugTextColor =FColor::Red; 
				}
				
				DrawDebugString(
					GetWorld(),
					Enemy->GetActorLocation() + FVector(0.f,0.f, 100.f),
					DebugText,
					nullptr,
					DebugTextColor,
					3.0f,
					false,
					1.5f);
			}*/
			/*====================점수 테스트용 텍스트=================*/
		
			if (EnemyScore > BestScore)
			{
				BestScore = EnemyScore;
				BestTarget = Enemy;
			}
		}
		PrimaryCombatTarget = BestTarget;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::InitAbilitySystem()
{
	/*
	 * State에 AbilitySystemComponent와 AttributeSet를 두는 이유
	 * 캐릭터가 죽으면 정보가 사라지지만, State에 저장된 정보는 세션종료시까지 유지됨.
	 */
	
	
	/* 멀티환경에서 PlayerController가 Null 오류
	 * LocalController가 아닌 다른 플레이어의 컨트롤도 가져오기때문
	 */
	
	APlayerStateG* PlayerStateG = GetPlayerState<APlayerStateG>();
	if (PlayerStateG == nullptr) return;
	PlayerStateG->GetAbilitySystemComponent()->InitAbilityActorInfo(PlayerStateG, this);
	
	// ASC,AS Caching
	AbilitySystemComponent = PlayerStateG->GetAbilitySystemComponent();
	AttributeSet = PlayerStateG->GetAttributeSet();
	
	if (APlayerControllerG* PlayerControllerG = GetController<APlayerControllerG>())
	{
		if (!PlayerControllerG->IsLocalController()) return;
		AMainHUD* MainHUD = Cast<AMainHUD>(PlayerControllerG->GetHUD());
		if (MainHUD)
		{
			MainHUD->InitOverlay(PlayerControllerG,PlayerStateG,AbilitySystemComponent,AttributeSet);
		}
	}
}


