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