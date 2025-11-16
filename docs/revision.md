# Revision

## reorganizing & error check

Please document the software architecture, ensuring that information is well-organized and not duplicated across files.

- **check `architecture.md`  and `systems/characters.md`**, base character class is inheritated from CS230::GameObject. but it doesn't take adventage of inheritating  GameObject and using GameObjectManager. check files `CS230/Engine/GameObject.h`, `CS230/Game/Cat.h`, `CS230/Game/Cat.cpp`, `CS230/Game/mode1.h`, `CS230/Game/mode1.cpp` , to check how inherited gameobject class override and use gameobjectmanager like : 
  
  ```cpp
  //in cat
  Cat(Math::vec2 start_position,GameObject* standing_on);
  void Update(double dt) override;
  void Draw(Math::TransformationMatrix camera_matrix) override;
  GameObjectTypes Type() override { return GameObjectTypes::Cat; }
  std::string TypeName() override { return "Cat"; }
  bool CanCollideWith(GameObjectTypes other_object_type) override;
  const Math::vec2& GetPosition() const { return GameObject::GetPosition
  
  (); }
  void ResolveCollision(GameObject* other_object) override;
  
  enum class Animations {
   Idle,
   Running,
   Jumping,
   Falling,
   Skidding
  };
  void update_x_velocity(double dt);
  
  class State_Jumping : public CS230::GameObject::State {
  public:
      virtual void Enter(GameObject* object) override;
      virtual void Update(GameObject* object, double dt) override;
      virtual void CheckExit(GameObject* object) override;
      std::string GetName() override { return "Jumping"; }
  };
  
  State_Jumping state_jumping;
  
  class State_Idle : public CS230::GameObject::State {
  public:
      virtual void Enter(GameObject* object) override;
      virtual void Update(GameObject* object, double dt) override;
      virtual void CheckExit(GameObject* object) override;
      std::string GetName() override { return "Idle"; }
  };
  
  State_Idle state_idle;
  
  class State_Falling : public CS230::GameObject::State {
  public:
      virtual void Enter(GameObject* object) override;
      virtual void Update(GameObject* object, double dt) override;
      virtual void CheckExit(GameObject* object) override;
      std::string GetName() override { return "Falling"; }
  };
  
  State_Falling state_falling;
  
  class State_Running : public CS230::GameObject::State {
  public:
      virtual void Enter(GameObject* object) override;
      virtual void Update(GameObject* object, double dt) override;
      virtual void CheckExit(GameObject* object) override;
      std::string GetName() override { return "Running"; }
  };
  
  State_Running state_running;
  
  class State_Skidding : public CS230::GameObject::State {
  public:
      virtual void Enter(GameObject* object) override;
      virtual void Update(GameObject* object, double dt) override;
      virtual void CheckExit(GameObject* object) override;
      std::string GetName() override { return "Skidding"; }
  };
  
  State_Skidding state_skidding;
  //in mode1
  AddGSComponent(new CS230::GameObjectManager());
  cat_ptr = new Cat({ 300, floor },starting_floor_ptr);
  
  GetGSComponent<CS230::GameObjectManager>()->Add(cat_ptr);
  GetGSComponent<CS230::GameObjectManager>()->Add(new Asteroid({ 600, floor }));
  ```
  
  if you want to check another files to understand throughly, go ahead.

- **In `architecture.md`**, provide a high-level overview. For each system (component, interface, gscomponent, gocomponent), describe:
  
  - What it is and its purpose.
  
  - Its capabilities once implemented.
  
  - How all the systems interact with each other.
  
  - How inheritance will being happened.
  
  - How interface will be used & inheritated .

- **In separate markdown files within a `/systems` directory**, detail the specifics for each system, including:
  
  - Usage examples.
  
  - The expected structure of its header file.
    
    The goal is to **refactor the documentation by removing any duplicated content and ensuring everything is well-organized, while preserving the existing high-quality explanations**

## parallelism

We are a team of five developers, so our implementation plan must support parallel work. The current `implementation-plan.md` has too many dependencies, which will create significant bottlenecks where four developers are blocked waiting for one person's task to finish.

As our `game_architecture_rules.md` states, services must be as independent as possible to allow for parallel development. If a dependency is unavoidable, the component must at least be independently testable.

Please revise `implementation-plan.md` with this in mind. Afterwards, update `PHASE1_IMPLEMENTATION.md` based on the new plan. but don't mess up `PHASE1_IMPLEMENTATION.md`  's original structure like - Classes to Implement, Implementation Tasks, Verification Checklist,Dependencies, Example JSON Structure,Example C++ Usage , Debug Console Commands, File Structure After this task.
