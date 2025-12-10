# Dragonic Tactics - 구현 체크리스트

**최종 업데이트**: 2025-12-09
**문서 버전**: 2.0

---

## 범례

- ✅ **완료** (Week 1-3 구현 완료)
- ⏳ **진행중** (Week 4-5 작업중)
- 📋 **계획** (Week 6+ 예정)

---

## 1. Singleton Services (StateComponents)

| 서비스 | 목적 | 핵심 기능 | 상태 |
|--------|------|----------|------|
| **EventBus** | 시스템 간 통신 | 타입 안전 이벤트 발행/구독 | ✅ 완료 |
| **DiceManager** | 주사위 굴림 | D&D 표기법 파싱 (3d6+5) | ✅ 완료 |
| **DataRegistry** | JSON 데이터 관리 | Hot-reloadable 외부 설정 | ✅ 완료 |
| **AIDirector** | 전역 AI 조율 | 팀 전술 및 난이도 조정 | 📋 계획 (Week 8+) |
| **EffectManager** | 시각 효과 | 주문 애니메이션 및 전투 피드백 | 📋 계획 (Week 10+) |
| **SaveManager** | 캠페인 영속화 | 세이브/로드 게임 상태 | 📋 계획 (Week 12+) |

---

## 2. GameState Components (전투 시스템)

| 컴포넌트 | 전투에서의 역할 | 상태 |
|----------|----------------|------|
| **BattleManager** | 승리 조건, 전투 플로우 조율 | 📋 계획 (Week 6+) |
| **TurnManager** | 이니셔티브 순서, 행동력 할당 | ✅ 완료 |
| **GridSystem** | 8x8 타일 검증, 경로찾기, 시야 | ✅ 완료 |
| **CombatSystem** | 데미지 계산, 공격 해결, 밀쳐내기 | ✅ 완료 |
| **SpellSystem** | 주문 시전, 효과, 슬롯 관리, 업캐스팅 | ✅ 완료 |
| **AISystem** | AI 의사결정, 팀 조율, 난이도 | ✅ 완료 |
| **StatusEffectManager** | 버프/디버프 추적, 스탯 수정, 효과 타이머 | 📋 계획 (Week 6+) |

---

## 3. GameObject Components (캐릭터 컴포넌트)

| 컴포넌트 | 부착 대상 | 목적 | 상태 |
|----------|----------|------|------|
| **GridPosition** | 모든 캐릭터 | 타일 기반 위치, 경로찾기 캐시 | ✅ 완료 |
| **ActionPoints** | 모든 캐릭터 | 턴 기반 행동 경제 | ✅ 완료 |
| **StatsComponent** | 모든 캐릭터 | HP, 공격력, 방어력, 속도 | ✅ 완료 |
| **SpellSlots** | Dragon, Wizard, Cleric | 마법 자원 추적 (업캐스팅) | ✅ 완료 |
| **MovementComponent** | 모든 캐릭터 | 이동 애니메이션 및 경로 | ✅ 완료 |
| **StatusEffects** | 모든 캐릭터 | 개별 버프/디버프 컬렉션 | 📋 계획 (Week 6+) |
| **AIMemory** | AI 캐릭터 | 전술 의사결정 히스토리 | 📋 계획 (Week 8+) |
| **DamageCalculator** | 모든 캐릭터 | 주사위 기반 전투 해결 | 📋 계획 (Week 7+) |
| **TargetingSystem** | 모든 캐릭터 | 공격/주문 범위 검증 | 📋 계획 (Week 7+) |

---

## 4. 캐릭터 시스템

### 4.1 플레이어 캐릭터

| 캐릭터 | 스탯 | 구현 상태 |
|--------|------|----------|
| **Dragon** | HP 140, 속력 5, 행동력 2, 공격 3d6, 방어 2d8 | ✅ 완료 |
| └─ 주문 슬롯 | 레벨 1-5 (4/3/2/2/1개) | ✅ 완료 |
| └─ 구현된 주문 | 벽 생성, 화염탄, 용암 뿌리기 | ✅ 완료 (3/9) |
| └─ 나머지 주문 6개 | 용의 포효, 꼬리 휘두르기, 용의 분노, 운석 소환, 열 흡수, 열 방출 | 📋 계획 (Week 6-8) |

### 4.2 AI 캐릭터 (모험가)

| 캐릭터 | 스탯 | 역할 | AI 전략 | 상태 |
|--------|------|------|---------|------|
| **Fighter** | HP 90, 속력 3, 행동력 2 | 탱커, 어그로 담당 | FighterStrategy | ✅ 완료 |
| **Cleric** | HP 90, 속력 2, 행동력 1 | 힐러, 버프/디버프 | ClericStrategy | ⏳ 작업중 (Week 4-5) |
| **Wizard** | HP 55, 속력 1, 행동력 1 | 원거리 딜러 | WizardStrategy | ⏳ 작업중 (Week 4-5) |
| **Rogue** | HP 65, 속력 4, 행동력 1 | 기습 암살자 | RogueStrategy | ⏳ 작업중 (Week 4-5) |

