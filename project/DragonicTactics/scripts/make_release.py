#!/usr/bin/env python3
"""
make_release.py — 플레이테스트 릴리즈 ZIP 생성 (선택적 GitHub Release 업로드)

사용:
  python scripts/make_release.py --version v0.3.0
  python scripts/make_release.py --version v0.3.0 --upload

실행 위치: DragonicTactics/ 디렉토리 (혹은 어느 위치든 가능)
"""

import argparse
import shutil
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

# __file__ 기준으로 DragonicTactics/ 절대경로 계산
SCRIPT_DIR   = Path(__file__).resolve().parent.parent   # DragonicTactics/
PROJECT_ROOT = SCRIPT_DIR.parent                         # 프로젝트 루트

BUILD_DIR    = SCRIPT_DIR / "build" / "windows-release"
ASSETS_SRC   = SCRIPT_DIR / "Assets"
README_EN    = SCRIPT_DIR / "README.md"
README_KOR   = SCRIPT_DIR / "README(KOR).md"

BINARIES = [
    "dragonic_tactics.exe",
    "OpenAL32.dll",
    "SDL2.dll",
    "soft_oal.dll",
]


def build():
    print("=== 1/3  릴리즈 빌드 ===")
    subprocess.run(
        ["cmake", "--preset", "windows-release"],
        cwd=SCRIPT_DIR,
        check=True,
    )
    subprocess.run(
        ["cmake", "--build", "--preset", "windows-release"],
        cwd=SCRIPT_DIR,
        check=True,
    )


def stage(tmp_root: Path) -> Path:
    """임시 디렉토리 안에 CODEPISTOLS_DRAGONICTACTICS/ 스테이징 폴더를 구성한다."""
    print("=== 2/3  스테이징 ===")
    stage_dir = tmp_root / "CODEPISTOLS_DRAGONICTACTICS"
    stage_dir.mkdir(parents=True, exist_ok=True)

    # 바이너리 복사
    for name in BINARIES:
        src = BUILD_DIR / name
        if not src.exists():
            print(f"  [경고] {src} 를 찾을 수 없습니다 — 건너뜁니다.", file=sys.stderr)
            continue
        shutil.copy2(src, stage_dir / name)
        print(f"  복사: {name}")

    # README.md (영문)
    if README_EN.exists():
        shutil.copy2(README_EN, stage_dir / "README.md")
        print("  복사: README.md")
    else:
        print(f"  [경고] {README_EN} 를 찾을 수 없습니다.", file=sys.stderr)

    # README(KOR).md (한글, 이름 그대로 복사)
    if README_KOR.exists():
        shutil.copy2(README_KOR, stage_dir / "README(KOR).md")
        print("  복사: README(KOR).md")
    else:
        print(f"  [경고] {README_KOR} 를 찾을 수 없습니다.", file=sys.stderr)

    # Assets/ 전체 복사
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


def upload(zip_path: Path, version: str):
    print(f"=== GitHub Release 업로드: {version} ===")
    subprocess.run(
        [
            "gh", "release", "create", version,
            str(zip_path),
            "--title", f"Dragonic Tactics {version} Playtest",
            "--notes", f"플레이테스트 릴리즈 {version}",
            "--repo", "taekyung00/CodePistols_DragonicTactics",
        ],
        check=True,
    )
    print(f"  GitHub Release '{version}' 업로드 완료.")


def main():
    parser = argparse.ArgumentParser(
        description="플레이테스트 릴리즈 ZIP 생성 및 GitHub Release 업로드"
    )
    parser.add_argument(
        "--version",
        required=True,
        help="릴리즈 버전 태그 (예: v0.3.0)",
    )
    parser.add_argument(
        "--upload",
        action="store_true",
        help="ZIP 생성 후 GitHub Release에 업로드",
    )
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="CMake 빌드 단계 건너뜀 (이미 빌드된 경우)",
    )
    args = parser.parse_args()

    if not args.skip_build:
        build()
    else:
        print("  [빌드 건너뜀] --skip-build 플래그 감지")

    with tempfile.TemporaryDirectory() as tmp:
        stage_dir = stage(Path(tmp))
        zip_path  = make_zip(stage_dir, args.version)

    print(f"\nZIP 생성 완료: {zip_path}")

    if args.upload:
        upload(zip_path, args.version)


if __name__ == "__main__":
    main()
