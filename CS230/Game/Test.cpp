#include "../Engine/Engine.h"
#include "../Game/Test/Week1TestMocks.h"
#include "../Game/Types/Events.h"
#include "Test.h"
#include "States.h"


#include "Fighter.h"
#include "../Engine/GameObjectManager.h"
#include "GridPosition.h"
#include "StatsComponent.h"
#include "ActionPoints.h"


Test::Test() : fighter(nullptr)
{
}
void Test::Load() {
    AddGSComponent(new CS230::GameObjectManager());

    fighter = new Fighter({ 3, 5 });
    GetGSComponent<CS230::GameObjectManager>()->Add(fighter);
    
    Engine::GetLogger().LogEvent("========== Fighter Testbed Initialized ==========");
    Engine::GetLogger().LogEvent("Press 'D' to Damage, 'H' to Heal, 'T' to Start Turn.");
    LogFighterStatus();

}
void Test::Update([[maybe_unused]] double dt) {
    

    //test_subscribe_publish_singleSubscriber();

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::D)) {
        Engine::GetLogger().LogEvent("--- Player presses 'D': Applying 10 Damage ---");
        fighter->TakeDamage(10, nullptr); // 공격자는 테스트 목적이므로 nullptr
        LogFighterStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::T)) {
        Engine::GetLogger().LogEvent("--- Player presses 'T': Starting Fighter's Turn ---");
        fighter->OnTurnStart(); // 턴 시작 이벤트 호출
        LogFighterStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::H)) {
        Engine::GetLogger().LogEvent("--- Player presses 'H': Applying 10 Heal ---");
        fighter->ReceiveHeal(10);
        LogFighterStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::A)) {
        Engine::GetLogger().LogEvent("--- Player presses 'A': Consuming 1 Action Point ---");
        ActionPoints* ap = fighter->GetActionPointsComponent();
        if (ap != nullptr) {
            if (ap->Consume(1)) {
                Engine::GetLogger().LogDebug("Action Point consumed successfully.");
            }
            else {
                Engine::GetLogger().LogDebug("Not enough Action Points to consume.");
            }
        }
        LogFighterStatus();
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {

        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}

void Test::Draw() {
    Engine::GetWindow().Clear(0x1a1a1aff);
}

void Test::LogFighterStatus() {
    if (fighter == nullptr) return;

    // 컴포넌트들 가져오기
    GridPosition* grid_pos = fighter->GetGridPosition();
    StatsComponent* stats = fighter->GetStatsComponent();
    ActionPoints* ap = fighter->GetActionPointsComponent();

    // 로그 출력
    Engine::GetLogger().LogDebug("========== Fighter Status Report ==========");

    // 위치 정보
    if (grid_pos != nullptr) {
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    }

    // 스탯 정보
    if (stats != nullptr) {
        std::string hp_status = "HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP());
        hp_status += " (" + std::to_string(stats->GetHealthPercentage() * 100.0f) + "%)";
        Engine::GetLogger().LogDebug(hp_status);
        Engine::GetLogger().LogDebug("IsAlive: " + std::string(stats->IsAlive() ? "true" : "false"));
        Engine::GetLogger().LogDebug("Speed: " + std::to_string(stats->GetSpeed()));
    }

    // 턴 자원 정보
    if (ap != nullptr) {
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    }

    // 현재 상태 (State Machine)
    // TODO: fighter->GetCurrentStateName() 같은 함수가 Character에 추가되면 활성화
    // Engine::GetLogger().LogDebug("Current State: " + fighter->GetCurrentStateName());

    Engine::GetLogger().LogDebug("==========================================");
}

//void Test::test_subscribe_publish_singleSubscriber()
//{
//    auto& eventbus = Engine::GetEventBus();
//    eventbus.Clear();
//
//    bool callbackInvoked = false;
//    //int receivedDamage = 0;
//    MockCharacter* receivedTarget = nullptr;
//
//    MockCharacter character("TestDragon");
//    const int damage = 30;
//    CharacterDamagedEvent event{ reinterpret_cast<Character*>(&character),damage,70,nullptr,false };
//    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
//        callbackInvoked = true;
//        character.SetHP(character.GetCurrentHP() - e.damageAmount);
//        receivedTarget = reinterpret_cast<MockCharacter*>(e.target);
//        });
//    
//    eventbus.Publish(event);
//
//    ASSERT_TRUE(callbackInvoked);
//    ASSERT_EQ(character.GetCurrentHP(), event.remainingHP);
//    ASSERT_EQ(receivedTarget, &character);
//
//}

void Test::Unload() {
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    fighter = nullptr;
}