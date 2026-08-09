#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later

"""
Manage baselines for the Coin node visual snapshot test.

This script runs the existing unittest `TestCoinNodeSnapshots` via FuCadCmd,
configuring it through environment variables (so we don't depend on FuCad forwarding
CLI args to Python).

Examples:

  # Update baselines in-tree (recommended: do this on a controlled setup)
  tools/rendering/manage_coin_node_baselines.py update \
    --fucadcmd build/<preset>/bin/FuCadCmd

  # Compare current renders against baselines (writes actual/expected/diff under --out-dir)
  tools/rendering/manage_coin_node_baselines.py compare \
    --out-dir /tmp/FuCadTesting/CoinNodeSnapshots \
    --fucadcmd build/<preset>/bin/FuCadCmd
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

# pylint: disable=broad-exception-caught,duplicate-code


def _default_fucadcmd() -> str | None:
    # Allow explicit override from the environment.
    for env_var in ("FUCADCMD", "FC_FUCADCMD", "FREECAD_CMD"):
        val = os.environ.get(env_var, "").strip()
        if val:
            return val

    # Common local build layouts.
    candidates: list[Path] = [
        Path("build/bin/FuCadCmd"),
        *sorted(Path("build").glob("*/bin/FuCadCmd")),
        *sorted(Path("build").glob("*/bin/FuCadCmd.exe")),
    ]
    for candidate in candidates:
        if candidate.is_file():
            return str(candidate)

    return shutil.which("FuCadCmd")


def _default_baseline_dir() -> str:
    # This script lives in-tree under `tools/rendering/`.
    repo_root = Path(__file__).resolve().parents[2]
    return str(repo_root / "tests" / "visual" / "baselines" / "coin-nodes")


def _parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="mode", required=True)

    common = argparse.ArgumentParser(add_help=False)
    common.add_argument(
        "--fucadcmd",
        default=_default_fucadcmd(),
        help=(
            "Path to FuCadCmd (default: $FUCADCMD or auto-detect under build/*/bin/FuCadCmd or PATH)"
        ),
    )
    common.add_argument(
        "--qt-platform",
        default="",
        help=(
            "Value for QT_QPA_PLATFORM. By default this script does not override it; "
            "if no DISPLAY/WAYLAND is available, it auto-sets QT_QPA_PLATFORM=offscreen."
        ),
    )
    common.add_argument(
        "--baseline-dir",
        default=_default_baseline_dir(),
        help="Baseline directory containing/writing *.png files (default: %(default)s)",
    )
    common.add_argument(
        "--out-dir",
        default=os.path.join("/tmp", "FuCadTesting", "CoinNodeSnapshots"),
        help="Artifact output directory (default: %(default)s)",
    )
    common.add_argument("--nodes", default="", help="Comma-separated node type list (optional)")

    sub.add_parser("update", parents=[common], help="Write/update baselines")
    sub.add_parser("compare", parents=[common], help="Compare against baselines")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    """Entry point."""
    args = _parse_args(argv)

    if not args.fucadcmd:
        print(
            "ERROR: could not auto-detect FuCadCmd; pass --fucadcmd "
            "build/<preset>/bin/FuCadCmd (or set $FUCADCMD)",
            file=sys.stderr,
        )
        return 2

    fucadcmd = Path(args.fucadcmd)
    if not fucadcmd.is_file():
        print(f"ERROR: FuCadCmd not found: {fucadcmd}", file=sys.stderr)
        return 2

    baseline_dir = Path(args.baseline_dir)
    if args.mode == "update":
        baseline_dir.mkdir(parents=True, exist_ok=True)
    elif not baseline_dir.is_dir():
        print(
            f"ERROR: baseline directory not found: {baseline_dir} "
            "(pass --baseline-dir or run `update` first)",
            file=sys.stderr,
        )
        return 2

    env = os.environ.copy()
    if args.qt_platform.strip():
        env["QT_QPA_PLATFORM"] = args.qt_platform.strip()
    else:
        has_display = bool(env.get("DISPLAY") or env.get("WAYLAND_DISPLAY"))
        if not has_display and not env.get("QT_QPA_PLATFORM"):
            env["QT_QPA_PLATFORM"] = "offscreen"
    env["FC_VISUAL_BASELINE_DIR"] = str(baseline_dir.resolve())
    env["FC_VISUAL_OUT_DIR"] = str(Path(args.out_dir).resolve())
    if args.nodes.strip():
        env["FC_VISUAL_NODES"] = args.nodes

    if args.mode == "update":
        env["FC_VISUAL_UPDATE_BASELINE"] = "1"
        cmd = [str(fucadcmd), "-t", "TestCoinNodeSnapshots"]
    else:
        env.pop("FC_VISUAL_UPDATE_BASELINE", None)
        cmd = [str(fucadcmd), "-t", "TestCoinNodeSnapshots"]

    print(f"Running: {' '.join(cmd)}")
    if "QT_QPA_PLATFORM" in env:
        print(f"  QT_QPA_PLATFORM={env['QT_QPA_PLATFORM']}")
    print(f"  FC_VISUAL_BASELINE_DIR={env['FC_VISUAL_BASELINE_DIR']}")
    print(f"  FC_VISUAL_OUT_DIR={env['FC_VISUAL_OUT_DIR']}")
    if "FC_VISUAL_NODES" in env:
        print(f"  FC_VISUAL_NODES={env['FC_VISUAL_NODES']}")

    proc = subprocess.run(cmd, env=env, cwd=str(Path.cwd()), check=False)
    return proc.returncode


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
