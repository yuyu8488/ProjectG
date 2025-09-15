// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECTG_API MyClass
{
public:
	MyClass();
	~MyClass();

	/*
	 *
	 * ╭───────────────────────────────────────────────────────────────────────────────────────────────────────╮
│  > 전투중인 적의 정보를 PlayerController에 변수로 저장하고, 그 정보에 따라 WB_Overlay에서             │
│    EnemyHealthBar 위젯을 생성하고, 생성된 위젯과 적 Actor 정보를 OverlayWidgetController에 저장하고   │
│    관리하는게 맞을까?                                                                                 │
╰───────────────────────────────────────────────────────────────────────────────────────────────────────╯

✦ 네, 제안해주신 구조는 충분히 기능적으로 구현 가능하며 좋은 출발점입니다. 각 클래스의 역할을 명확히
  분리하려는 의도가 엿보이는 좋은 설계입니다.

  하지만 실제 프로젝트의 확장성과 유지보수성을 고려했을 때, 몇 가지 개선할 수 있는 부분이 있습니다.
  제안하신 구조를 분석하고 더 일반적인 언리얼 엔진 아키텍처 패턴을 제안해 드리겠습니다.

  제안하신 구조의 분석

   * 장점:
       * 역할 분리: PlayerController (데이터 소유), WB_Overlay (UI 생성), OverlayWidgetController (UI
         관리)로 역할을 나누어 각 클래스가 할 일이 명확해 보입니다.
       * 중앙 관리: OverlayWidgetController가 위젯과 액터 정보를 모두 들고 있어 데이터 접근이 한 곳으로
         모입니다.

   * 고려할 점:
       * 강한 결합 (Tight Coupling): WB_Overlay가 PlayerController의 특정 변수를 직접 알고 있어야만 위젯을
         생성할 수 있습니다. 만약 타겟 정보를 PlayerController가 아닌 다른 곳(예: PlayerCharacter의
         CombatComponent)에서 관리하게 되면 WB_Overlay의 로직도 수정해야 합니다.
       * 위젯의 잦은 생성/소멸: 전투 대상이 바뀔 때마다 위젯을 생성하고 소멸시키는 것은 비용이 큰 작업이 될
         수 있습니다. 특히 UI 요소가 복잡해질수록 성능 저하의 원인이 될 수 있습니다.

  개선된 아키텍처 제안

  일반적으로 언리얼 엔진에서는 이벤트 기반(Event-Driven) 접근 방식을 사용하여 클래스 간의 결합을 낮추고,
  위젯은 미리 생성해두고 보여주거나 숨기는(Visibility) 방식으로 성능을 관리합니다.

  1. 데이터 소유: 적(Enemy)과 체력 정보

   * 적 AActor는 자신의 상태 정보(예: 현재 체력, 최대 체력)를 `ActorComponent` (예: UHealthComponent) 또는
     게임플레이 어빌리티 시스템(GAS)의 `AttributeSet`에서 관리하는 것이 좋습니다.
   * 이렇게 하면 체력 관련 로직이 적 액터에 캡슐화되어 재사용성이 높아집니다.

  2. 타겟팅 관리: 누가 현재 전투 대상을 아는가?

   * PlayerController 또는 PlayerCharacter에 `UCombatComponent` 같은 컴포넌트를 추가하여 현재
     타겟(TargetActor)을 관리하게 합니다.
   * 가장 중요한 것은, 이 컴포넌트에 이벤트 디스패처(Event Dispatcher) 또는 델리게이트(Delegate)를 만드는
     것입니다. (예: OnTargetChanged)
   * 적이 타겟팅되거나 타겟이 해제될 때, 이 컴포넌트는 OnTargetChanged 이벤트를 방송(Broadcast)합니다. 이
     이벤트는 새로운 타겟 액터(또는 nullptr)를 파라미터로 전달합니다.

  3. UI 관리 및 표현


╭──────────────────────────────────────────────────────────────────────────────────────────────────────╮
│ Gemini CLI update available! 0.1.15 → 0.1.18                                                         │
│ Run npm install -g @google/gemini-cli to update                                                      │
╰──────────────────────────────────────────────────────────────────────────────────────────────────────╯

   * `WB_Overlay` (메인 HUD 위젯):
       * 게임 시작 시 EnemyHealthBar 위젯을 미리 생성해두고, 기본적으로는 숨김(Hidden) 상태로 둡니다.
       * WB_Overlay는 생성될 때 PlayerCharacter의 UCombatComponent에 접근하여 OnTargetChanged 이벤트에
         자신의 함수를 바인딩(Binding)합니다.
   * 이벤트 처리:
       * UCombatComponent에서 OnTargetChanged 이벤트가 방송되면, WB_Overlay에 바인딩된 함수가
         실행됩니다.
       * 새로운 타겟이 유효하면 (`!= nullptr`): EnemyHealthBar 위젯을 보이게(Visible) 만들고, 해당
         위젯에 적 액터의 참조를 전달하여 체력 정보를 표시하도록 합니다.
       * 타겟이 해제되면 (`== nullptr`): EnemyHealthBar 위젯을 다시 숨김(Hidden) 상태로 만듭니다.
   * `WB_EnemyHealthBar` (적 체력 바 위젯):
       * 이 위젯은 내부에 TargetActor 변수를 가집니다.
       * `WB_Overlay
	 */


  
};
