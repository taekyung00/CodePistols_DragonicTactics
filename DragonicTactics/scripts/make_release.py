#!/usr/bin/env python3
"""
make_release.py — 플레이테스트 릴리즈 ZIP 생성 (선택적 GitHub Release 업로드)

터미널 사용:
  python scripts/make_release.py --version v0.3.0
  python scripts/make_release.py --version v0.3.0 --upload

더블클릭 실행:
  버전과 옵션을 프롬프트로 입력
"""

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

# PyInstaller --onefile exe는 __file__이 임시 압축 해제 경로를 가리키므로
# sys.executable(exe 자체 경로)을 사용해 실제 위치를 계산한다.
if getattr(sys, "frozen", False):
    SCRIPT_DIR = Path(sys.executable).resolve().parent.parent  # scripts/../ = DragonicTactics/
else:
    SCRIPT_DIR = Path(__file__).resolve().parent.parent        # DragonicTactics/

PROJECT_ROOT = SCRIPT_DIR.parent                               # 프로젝트 루트

BUILD_DIR  = SCRIPT_DIR / "build" / "windows-release"
ASSETS_SRC = SCRIPT_DIR / "Assets"
README_EN  = SCRIPT_DIR / "README.md"
README_KOR = SCRIPT_DIR / "README(KOR).md"

BINARIES = [
    "dragonic_tactics.exe",
    "OpenAL32.dll",
    "SDL2.dll",
    "soft_oal.dll",
]


# ---------------------------------------------------------------------------
# 사전 점검 및 자동 설치
# ---------------------------------------------------------------------------

def _refresh_path():
    """winget 설치 후 새 PATH를 현재 프로세스에 반영한다."""
    try:
        result = subprocess.run(
            ["powershell", "-NoProfile", "-Command",
             "[System.Environment]::GetEnvironmentVariable('PATH','Machine') + ';' + "
             "[System.Environment]::GetEnvironmentVariable('PATH','User')"],
            capture_output=True, text=True
        )
        if result.returncode == 0:
            os.environ["PATH"] = result.stdout.strip()
    except Exception:
        pass


def _is_available(cmd: str) -> bool:
    return shutil.which(cmd) is not None


def _winget_install(package_id: str, display_name: str):
    print(f"  {display_name} 설치 중 (winget) ...")
    result = subprocess.run(
        ["winget", "install", "--id", package_id,
         "--accept-source-agreements", "--accept-package-agreements"],
    )
    if result.returncode != 0:
        print(f"\n[오류] {display_name} 설치 실패. 수동으로 설치 후 다시 실행하세요.")
        input("Enter 키를 눌러 종료...")
        sys.exit(1)
    _refresh_path()


def preflight_check(need_upload: bool):
    """필요한 도구가 설치되어 있는지 확인하고, 없으면 자동 설치한다."""
    print("=== 사전 점검 ===")
    all_ok = True

    # cmake
    if _is_available("cmake"):
        print("  [OK] cmake")
    else:
        print("  [없음] cmake — 설치를 시도합니다.")
        _winget_install("Kitware.CMake", "CMake")
        if _is_available("cmake"):
            print("  [OK] cmake 설치 완료")
        else:
            print("  [오류] cmake를 찾을 수 없습니다. 터미널을 재시작한 뒤 다시 시도하세요.")
            all_ok = False

    # gh (업로드 시에만 필요)
    if need_upload:
        if _is_available("gh"):
            # 로그인 상태 확인
            auth = subprocess.run(["gh", "auth", "status"], capture_output=True, text=True)
            if auth.returncode == 0:
                print("  [OK] gh (GitHub CLI, 로그인됨)")
            else:
                print("  [주의] gh가 설치되어 있지만 로그인되어 있지 않습니다.")
                print("         GitHub 인증을 시작합니다...")
                subprocess.run(["gh", "auth", "login"])
                # 재확인
                auth2 = subprocess.run(["gh", "auth", "status"], capture_output=True)
                if auth2.returncode != 0:
                    print("  [오류] GitHub 로그인에 실패했습니다.")
                    all_ok = False
                else:
                    print("  [OK] gh 로그인 완료")
        else:
            print("  [없음] gh (GitHub CLI) — 설치를 시도합니다.")
            _winget_install("GitHub.cli", "GitHub CLI")
            _refresh_path()
            if _is_available("gh"):
                print("  [OK] gh 설치 완료 — GitHub 인증을 시작합니다...")
                subprocess.run(["gh", "auth", "login"])
            else:
                print("  [오류] gh를 찾을 수 없습니다. 터미널을 재시작한 뒤 다시 시도하세요.")
                all_ok = False

    if not all_ok:
        input("\nEnter 키를 눌러 종료...")
        sys.exit(1)

    print()


# ---------------------------------------------------------------------------
# 빌드 / 스테이징 / ZIP / 업로드
# ---------------------------------------------------------------------------

