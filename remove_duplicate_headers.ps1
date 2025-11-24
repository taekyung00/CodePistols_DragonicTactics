# PCH에 포함된 헤더들을 cpp 파일에서 제거하는 스크립트

# pch.h에 포함된 헤더 목록 (정규식 패턴)
$pchHeaders = @(
    # STL 헤더
    '^\s*#include\s+<vector>\s*$',
    '^\s*#include\s+<string>\s*$',
    '^\s*#include\s+<memory>\s*$',
    '^\s*#include\s+<map>\s*$',
    '^\s*#include\s+<unordered_map>\s*$',
    '^\s*#include\s+<set>\s*$',
    '^\s*#include\s+<array>\s*$',
    '^\s*#include\s+<functional>\s*$',
    '^\s*#include\s+<algorithm>\s*$',
    '^\s*#include\s+<iostream>\s*$',
    '^\s*#include\s+<fstream>\s*$',
    '^\s*#include\s+<sstream>\s*$',
    '^\s*#include\s+<cmath>\s*$',

    # 엔진 헤더
    '^\s*#include\s+"Engine/Engine\.hpp"\s*$',
    '^\s*#include\s+"Engine/GameObject\.h"\s*$',
    '^\s*#include\s+"Engine/GameState\.hpp"\s*$',
    '^\s*#include\s+"Engine/Component\.h"\s*$',
    '^\s*#include\s+"Engine/ComponentManager\.h"\s*$',
    '^\s*#include\s+"Engine/GameObjectManager\.h"\s*$',
    '^\s*#include\s+"Engine/GameStateManager\.hpp"\s*$',
    '^\s*#include\s+"Engine/Logger\.hpp"\s*$',
    '^\s*#include\s+"Engine/Input\.hpp"\s*$',
    '^\s*#include\s+"Engine/Window\.hpp"\s*$',
    '^\s*#include\s+"Engine/TextureManager\.hpp"\s*$',
    '^\s*#include\s+"Engine/Sprite\.h"\s*$',
    '^\s*#include\s+"Engine/Animation\.h"\s*$',

    # 수학 헤더
    '^\s*#include\s+"Engine/Vec2\.hpp"\s*$',
    '^\s*#include\s+"Engine/Matrix\.hpp"\s*$',
    '^\s*#include\s+"Engine/Rect\.hpp"\s*$',

    # 게임 타입 헤더
    '^\s*#include\s+"Game/DragonicTactics/Types/GameTypes\.h"\s*$',
    '^\s*#include\s+"Game/DragonicTactics/Types/CharacterTypes\.h"\s*$',
    '^\s*#include\s+"Game/DragonicTactics/Types/GameObjectTypes\.h"\s*$',
    '^\s*#include\s+"Game/DragonicTactics/Types/Events\.h"\s*$',

    # 외부 라이브러리
    '^\s*#include\s+<SDL\.h>\s*$',
    '^\s*#include\s+<imgui\.h>\s*$',
    '^\s*#include\s+<gsl/gsl>\s*$'
)

# main.cpp 제외하고 모든 cpp 파일 처리
Get-ChildItem -Path "DragonicTactics/source" -Recurse -Filter *.cpp |
    Where-Object { $_.Name -ne "main.cpp" } |
    ForEach-Object {
        $filePath = $_.FullName
        $originalContent = Get-Content $filePath
        $modifiedContent = @()
        $removedCount = 0
        $removedHeaders = @()

        foreach ($line in $originalContent) {
            $shouldRemove = $false

            # pch.h에 있는 헤더인지 확인
            foreach ($pattern in $pchHeaders) {
                if ($line -match $pattern) {
                    $shouldRemove = $true
                    $removedCount++
                    $removedHeaders += $line.Trim()
                    break
                }
            }

            # 제거하지 않을 라인은 추가
            if (-not $shouldRemove) {
                $modifiedContent += $line
            }
        }

        # 변경사항이 있으면 파일 업데이트
        if ($removedCount -gt 0) {
            Set-Content -Path $filePath -Value $modifiedContent
            Write-Host "[$($_.Name)] Removed $removedCount duplicate headers:" -ForegroundColor Green
            foreach ($header in $removedHeaders) {
                Write-Host "  - $header" -ForegroundColor Yellow
            }
        } else {
            Write-Host "[$($_.Name)] No duplicates found" -ForegroundColor Cyan
        }
    }

Write-Host "`nDone! All duplicate headers removed from cpp files." -ForegroundColor Green