---

## 5. AI 시스템 (Strategy 패턴)

| AI 전략 | 의사결정 로직 | 플로우차트 | 상태 |
|---------|--------------|-----------|------|
| **IAIStrategy** | AI 전략 인터페이스 | - | ✅ 완료 |
| **FighterStrategy** | 보물 운반, HP 관리, 근접 공격 | `fighter.mmd` | ✅ 완료 |
| **ClericStrategy** | 아군 치유 우선, 버프 시전, 안전 포지셔닝 | `cleric.mmd` | ⏳ 작업중 |
| **WizardStrategy** | 거리 유지, 원거리 주문, 안전 지대 활용 | `wizard.mmd` | ⏳ 작업중 |
| **RogueStrategy** | 은신, 기습 공격, 보물 훔치기 | `rouge.mmd` | ⏳ 작업중 |

**플로우차트 위치**: `architecture/character_flowchart/*.mmd`

---

## 6. 주문 & 능력 시스템

### 6.1 Dragon 주문 (총 9개)

| 이름 | 레벨 | 효과 | 범위 | 업캐스팅 | 상태 |
|------|------|------|------|---------|------|
| **벽 생성** | 1 | 1x1 벽 생성 | 5칸 이내 | X | ✅ 완료 |
| **화염탄** | 1 | 2d8 피해 | 5칸 이내 | +1d6/레벨 | ✅ 완료 |
| **용암 뿌리기** | 2 | 2x2 용암 (5턴간 8 피해/턴) | 5칸 이내 | 피해 +2, 지속 +1턴 | ✅ 완료 |
| **용의 포효** | 2 | 모든 적에게 공포 | 3칸 이내 | 범위 +1칸, 지속 +1턴 | 📋 계획 (Week 6) |
| **꼬리 휘두르기** | 2 | 1d8 피해 + 2칸 밀쳐내기 | 전방 3칸 | X | 📋 계획 (Week 6) |
| **용의 분노** | 3 | 4d6 피해 | 4칸 이내 | +2d6/레벨 | 📋 계획 (Week 7) |
| **운석 소환** | 3 | 3d20 피해, 다음턴 스킵 | 홀수/짝수 칸 | +1d20/레벨 | 📋 계획 (Week 7) |
| **열 흡수** | 4 | 화상 치료, 용암 제거로 스탯 상승 | 모든 칸 | X | 📋 계획 (Week 8) |
| **열 방출** | 5 | 최대 HP 절반 피해, 레벨 3 이하 슬롯 회복 | X | X | 📋 계획 (Week 8) |

### 6.2 Ability 시스템

| 클래스 | 설명 | 상태 |
|--------|------|------|
| **Action** | 액션 베이스 클래스 | ✅ 완료 |
| **ActionAttack** | 기본 공격 액션 | ✅ 완료 |
| **AbilityBase** | 능력 베이스 인터페이스 | 📋 계획 (Week 6) |
| **MeleeAttack** | 근접 공격 능력 | 📋 계획 (Week 6) |
| **ShieldBash** | 방패 밀치기 능력 | 📋 계획 (Week 6) |
| **RangedAttack** | 원거리 공격 능력 | 📋 계획 (Week 6) |

---

## 7. 상태 이상 시스템

| 상태 이상 | 효과 | 구현 상태 |
|----------|------|----------|
| **화상 (Burn)** | 턴 시작 시 지속 피해 | ✅ 완료 (용암 주문) |
| **공포 (Fear)** | 공격력 감소, 속력 감소 | 📋 계획 (용의 포효) |
| **저주 (Curse)** | 받는 피해 증가, 주는 피해 감소 | 📋 계획 (Week 6+) |
| **속박 (Bind)** | 이동 불가 | 📋 계획 (Week 6+) |
| **실명 (Blind)** | 모든 범위 감소 | 📋 계획 (Week 6+) |
| **축복 (Blessing)** | 받는 피해 감소, 주는 피해 증가 | 📋 계획 (Week 6+) |
| **신속 (Haste)** | 속력 증가, 행동력 증가 | 📋 계획 (Week 6+) |

**구현 계획**: `StatusEffectManager` (Week 6+)

---

## 8. 디버그 & 도구