def build():
    print("=== 1/3  릴리즈 빌드 ===")
    subprocess.run(["cmake", "--preset", "windows-release"], cwd=SCRIPT_DIR, check=True)
    subprocess.run(["cmake", "--build", "--preset", "windows-release"], cwd=SCRIPT_DIR, check=True)


def stage(tmp_root: Path) -> Path:
    print("=== 2/3  스테이징 ===")
    stage_dir = tmp_root / "CODEPISTOLS_DRAGONICTACTICS"
    stage_dir.mkdir(parents=True, exist_ok=True)

    for name in BINARIES:
        src = BUILD_DIR / name
        if not src.exists():
            print(f"  [경고] {src} 를 찾을 수 없습니다 — 건너뜁니다.", file=sys.stderr)
            continue
        shutil.copy2(src, stage_dir / name)
        print(f"  복사: {name}")

    if README_EN.exists():
        shutil.copy2(README_EN, stage_dir / "README.md")
        print("  복사: README.md")
    else:
        print(f"  [경고] {README_EN} 를 찾을 수 없습니다.", file=sys.stderr)

    if README_KOR.exists():
        shutil.copy2(README_KOR, stage_dir / "README(KOR).md")
        print("  복사: README(KOR).md")
    else:
        print(f"  [경고] {README_KOR} 를 찾을 수 없습니다.", file=sys.stderr)

    if ASSETS_SRC.exists():
        shutil.copytree(ASSETS_SRC, stage_dir / "Assets", dirs_exist_ok=True)
        print("  복사: Assets/")
    else:
        print(f"  [오류] Assets 디렉토리를 찾을 수 없습니다: {ASSETS_SRC}", file=sys.stderr)
        sys.exit(1)

    return stage_dir


def make_zip(stage_dir: Path, version: str) -> Path:
    zip_name = f"DragonicTactics_{version}_playtest.zip"
    zip_path = PROJECT_ROOT / zip_name

    print(f"=== 3/3  ZIP 생성: {zip_path} ===")
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for file in sorted(stage_dir.rglob("*")):
            if file.is_file():
                arcname = "CODEPISTOLS_DRAGONICTACTICS/" + file.relative_to(stage_dir).as_posix()
                zf.write(file, arcname)

    size_mb = zip_path.stat().st_size / (1024 * 1024)
    print(f"  완료: {zip_path.name} ({size_mb:.1f} MB)")
    return zip_path


RELEASE_NOTES = """\
# 🐉 Dragonic Tactics - First Playtest

**Language / 언어 선택:**
- [🇰🇷 한국어 버전 (Korean Version)](#korean-version)
- [🇺🇸 English Version](#english-version)

---

## <div id="korean-version"></div>🇰🇷 한국어 버전 (Korean Version)

**Dragonic Tactics**의 첫 번째 공개 플레이테스트 버전입니다. 귀중한 시간을 내어 참여해 주셔서 감사합니다.

> **💡 권장 사항:**  **게임을 플레이하는 동안 리드미(README.md) 파일을 함께 참고**해 주시기를 강력히 권장합니다.

### 📥 다운로드 및 실행 방법
1. 아래 **Assets** 항목에서 **Codepistols_DragonicTactics.zip** 파일을 다운로드하세요.
2. 압축을 해제한 후 폴더 내의 `.exe` 파일을 실행하면 게임이 시작됩니다.
   > **주의:** 밑에 있는 `Source code` 파일들은 개발용이므로 다운로드하실 필요 없습니다!

### 📜 게임 규칙 및 승리 조건
- 플레이어와 AI가 번갈아 가며 턴을 진행하는 전략 게임입니다.
- **승리 조건:** 맵 위의 모든 적을 처치하세요!

### ⌨️ 주요 조작법
- **마우스 왼쪽 클릭:** 캐릭터/타일 선택, 이동, 스킬 사용
- **마우스 오른쪽 클릭:** 스킬 범위 표시 취소
- **Enter:** 메인 메뉴 진입 (타이틀이 초록색일 때)
- **ESC:** 게임 중 메인 메뉴로 돌아가기

### 🕹️ 시스템 가이드
- **상태 확인:** 캐릭터 위에 마우스 커서를 올리면 HP, AP, Speed, 스펠 슬롯을 확인할 수 있습니다.
- **자원 관리:** 이동에는 **Speed**가, 스킬 사용에는 **AP**와 **스펠 슬롯**이 소모됩니다. (Speed와 AP는 매 턴 회복되지만, 스펠 슬롯은 자동으로 회복되지 않으니 주의하세요!)

---

## <div id="english-version"></div>🇺🇸 English Version

Welcome to the first public playtest of **Dragonic Tactics**! We appreciate your participation in our development process.

> **💡 Recommendation:**  **we highly recommend reading the README.md file** before or while playing.

### 📥 How to Download & Run
1. Navigate to the **Assets** section below and download **Codepistols_DragonicTactics.zip**.
2. Extract the ZIP file and run the `.exe` file inside to start the game.
   > **Note:** Please ignore the `Source code` files as they are for development purposes only.

### 📜 Game Rules & Win Condition
- A turn-based strategy game where you and the AI take alternating turns.
- **Win Condition:** Defeat all enemies on the map to claim victory!

### ⌨️ Shortcuts & Controls
- **Left Mouse Click:** Select character/tile, move, use skills
- **Right Mouse Click:** Cancel skill range display
- **Enter:** Enter the game from the main menu (when the title turns green)
- **ESC:** Return to the main menu from the in-game screen

### 🕹️ System Guide
- **Check Status:** Hover your mouse over characters to check their HP, AP, Speed, and Spell Slots.
- **Resource Management:** Movement consumes **Speed**, while skills consume **AP** and **Spell Slots**.
- **Recovery:** Speed and AP are restored every turn, but Spell Slots do **not** recover automatically. Use them wisely!

---

## 📝 Share Your Feedback / 피드백 전달하기
여러분의 소중한 의견은 게임 발전에 큰 도움이 됩니다. 플레이 후 아래 설문지를 작성해 주세요!
Your insights are vital to making this game better. Please fill out our survey after playing:

👉 **[Take the Playtest Survey / 설문조사 참여하기](https://forms.gle/fSAFsKoonyGdXREn7)**

---
**Credits:**
- **Producer:** Taekyung Heo (Voice Act: Cleric)
- **Tech Lead:** Junyoung Ki
- **Test Lead:** Kinam Park
- **QA:** Sangyoon Lee
- **Designer:** Seungju Song (Voice Act: Dragon, Fighter)
"""


