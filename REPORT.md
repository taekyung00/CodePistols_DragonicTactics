2025-10-06
오늘은 클래스를 근간이 되는 캐릭터 클래스를 생성했음
아직 구현은 안했고 대충 껍데기만 만듦
게임오브젝트타입이 enum class에 캐릭터만 추가할지, 위저드 다 하나하나씩 추가할지 고민중 
제미나이는 게임 오브젝트 파일을 수정하거나 컴포넌트로 타입을 관리할수있다고함

2025-10-07
Character.h를 조금 더 자세하게 작성함
모든 캐릭터가 따라야 할 행동 규약(인터페이스)인 OnTurnStart, TakeDamage 등을 순수 가상 함수로 정의하여, 자식 클래스가 반드시 고유한 로직을 구현하도록 강제하는 구조를 만듦.

캐릭터의 기본 능력치를 관리할 CharacterStats 구조체를 정의.
AI의 전략적 판단을 담당할 DecideAction 함수를 인터페이스로 추가하고, 판단 결과(목표 대상, 목표 종류)를 저장할 m_turn_target, m_turn_goal 변수를 마련하여 나중에 AI 시스템 개발할때 편하게 하려고 함

게임의 범용 규칙과 타입 중앙 관리 (GameTypes.h, CharacterTypes.h):
TurnGoal, Direction처럼 게임 전반에서 사용될 수 있는 범용 enum들을 GameTypes.h로 분리하여 중앙에서 관리하도록 함.
Fighter, Wizard 등 캐릭터의 직업 종류는 CharacterTypes.h로 분리함.

캐릭터 행동 상태(State)의 기반 마련 (CharacterStates.h, CharacterStates.cpp):
AI의 목표에 따라 실제 행동을 수행할 State_Idle, State_Moving, State_Attacking, State_TurnEnd 상태 클래스들을 CharacterStates.h에 선언.
CharacterStates.cpp에 각 상태의 실제 로직이 구현될 틀을 마련하여, 이제 각 상태가 어떤 일을 할지 구체적인 코드만 채워 넣으면 되도록 준비함.

필요없는 코드들 전부 지움

+ 허태경한테 10 9 8 7 코드 받는대로 map에 캐릭터 아무거나 하나 띄우도록 할 예정

2025-10-08
머지 꼬여서 그거 풀고 리포 다시 만들고 함

2025-10-09 
태경이 코드 다시 받아서 머지, 캐릭터 업데이트

2025-10-10
Character.h
캐릭터의 스탯을 관리하는 class StatsComponent를 만들고 전방선언
그렇기 때문에 기존 멤버 변수인 m_stats는 삭제
IsAlive와 GetStats 같이 m_stats를 반환하는 함수는 껍데기만 선언해서 cpp파일에서 구현
GetStatsComponent 함수 생성
이니셜라이즈 컴포넌트에 시작좌표를 받아서 생성하는걸로 만듦

GridPosition.h , .cpp
이건 캐릭터의 좌표를 다루는 컴포넌트임
생성자에서 좌표를 받아 저장해서 데이터를 다룸
간단한 Getter, Setter를 가지고 있음
TODO : 업데이트 함수 만들기

StatsComponent.h, cpp
이건 캐릭터의 전투에 관한 컴포넌트임
데미지를 받는 함수, 힐을 받는 함수, 생존 판정, 현재 체력 getter, 최대 체력 getter, 남은 체력의 퍼센테이지를 반환하고
다른 스탯 Getter도 만듦

CharacterTypes.h
캐릭터 스탯 구조체를 그냥 따로 빼서 관리하기로 함
attack_dice나 defend_dice를 string으로 만들었는데
이렇게 하는게 나중에 txt나 json 파일로 만들어 파싱하기 좋다고 함


Engine\GameObject.h
GetGOComponent의 const 버전을 만듦

Character.cpp
생성자를 만들고 컴포넌트 초기화 함수를 만듦
업데이트, 드로우기타 껍데기 함수도 만듦
컴포넌트들을 더 만들면 업데이트

앞으로 할 일
더 많은 컴포넌트 만들기
-> 자원관리 -> 액션시스템 -> 상태이상 -> AI 보조 컴포넌트 순서로 만들 예정