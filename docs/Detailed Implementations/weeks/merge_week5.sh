#!/bin/bash
# Script to merge all week5 sections into final week5.md

echo "Merging Week 5 documentation sections..."

# Navigate to the weeks directory
cd "$(dirname "$0")"

# Create backup of existing week5.md
if [ -f "week5.md" ]; then
    cp week5.md week5.md.backup
    echo "Created backup: week5.md.backup"
fi

# Merge all sections (keeping existing Developer A content)
cat week5_developer_b.md >> week5.md
echo "" >> week5.md

cat week5_developer_c.md >> week5.md
echo "" >> week5.md

cat week5_developer_d.md >> week5.md
echo "" >> week5.md

cat week5_developer_e.md >> week5.md
echo "" >> week5.md

cat week5_final_sections.md >> week5.md
echo "" >> week5.md

echo "✅ Merge complete!"
echo "Final week5.md created with all sections."
echo ""
echo "File contains:"
echo "  - Overview and Table of Contents"
echo "  - Developer A: Visual Effects & Animation"
echo "  - Developer B: Game Flow & Victory Conditions"
echo "  - Developer C: UI/UX Polish"
echo "  - Developer D: BattleState Integration"
echo "  - Developer E: Testing & Build Preparation"
echo "  - Week 5 Integration Tests"
echo "  - Deliverables & Verification"
echo "  - Playtest 1 Execution Plan"
echo ""
echo "Total lines:"
wc -l week5.md

echo ""
echo "To clean up temporary files, run:"
echo "  rm week5_developer_b.md week5_developer_c.md week5_developer_d.md week5_developer_e.md week5_final_sections.md week5_part2.txt"