def upload(zip_path: Path, version: str):
    print(f"=== GitHub Release 업로드: {version} ===")
    subprocess.run(
        [
            "gh", "release", "create", version,
            str(zip_path),
            "--title", f"Dragonic Tactics {version} Playtest",
            "--notes", RELEASE_NOTES,
            "--repo", "taekyung00/CodePistols_DragonicTactics",
        ],
        check=True,
    )
    print(f"  GitHub Release '{version}' 업로드 완료.")


# ---------------------------------------------------------------------------
# 진입점
# ---------------------------------------------------------------------------

def interactive_mode():
    print("=" * 50)
    print("  Dragonic Tactics 플레이테스트 릴리즈 생성기")
    print("=" * 50)
    print(f"  프로젝트 루트: {PROJECT_ROOT}")
    print()

    version = input("버전을 입력하세요 (예: v0.3.0): ").strip()
    if not version:
        print("[오류] 버전을 입력해야 합니다.")
        input("\nEnter 키를 눌러 종료...")
        sys.exit(1)

    skip_build = input("CMake 빌드를 건너뛰시겠습니까? (이미 빌드된 경우) [y/N]: ").strip().lower() == "y"
    do_upload  = input("ZIP 생성 후 GitHub Release에 업로드하시겠습니까? [y/N]: ").strip().lower() == "y"

    print()
    return version, skip_build, do_upload


def main():
    if len(sys.argv) == 1:
        version, skip_build, do_upload = interactive_mode()
    else:
        parser = argparse.ArgumentParser(description="플레이테스트 릴리즈 ZIP 생성 및 GitHub Release 업로드")
        parser.add_argument("--version", required=True, help="릴리즈 버전 태그 (예: v0.3.0)")
        parser.add_argument("--upload", action="store_true", help="ZIP 생성 후 GitHub Release에 업로드")
        parser.add_argument("--skip-build", action="store_true", help="CMake 빌드 단계 건너뜀")
        args = parser.parse_args()
        version, skip_build, do_upload = args.version, args.skip_build, args.upload

    try:
        preflight_check(need_upload=do_upload)

        if skip_build:
            print("  [빌드 건너뜀] --skip-build 플래그 감지\n")
        else:
            build()

        with tempfile.TemporaryDirectory() as tmp:
            stage_dir = stage(Path(tmp))
            zip_path  = make_zip(stage_dir, version)

        print(f"\nZIP 생성 완료: {zip_path}")

        if do_upload:
            upload(zip_path, version)

    except subprocess.CalledProcessError as e:
        print(f"\n[오류] 명령 실패: {e}", file=sys.stderr)
        input("\nEnter 키를 눌러 종료...")
        sys.exit(1)
    except Exception as e:
        print(f"\n[오류] {e}", file=sys.stderr)
        input("\nEnter 키를 눌러 종료...")
        sys.exit(1)

    if len(sys.argv) == 1:
        input("\nEnter 키를 눌러 종료...")


if __name__ == "__main__":
    main()
