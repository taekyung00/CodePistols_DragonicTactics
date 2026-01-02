# Playtest 1 Weekly Implementation Plans

This directory contains week-by-week breakdown of the Playtest 1 implementation (Weeks 1-5).

## Quick Navigation

| Week       | Focus                                  | File                 | Key Deliverables                                              |
| ---------- | -------------------------------------- | -------------------- | ------------------------------------------------------------- |
| **Week 1** | Foundation (All 5 Systems in Parallel) | [week1.md](week1.md) | Character, GridSystem, EventBus, DiceManager, DebugConsole    |
| **Week 2** | Dragon + Grid + Dice Integration       | [week2.md](week2.md) | Dragon class, A* pathfinding, TurnManager, GridDebugRenderer  |
| **Week 3** | Combat + Spells Begin                  | [week3.md](week3.md) | Fireball spell, CombatSystem, SpellSystem, Fighter class      |
| **Week 4** | More Spells + Turn System              | [week4.md](week4.md) | CreateWall + LavaPool spells, Initiative system, DataRegistry |
| **Week 5** | Polish + PLAYTEST 1 🧪                 | [week5.md](week5.md) | Balance tuning, BattleState integration, **First Playtest**   |

## Document Structure

Each week file contains:

- **Document Header**: Project overview, timeline, success criteria
- **Developer Tasks**: Detailed tasks for all 5 developers (A, B, C, D, E)
- **Implementation Details**: Code examples, data structures, algorithms
- **Testing Requirements**: Rigorous test suites with edge cases
- **Integration Tests**: Friday integration verification
- **Deliverables**: What must be complete by end of week

## File Statistics

| File      | Lines           | Content Size                      |
| --------- | --------------- | --------------------------------- |
| week1.md  | 1,940 lines     | Foundation systems (largest week) |
| week2.md  | 540 lines       | Dragon integration                |
| week3.md  | 542 lines       | Combat systems                    |
| week4.md  | 528 lines       | Spell expansion                   |
| week5.md  | 371 lines       | Polish + playtest                 |
| **Total** | **3,921 lines** | Complete Playtest 1 plan          |

## Related Documentation

- **Parent Document**: [PLAYTEST1_IMPLEMENTATION.md](../PLAYTEST1_IMPLEMENTATION.md) - Complete 5-week plan
- **Master Timeline**: [docs/implementation-plan.md](../../implementation-plan.md) - Full 26-week project
- **Architecture**: [docs/architecture.md](../../architecture.md) - System design overview
- **Systems Specs**: [docs/systems/](../../systems/) - Detailed component specifications

## Usage

### For Individual Developers

Read your assigned week file at the start of each week:

```bash
# Week 1 - All developers
cat weeks/week1.md

# Week 2 - Developer A example
cat weeks/week2.md | grep -A 50 "Developer A"
```

### For Team Leads

Use for weekly planning meetings:

1. **Monday Morning**: Review week file, assign tasks
2. **Daily Standups**: Reference deliverables for progress tracking
3. **Friday Integration**: Verify all deliverables complete

### For Code Reviews

Each week file includes test requirements - use these for PR validation:

```bash
# Check if Week 1 Character tests exist
grep "Test_TakeDamage_Fatal" CS230/Tests/CharacterTests.cpp
```

## Playtest 1 Success Criteria (Week 5)

By end of Week 5, the game must:

- ✅ Dragon spawns on 8x8 grid
- ✅ Dragon can move using pathfinding
- ✅ Dragon has 3 working spells (Fireball, CreateWall, LavaPool)
- ✅ Spells use dice-based damage
- ✅ Turn-based combat works (initiative, turn order)
- ✅ Fighter can attack back (manual control)
- ✅ Combat ends when Dragon or Fighter dies
- ✅ Debug console allows testing (`spawn`, `damage`, `castspell`)
- ✅ **Play for 5 minutes without crash**

---

**Last Updated**: 2025-10-06
**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Foundation Layer + Dragon Playable
