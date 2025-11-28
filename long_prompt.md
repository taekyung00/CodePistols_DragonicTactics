지금 @architecture/REFACTORING_TODO.md  를 끝냈어. 그 전에는 @docs/architecture.md  중에서 어떤것을 먼저 해야할지 모르겠어서 너한테 뭐부터 구현해야할지 순서를 맡겼고 너가 자동으로 @docs/implementation-plan.md  을 작성했지. 이제 어느정도 게임 토대가 만들어졌고, 우리는 이후에 뭐부터 구현하고 싶은지 방향이 정해졌어. 앞으로 @docs/implementation-plan.md은 ,@docs/architecture.md  를 기반으로, 우리가 원하는 방향에 따라서 언제든지 reorder 되도록 유연하게 작성되기를 원해. 일단 우리가 뭐부터 구현하고싶은지 우선순위를 정해주면, 그것이 향후에 문제가 될지 너가 평가해주고, 너와 내가 합의를 끝내면 그것을 토대로 @docs/implementation-plan.md을 작성하는 형식이야. 이런식의 재작성은 언제든지 발생할 수 있어. 그래서 @docs/implementation-plan.md은 @docs/architecture.md의 모든 계획을 담고있되, 향후 1달정도의 계획은 아주 자세히 작성되어있고, 나머지는 간략하게 작성되어있도록 하고싶어, 언제든지 순서가 변경될 수 있으니까. 물론 모두 한글로 작성해줘. 지금 현재, 우리가 우선적으로 구현하고 싶은건 (우선도에 따라 순서대로 작성함): 

1. (due to dec. 9) - 문제제기 : 매 턴마다 캐릭터가 해야하는것(ap를 리셋한다던가, speed를 리셋한다던가, ...) 이러한 행위가 무조건 되어야함. 이것을 제대로 call 되었는지 log로 확인해야함. 그리고 불필요한 함수들은 삭제되어야 함. 예시 -> 각 캐릭터마다 ap->refresh를 해주는 함수인 onturnstart가 있지만 call되지 않음, 대신 turnmanager의 StartNextTurn()에서 currentChar->RefreshActionPoints();로 그 역할을 대신함. 중복되는 기능 중 하나는 삭제되어야함. 

이런 중복되는 함수가 있는 문제점, 해야할 일을 실제로 했는지를 알기 어려운 문제를 근본적으로 해결하기 위해서 , 매 턴의 시작, 턴 안에서 해야할 일, 턴 종료 후 해야할 일들을 정리한 플로우 차트를 먼저 함께 작성하기를 바람. 나와 합의를 거쳐서 완성된 플로우 차트의 각 항목과, 실제로 그 일을 하는 함수가 "일대일대응"되어야 함. 현재 debug 과정에서는 실제로 그 함수가 잘 call되었는지를 알기위해 __pretty_function__매크로와 logger를 이용해서 확인하기를 바람.

2. (due to dec. 9)  - aisystem이  robust하게 완성되기를 원함(기획당시 작성된 각각의 플로우차트를 완벽히 대변하지 못함). 4명의 모험가(ai가 자동으로 플레이하는 캐릭터들)가 공통으로 가지는 aisystem과 각자의 차별되는 행동들을 잘 지원하도록 구현되어야함. 공통적으로 추가해야하는 부분과, 각자 개인이 추가되는 부분들 모두 쉽게 추가될 수 있어야 하며, 추가하더라도 기본적인 플로우를 해치는 것을 지양해야함(꼭 필요한 것이 아니라면). 각자의 분기에서 bias가 개입 될수도, 안될수도있음. 이것을 유연하게 전환할 수 있어야함.

3. (due to dec. 9) - 게임 내 필요한 정보 표시(현재는 imgui를 이용해 표시하는중) - 원하면 언제든지 쉽게 켜고 끌 수 있어야함

4. (due to dec. 9) - 캐릭터의 ownership을 분명히 해야함. 팩토리에서 소유하는것이 맞는가? 아니면 어디서? 그건 아직 잘 모르겠음. share해야하는지도 잘 모르겠음. 그리고 스마트포인터를 이용해서 raii의 이점을 활용하길 바람.

5. (due to dec. 9) - playable 캐릭터는 플레이어가 인풋을 넣고 그것에 따라 행동을 도출하므로 상관없지만, ai-based 캐릭터는 행동 중간중간에 pause가 없이 바로 행동하면 플레이어 입장에서 어떤 일이 일어났는지 파악하기 힘듦. 따라서 ai의 행동 중간중간에 pause가 존재해야하며 그 pause의 시간은 쉽게 조정할 수 있어야함.

6. (due to dec. 9) - 맵을 dataregistry를 이용해서 json파일을 파싱해서 정보를 받아와서 실제로 그려야함. 현재는 gameplay에서 스트링의 벡터로 하드코딩되어있는 맵 데이터를 이용해 타일정보를 설정함. 일단 맵 정보를 파싱하는것을 우선 구현하고, 이후에 타일정보를 설정하는 로직도 필요에 따라 재작성되기를 원함(안할수도 있음).

12월 9일까지의 역할분배 :
1 -> 1명
2 -> 1명
3 -> 1명
4 -> 1명
5, 6 ->1명
이 점들을 참고하여 implement-plan을 한글로 재작성하길 바람. 

이런식으로 앞으로 주기적으로 @docs/implementation-plan.md를 재작성할것임. 그리고 이렇게 완성된 @docs/implementation-plan.md을 기반으로 한 주 분량의detailed plan을 매 주 작성할 예정임. 현재 @docs/Detailed Implementations/weeks/week4.md  까지 작성되어 있으며, 영어로 작성되어 있음. 앞으로는 한글로 작성되길 바람. @docs/Detailed Implementations/weeks/week4.md에서 알 수있듯이 현재 plan의 구조는 
-implemantation tasks
-implemantation example
-rigorous test
-usage example
구조를 따르고 있음. 이 구조를 그대로 유지하길 바람. 5명이서 개발중이며 그점을 참고할것.
이 점들을 참고하여 week5을 작성하길 바람.
