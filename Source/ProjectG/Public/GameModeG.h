// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeG.generated.h"

/**
 * GameMode[Server]
 * 새 플레이어 접속 > PlayerController생성 (이 플레이어는 지정한 HUD Class를 써야해)
 *
 * PlayerController[Client]
 * 서버가 지정한 HUD를 쓰자 (생성) > 화면에 표시(BeginPlay)
 *
 * <클래스 생성 순서>
 * 1.(서버) PlayerController 생성
 *
 * 2.(서버) PlayerController가 초기화하면서 PlayerState 생성
 *	 (엔진) 엔진은 이미 PlayerState가 Replicated가 필요한 액터임을 알고있음.
 *	 (엔진) 서버가 PlayerState를 모든 클라이언트에게 만들라고 명령함.
 *	 (클라이언트) 서버에서 생성된 PlayerState에 대한 복사본을 로컬에 저장.
 *	 
 * 3.OnPostLogin 호출(서버) 
 *
 * 4.HUD 생성(클라이언트)
 */

UCLASS()
class PROJECTG_API AGameModeG : public AGameModeBase
{
	GENERATED_BODY()
		
};
