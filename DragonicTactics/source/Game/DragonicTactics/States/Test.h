#pragma once
#include "../StateComponents/GridSystem.h"
#include "../../../Engine/GameState.hpp"
class Dragon;
class Fighter;
class Test : public CS230::GameState
{
public:
    Test();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    void     test_subscribe_publish_singleSubscriber();
    void     test_multiple_subscribers_sameEvent();
    void     test_multiple_different_events();
    void     test_EventData_CompleteTransfer();
    void     test_EventData_MultiplePublishes();
    Fighter* fighter;
    Dragon*  dragon;
    void     LogFighterStatus();
    void     LogDragonStatus();
};
class Test2 : public CS230::GameState
{
    public:
    Test2();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;
private:
    void     Test2_subscribe_publish_singleSubscriber();
    void     Test2_multiple_subscribers_sameEvent();
    void     Test2_multiple_different_events();
    void     Test2_EventData_CompleteTransfer();
    void     Test2_EventData_MultiplePublishes();
    Fighter* fighter;
    Dragon*  dragon;
    void     LogFighterStatus();
    void     LogDragonStatus();
};