| 도구 | 목적 | 상태 |
|------|------|------|
| **DebugConsole** | 명령어 실행 및 테스트 | ✅ 완료 |
| **DebugManager** | 디버그 모드 관리 | ✅ 완료 |
| **DebugVisualizer** | 그리드 시각화 | ✅ 완료 |
| **GridDebugRenderer** | 그리드 및 이동 범위 오버레이 | ⏳ 작업중 (Week 5) |
| **AIDebugVisualizer** | AI 경로 및 위협도 시각화 | ⏳ 작업중 (Week 5) |
| **GamePlayUIManager** | 실제 플레이 UI (HP, AP, 턴 순서) | ⏳ 작업중 (Week 5) |
| **DebugUIManager** | ImGui 기반 개발자 패널 | ⏳ 작업중 (Week 5) |
| **StatusInfoOverlay** | HP/AP 바 UI | 📋 계획 (Week 6+) |
| **CollisionDebugRenderer** | 히트박스 시각화 | 📋 계획 (Week 7+) |
| **GodModeManager** | 무적 모드 (테스트용) | 📋 계획 (Week 8+) |
| **HotReloadManager** | JSON 핫리로드 | 📋 계획 (Week 8+) |

---

## 9. 렌더링 시스템

| 렌더러 | 설명 | 성능 | 상태 |
|--------|------|------|------|
| **IRenderer2D** | 렌더러 인터페이스 | - | ✅ 완료 |
| **ImmediateRenderer2D** | 즉시 모드 렌더링 | 낮음 | ✅ 완료 |
| **BatchRenderer2D** | 배치 렌더링 | 중간 | ✅ 완료 |
| **InstancedRenderer2D** | GPU 인스턴싱 | 높음 | ✅ 완료 |
| **런타임 전환** | TextureManager를 통한 전환 | - | ✅ 완료 |

---

## 10. 팩토리 패턴

| 팩토리 | 생성 대상 | 데이터 소스 | 상태 |
|--------|----------|------------|------|
| **CharacterFactory** | Dragon, Fighter, Cleric, Wizard, Rogue | `characters.json` | ✅ 완료 (Dragon, Fighter) |
| **SpellFactory** | 모든 주문 | `spells.json` | 📋 계획 (Week 7+) |
| **EffectFactory** | 버프/디버프 효과 | `effects.json` | 📋 계획 (Week 8+) |
| **MapFactory** | 8x8 그리드 레이아웃 | `maps.json` | 📋 계획 (Week 5-6) |

---

## 11. 진행 상황 요약

### ✅ 완료 (Week 1-3)

- **StateComponents**: 8개 (EventBus, DiceManager, DataRegistry, GridSystem, TurnManager, CombatSystem, SpellSystem, AISystem)
- **GameObject Components**: 5개 (GridPosition, ActionPoints, StatsComponent, SpellSlots, MovementComponent)
- **캐릭터**: 2개 (Dragon, Fighter)
- **AI 전략**: 1개 (FighterStrategy)
- **주문**: 3개 (벽 생성, 화염탄, 용암 뿌리기)
- **렌더러**: 3개 (Immediate, Batch, Instanced)
- **디버그 도구**: 3개 (Console, Manager, Visualizer)

### ⏳ 진행중 (Week 4-5)

- **턴 플로우 시스템 명확화**
- **AI 전략 3개** (ClericStrategy, WizardStrategy, RogueStrategy)
- **UI 시스템** (GamePlayUIManager, DebugUIManager)
- **소유권 모델 재설계** (스마트 포인터)
- **맵 데이터 로딩** (MapDataRegistry, maps.json)
- **AI 행동 시각화** (pause 시스템)

### 📋 계획 (Week 6+)

- **캐릭터**: 3개 (Cleric, Wizard, Rogue)
- **주문**: 6개 (나머지 Dragon 주문)
- **Ability 시스템** (AbilityBase, MeleeAttack, ShieldBash 등)
- **StatusEffect 시스템** (상태 이상 6가지)
- **보물 시스템** (획득, 운반, 탈출)
- **고급 AI** (AIDirector, 팀 전술, Bias 시스템)
- **UI/UX 시스템** (메인 메뉴, 옵션, 튜토리얼)
- **사운드 시스템** (BGM, 효과음)
- **캠페인 시스템** (여러 스테이지, 성장, 세이브/로드)

---

## 12. 개발 메트릭

| 카테고리 | 완료 | 진행중 | 계획 | 총합 | 진행률 |
|----------|------|--------|------|------|--------|
| StateComponents | 8 | 0 | 4 | 12 | 67% |
| GameObject Components | 5 | 0 | 4 | 9 | 56% |
| 캐릭터 | 2 | 0 | 3 | 5 | 40% |
| AI 전략 | 1 | 3 | 0 | 4 | 25% |
| 주문 | 3 | 0 | 6 | 9 | 33% |
| 디버그 도구 | 3 | 4 | 4 | 11 | 27% |

**전체 진행률**: 약 **40%** (Week 1-5 / Week 1-26)

---

**문서 생성일**: 2025-12-09
**담당**: 프로젝트 리더 및 전체 팀
**참조**: [docs/architecture.md](../docs/architecture.md), [implementation-plan.md](../docs/implementation-plan.md)
