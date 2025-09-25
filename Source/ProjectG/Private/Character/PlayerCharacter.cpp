// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"

#include "AbilitySystem/AbilitySystemComponentG.h"
#include "Camera/CameraComponent.h"
#include "Character/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
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

	// 서버측 초기화 
	InitAbilitySystem();
	
	InitCharacterAbility();
	InitCharacterAttributes();
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라이언트측 초기화
	InitAbilitySystem();

}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, LastInputDirection);
}

void APlayerCharacter::UpdateTargets_Implementation()
{
	CombatTargets.Empty();
	
	// 일정반경에 있는 모든 적을 배열에 저장.
	TArray<AActor*> FoundEnemyActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
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
	// TODO: 이동입력이 없을떄? PrimaryTarget 선정하는 부분 다시 고려하기. PrimaryTarget이 있는경우 무조건 타겟으로 되게 하는게 좋을거 같다. 
	// 이동입력이 없을때는 Alignment 점수계산은 빼는게 좋을듯?
	
	/* 이동입력이 없다
	 * 1. Primary Target이 존재하는가? 
	 *  ->(1) 존재한다. >> 
	 *  ->(2) 존재하지 않는다. >> 
	 */

	AActor* BestTarget = nullptr;
	float BestScore = -1.f;

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
			const float AngleInRadians = FMath::DegreesToRadians(TargetSearchFieldOfView * 0.5f); // <135도---|----135도> 중심으로 부터 최대 각도 
			const float AlignmentForTargeting = FMath::Cos(AngleInRadians);
			if (AlignmentDot < AlignmentForTargeting) continue; 

			// 거리가 멀어질수록 1에서 0에 가까워지는 점수 계산
			// 최종 점수 계산 (거리 + 입력방향) * (타겟인경우)
			const float DistanceScore = 1.f - (DistanceToEnemy / TargetSearchRadius);
			const float EnemyScore = (Enemy == PrimaryCombatTarget) ?
			((DistanceScore * TargetSearchDistanceWeight) + (AlignmentDot * TargetSearchAlignmentWeight)) * (1 + TargetSearchPrimaryTargetWeight)
				: (DistanceScore * TargetSearchDistanceWeight) + (AlignmentDot * TargetSearchAlignmentWeight);
					
			if (EnemyScore > BestScore)
			{
				BestScore = EnemyScore;
				BestTarget = Enemy;
			}
		}
		PrimaryCombatTarget = BestTarget;
		/*====================점수 테스트용 텍스트 출력=================*/
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
				1.0f,
				false,
				1.5f);
		}*/
		/*====================점수 테스트용 텍스트 출력=================*/
	}
}

void APlayerCharacter::SetLastInputDirection(const FVector& InDirection)
{
	// 클라이언트에서 먼저 로컬로 업데이트하여 즉각적인 반응 (클라이언트 예측)
	LastInputDirection = InDirection;

	// 서버로 값 전송(클라이언트인 경우에만)
	// GetLocalRole() : 모든 액터에 대해 ROLE_Authority를 반환한다.
	// ROLE_Authority : 서버만이 이 권한을 가진다.
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetLastInputDirection(InDirection);
	}
}

void APlayerCharacter::Server_SetLastInputDirection_Implementation(const FVector& InDirection)
{
	LastInputDirection = InDirection;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::InitAbilitySystem()
{
	 // State에 AbilitySystemComponent와 AttributeSet를 두는 이유
	 // 캐릭터가 죽으면 정보가 사라지지만, State에 저장된 정보는 세션종료시까지 유지됨.
	
	APlayerStateG* PlayerStateG = GetPlayerState<APlayerStateG>();
	if (PlayerStateG == nullptr) return;
	
	PlayerStateG->GetAbilitySystemComponent()->InitAbilityActorInfo(PlayerStateG, this);
	AbilitySystemComponent = PlayerStateG->GetAbilitySystemComponent();
	AttributeSet = PlayerStateG->GetAttributeSet();
}


