# source/

All C++ source for Dragonic Tactics, split into a reusable engine and the game built on top of it.

| Path | Layer | Contents |
|---|---|---|
| [`Engine/`](Engine/) | Engine | Core engine services: `Engine`/`GameStateManager` (app loop + state stack), `GameObject`/`GameObjectManager`, the two component systems (GameState components and GameObject components), input, camera, fonts/text, sound, textures, screenshots, timers. |
| [`CS200/`](CS200/) | Engine | 2D rendering abstraction (`IRenderer2D`) with Immediate/Batch/Instanced backends, plus the ImGui integration used by debug tools. |
| [`OpenGL/`](OpenGL/) | Engine | Thin OpenGL wrapper — buffers, shaders, textures, framebuffers, vertex arrays — that `CS200` renders through. |
| [`Game/`](Game/) | Game | Everything specific to Dragonic Tactics. |
| `main.cpp` | — | Entry point: starts the engine, registers a crash-dump handler (Windows), and pushes the first `Splash` game state. |

`Engine`, `CS200`, and `OpenGL` form the custom C++20 OpenGL engine the game is built on; `Game/` is the only layer that knows about dragons, spells, or turn order.

## Game/

| Path | Contents |
|---|---|
| `Splash.*`, `MainMenu.*`, `Settings.*`, `LevelSelect.*`, `GameOver.*`, `States.h` | The shell layer — menus, splash/cutscenes, settings, and level select. Each is a `GameState` pushed/popped via `GameStateManager`. |
| `Background.*`, `Score.*`, `Particles.h` | Small `CS230::Component`s shared by the shell states (not game states themselves). |
| [`DragonicTactics/`](Game/DragonicTactics/) | The battle itself — everything that runs once `GamePlay` is pushed. |

### Game/DragonicTactics/ (the battle system)

| Path | Contents |
|---|---|
| `States/` | `GamePlay` (the central game state — see its own doc comment for a map of the whole battle flow), plus `PlayerInputHandler`, `BattleOrchestrator`, `GamePlayUIManager`, `ButtonManager`. |
| `StateComponents/` | GameState-scoped systems: `GridSystem`, `TurnManager`, `CombatSystem`, `SpellSystem`, `StatusEffectHandler`, `AISystem` (+ `AI/` with one strategy class per character archetype), `EventBus`, `DiceManager`, `DataRegistry`, and others. |
| `Objects/` | `Character` and its subclasses (`Dragon`, `Fighter`, `Cleric`, `Rogue`, `Wizard`), `Actions/` (the attack-action system), and `Components/` (GameObject-scoped components like `StatsComponent`, `MovementComponent`, `StatusEffectComponent`). |
| `Factories/` | `CharacterFactory` — the only sanctioned way to construct a character. |
| `Types/` | Shared enums/structs: `CharacterTypes`, `Events.h`, `GameTypes.h`, `GameTimings.h`. |
| `Debugger/` | `DebugManager`, `DebugConsole`, `DebugVisualizer` — the in-game debug console and overlay tools (F1). |
| `External/` | Vendored `json.hpp` (nlohmann/json). |

For how these pieces actually fit together at runtime (component registration order, event flow, AI decision loops, spell/status data formats, timing constants, and so on), see **[CLAUDE.md](../../CLAUDE.md)** at the repository root — it's the authoritative, continuously-updated systems reference for this codebase.
