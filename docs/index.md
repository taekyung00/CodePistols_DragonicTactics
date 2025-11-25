# Dragonic Tactics - Documentation Index

**Project**: Turn-based Tactical RPG built on CS230 Engine
**Timeline**: 26-week development cycle
**Status**: Architecture & Planning Phase

---

## Quick Navigation

### Core Documentation

1. **[Class List](class-list.md)** - ⭐ **NEW!** Complete reference of all ~50 classes to implement
2. **[Architecture Overview](architecture.md)** - High-level system design, class hierarchies, architectural patterns
3. **[Implementation Plan](implementation-plan.md)** - ⭐ **UPDATED!** 26-week domain-based parallel development plan
4. **[Game Design](../architecture/dragonic_tactics.md)** - Original game design document (Korean)

### System Specifications

#### Character & Combat Systems

- **[Character System](systems/characters.md)** - Character classes (Dragon, Fighter, Cleric, Wizard, Rogue) with GameObject implementation
- **[Singleton Services](systems/singletons.md)** - Global services (DiceManager, EventBus, DataRegistry, AIDirector, EffectManager, SaveManager)
- **[EventBus System](systems/eventbus.md)** - ⭐ **NEW!** Detailed event-driven communication specification with usage examples
- **[GameState Components](systems/game-state-components.md)** - Battle-wide systems (BattleManager, TurnManager, GridSystem, CombatSystem, SpellSystem, AISystem, StatusEffectManager)
- **[GameObject Components](systems/game-object-components.md)** - Individual character behaviors (GridPosition, ActionPoints, SpellSlots, StatusEffects)
- **[Interfaces & Factories](systems/interfaces.md)** - Interface design patterns and factory implementations

### Debug Tools

- **[Debug Tools Specification](debug/tools.md)** - Visual debug overlays, console system, god mode, hot reloading
- **[Debug Command Examples](debug/commands.md)** - Implementation examples for all console commands
- **[Debug UI Layout](debug/ui.md)** - Visual design for debug overlays and console window

### Supporting Documents

- **[Architecture Principles](../architecture/game_architecture_rules.md)** - 5 core architectural principles (Korean)
- **[CS230 Engine Components](../ENGINE_COMPONENTS.md)** - Complete reference of existing CS230 engine capabilities

---

## Documentation Structure

```
docs/
├── index.md                          # This file - main navigation
├── class-list.md                     # ⭐ NEW: Complete class reference
├── architecture.md                   # High-level architecture (703 lines)
├── implementation-plan.md            # ⭐ UPDATED: Domain-based parallel plan
├── systems/
│   ├── characters.md                 # Character implementations (705 lines)
│   ├── singletons.md                 # Singleton services (755 lines)
│   ├── eventbus.md                   # ⭐ NEW: EventBus detailed spec (1,050 lines)
│   ├── game-state-components.md      # All 7 GSComponent systems (1,690 lines) ✅
│   ├── game-object-components.md     # GOComponent behaviors (693 lines)
│   └── interfaces.md                 # Interfaces & factories (296 lines)
└── debug/
    ├── tools.md                      # Debug system spec (782 lines)
    ├── commands.md                   # Command examples (783 lines)
    └── ui.md                         # UI layout (648 lines)
```

**Total**: ~9,700+ lines of comprehensive, organized documentation

---

## File Size Reference

Most files under 850 lines, eventbus.md and game-state-components.md are comprehensive

| File                              | Lines | Topic                      |
| --------------------------------- | ----- | -------------------------- |
| architecture.md                   | 703   | Architecture overview      |
| implementation-plan.md            | 821   | Development timeline       |
| systems/characters.md             | 705   | Character classes          |
| systems/singletons.md             | 755   | Global services            |
| systems/eventbus.md               | 1,050 | Event-driven communication |
| systems/game-state-components.md  | 1,690 | All 7 Battle systems       |
| systems/game-object-components.md | 693   | Character components       |
| systems/interfaces.md             | 296   | Patterns & factories       |
| debug/tools.md                    | 782   | Debug tools                |
| debug/commands.md                 | 783   | Debug commands             |
| debug/ui.md                       | 648   | Debug UI design            |

---

## Cross-References

### External Links

- Files reference each other using relative markdown links
- Example: `See [Character System](systems/characters.md) for implementations`
- Debug docs link to main architecture: `See [Architecture](../architecture.md)`

### Navigation Tips

- All links use relative paths for portability
- Click file links to jump directly to relevant sections
- Use browser back button to return to index

---

## For Claude/LLM Context

When working on this project, recommended reading order:

1. **Quick reference**: [class-list.md](class-list.md) - ⭐ See all classes at a glance
2. **Start here**: [architecture.md](architecture.md) - Understand overall design
3. **Specific systems**: Navigate to relevant `systems/*.md` file
4. **Implementation timing**: [implementation-plan.md](implementation-plan.md) - Domain-based parallel plan
5. **Debug support**: [debug/tools.md](debug/tools.md) - Development tools

**Key principle**: Use class-list.md as your navigation hub. It shows all ~50 classes organized by category with developer assignments.
