#include "../Engine/Engine.h"
#include "../Game/Test/Week1TestMocks.h"
#include "../Game/Types/Events.h"
#include "Test.h"
#include "States.h"

#include "Dragon.h"
#include "Fighter.h"
#include "../Engine/GameObjectManager.h"
#include "GridPosition.h"
#include "StatsComponent.h"
#include "ActionPoints.h"
#include "SpellSlots.h"

Test::Test() : fighter(nullptr), dragon(nullptr)
{
}
void Test::Load() {
    AddGSComponent(new CS230::GameObjectManager());

    fighter = new Fighter({ 5, 5 });
    GetGSComponent<CS230::GameObjectManager>()->Add(fighter);

    dragon = new Dragon({ 6, 5 });
    GetGSComponent<CS230::GameObjectManager>()->Add(dragon);
    
    Engine::GetLogger().LogEvent("========== Combat Testbed Initialized ==========");
    Engine::GetLogger().LogEvent("'T' -> Fighter's Turn | 'Y' -> Dragon's Turn | 'D' -> Damage Dragon | 'H' -> Heal Fighter");
    LogFighterStatus();
    LogDragonStatus();

}
void Test::Update([[maybe_unused]] double dt) {
    
    // --- 입력 기반 테스트 로직 ---

    // 'T' 키: 파이터 턴 시작 (AI가 드래곤을 공격)
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::T)) {
        Engine::GetLogger().LogEvent("--- Player presses 'T': Starting Fighter's Turn ---");
        fighter->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus(); // 공격 후 드래곤의 상태 변화 확인
    }

    // 'Y' 키: 드래곤 턴 시작 (AI 작동)
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Y)) {
        Engine::GetLogger().LogEvent("--- Player presses 'Y': Starting Dragon's Turn ---");
        dragon->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus();
    }

    // 'D' 키: 드래곤에게 15 데미지
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::D)) {
        Engine::GetLogger().LogEvent("--- Player presses 'D': Testing PerformAttack ---");
        // 공격 전에 행동력이 있는지 간단히 확인
        if (fighter->GetActionPoints() > 0) {
            fighter->PerformAttack(dragon);
        }
        else {
            Engine::GetLogger().LogDebug("Fighter has no Action Points to attack!");
        }

        // 공격 후 두 캐릭터의 상태를 모두 로깅하여 결과 확인
        LogFighterStatus();
        LogDragonStatus();
    }

    // 'H' 키: 파이터를 10만큼 치유
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::H)) {
        Engine::GetLogger().LogEvent("--- Player presses 'H': Applying 10 Heal to Fighter ---");
        fighter->ReceiveHeal(10);
        LogFighterStatus();
    }

    // ESC 키: 메인 메뉴로
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

void Test::LogDragonStatus() {
    if (dragon == nullptr) return;
    Engine::GetLogger().LogDebug("========== Dragon Status Report ==========");

    GridPosition* grid_pos = dragon->GetGridPosition();
    StatsComponent* stats = dragon->GetStatsComponent();
    ActionPoints* ap = dragon->GetActionPointsComponent();
    SpellSlots* ss = dragon->GetSpellSlots();

    if (grid_pos != nullptr) { Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")"); }
    if (stats != nullptr) { Engine::GetLogger().LogDebug("HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP())); }
    if (ap != nullptr) { Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints())); }
    if (ss != nullptr) { Engine::GetLogger().LogDebug("Has Lvl 5 Slot: " + std::string(ss->HasSlot(5) ? "Yes" : "No")); }
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
    dragon = nullptr;
}