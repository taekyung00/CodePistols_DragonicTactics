# 🐉 Dragonic Tactics

## 🎮 How to Play

- Run the `.exe` file located in the game folder to play.

- **Main Menu Controls:** Left-click the 'Dragonic Tactics' title on the screen, or press the `Enter` key when the title turns green to enter the game.

## 📜 Game Rules & Win Condition

- A turn-based strategy game where the player and the enemy AI take alternating turns.

- When your turn ends, the enemies' turn begins, and the AI will advance to attack the player.

- **Win Condition:** Defeat all enemies on the map to claim victory!

## ⌨️ Shortcuts & Controls Summary

- **Left Mouse Click:** Select character/tile, move, use skills

- **Right Mouse Click:** Cancel skill range display

- **Enter:** Enter the game (when the title text on the main menu is green)

- **ESC:** Return to the main menu from the in-game screen

## 🕹️ System Guide

- **Check Status:** Hover your mouse cursor over the dragon or other characters to check their Health (HP), Action Points (AP), Movement (Speed), and Spell Slots.

- **Movement Range:** Hover the cursor over the dragon during its turn to see the tiles it can move to for that turn.

## 💧 Resource Management

- **Speed (Movement):** Click the dragon, then click a green tile to move. Speed points are consumed based on the distance traveled.

- **AP (Action Points):** Using attacks or skills consumes AP.

- **Spell Slots:** Consumed alongside AP when casting magic skills.

> **※ Note:** Speed and AP are fully restored at the start of each turn, but consumed Spell Slots do not recover automatically. Use them wisely!

### ✨ Upcasting

Some magic skills (from *Firebolt* to *Create Wall*), excluding basic attacks, can consume a higher-level spell slot to increase the skill's power. Check your limited spell slots and enhance your magic according to the situation.

## ⚔️ Skill List

Click the icons at the bottom of the screen (from left to right) to use your skills.

- **Basic Attack**

- **Firebolt (Level 1):** Single enemy target / Range: 4 tiles / **[Upcastable]**
  
  - Damage: 2d8 + (Consumed Spell Level - 1)d6

- **Tail Sweep (Level 2):** All enemies around the caster / Range: 2 tiles / **[Not Upcastable]**
  
  - Damage: 1d8
  
  - Special Effect: Hit enemies are pushed back 2 tiles away from the dragon.

- **Dragon's Wrath (Level 3):** All enemies in a straight line / Range: 4 tiles / **[Upcastable]**
  
  - Damage: 4d6 + (Consumed Spell Level - 3) * 2d6

- **Meteor (Level 3):** All enemies on the entire map / **[Upcastable]**
  
  - Damage: 3d20 + (Consumed Spell Level - 3) * 1d20
  
  - Special Effect: Grants the caster 'Exhaustion' for 1 turn.

- **Mana Conversion (Level 0):** Self / Range: 0 tiles / **[Upcastable]**
  
  - Effect: Deals (Consumed Spell Level + 1)d10 damage to yourself, but recovers one Spell Slot of (Consumed Spell Level + 1) level.

- **Purify (Level 1):** Self / Range: 0 tiles / **[Not Upcastable]**
  
  - Effect: Grants 'Purify' status to yourself for 1 turn.

- **Frightful Presence (Level 1):** All enemies around the caster / Range: 3 tiles / **[Not Upcastable]**
  
  - Effect: Inflicts 'Fear' status on all enemies in range for 3 turns.

- **Lava Spit (Level 2):** Target empty tile / Range: 6 tiles / **[Upcastable]**
  
  - Effect: Summons a lava zone at the selected location. Creates additional tiles equal to (Consumed Spell Level - 2).

- **Create Wall (Level 1):** Target empty tile / Range: 5 tiles / **[Upcastable]**
  
  - Effect: Summons a wall at the selected location. Creates additional tiles equal to (Consumed Spell Level - 1).

- **End Turn**

## 💫 Status Effects

Buffs and debuffs applied by character skills or specific conditions.

- **Lifesteal:** Restores HP equal to half of all damage dealt this turn. (Rounded down)

- **Frenzy:** If the next attack deals 10 or more damage, the hit enemy receives a random detrimental status effect (Curse, Fear, or Exhaustion, excluding Frenzy). If the attack fails to deal 10+ damage, the caster (Fighter) receives the random detrimental effect instead.

- **Exhaustion:** Speed and AP drop to 0 on the next turn.

- **Purify:** Immediately removes all status effects applied to yourself.

- **Blessing:** All damage taken is reduced by 3, and all damage dealt is increased by 3.

- **Curse:** All damage taken is increased by 3, and all damage dealt is reduced by 3.

- **Haste:** Speed increases by 1, and AP increases by 1.

- **Stealth:** Cannot be targeted by enemies, and the first damage dealt on the next turn is doubled. Cannot be used after attacking, and attacking immediately breaks Stealth.

- **Fear:** All damage dealt is reduced by 3, and Speed is reduced by 1.

## ⚠️ Known Issues / Patch Notes

- In-game BGM overlapping continuously when returning to the main menu by pressing ESC **(Resolved)**

- Skill range UI remaining on the screen when using a basic attack without canceling the previous skill's range display with a right-click **(Resolved)**

- Character sprites not disappearing from the screen even when their HP reaches 0 **(Resolved)**

- Basic attack range not visually displayed **(Resolved)**

- Enemies falling into a stunned state instead of the player (caster) when using Meteor **(Resolved)**

## 👥 Credits

- **Producer:** Taekyung Heo (Voice Act: Cleric)

- **Tech Lead:** Junyoung Ki

- **Test Lead:** Kinam Park

- **QA:** Sangyoon Lee

- **Designer:** Seungju Song (Voice Act: Dragon, Fighter)