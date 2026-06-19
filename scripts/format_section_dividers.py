#!/usr/bin/env python3
"""Normalize C++ section-divider comments to the left-aligned 85-column style.

A "section divider" is a full line of the form

    //===-------- SECTION NAME --------===//

(any number of dashes on each side, the name surrounded by at least one space).
It is rewritten with EXACTLY 5 leading dashes and the trailing dashes padded so
the whole line -- including any leading indentation -- is 85 columns wide:

    //===----- SECTION NAME ----------------------------------------------------===//

Lines that are pure rules -- text-less dash rules (//===------===//) or '=' rules
(//==========//) -- are left untouched. The transform is idempotent.
"""

import argparse
import pathlib
import re
import sys

EXTS = {".hpp", ".tpp", ".cpp", ".cc", ".h"}
WIDTH = 85
LEAD = 5  # fixed number of leading dashes after "//==="
MIN_TRAIL = 3  # never emit fewer trailing dashes than this

# Full-line named divider: indent, >=1 leading dash, spaced name, >=1 trailing dash.
DIVIDER = re.compile(r"^(?P<indent>[ \t]*)//===-+ +(?P<name>.+?) +-+===//[ \t]*$")


def rebuild(indent: str, name: str) -> str:
    fixed = len(indent) + len("//===") + LEAD + 1 + len(name) + 1 + len("===//")
    trail = max(WIDTH - fixed, MIN_TRAIL)
    return f"{indent}//==={'-' * LEAD} {name} {'-' * trail}===//"


def process(text: str):
    lines = text.split("\n")
    changes = []
    for i, line in enumerate(lines):
        m = DIVIDER.match(line)
        if not m:
            continue
        new = rebuild(m.group("indent"), m.group("name").strip())
        if new != line:
            changes.append((line, new))
            lines[i] = new
    return "\n".join(lines), changes


def iter_files(paths):
    for raw in paths:
        p = pathlib.Path(raw)
        if p.is_dir():
            for q in sorted(p.rglob("*")):
                s = str(q)
                if q.suffix in EXTS and "/build/" not in s and "/old/" not in s:
                    yield q
        elif p.suffix in EXTS:
            yield p


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("paths", nargs="+", help="files or directories to process")
    ap.add_argument("--check", action="store_true", help="report only; do not write")
    ap.add_argument(
        "--show", type=int, default=0, help="print the first N before/after pairs"
    )
    args = ap.parse_args()

    total_files = total_lines = 0
    shown = 0
    for f in iter_files(args.paths):
        text = f.read_text()
        new_text, changes = process(text)
        if not changes:
            continue
        total_files += 1
        total_lines += len(changes)
        if not args.check:
            f.write_text(new_text)
        verb = "would update" if args.check else "updated"
        print(f"{verb}: {f}  ({len(changes)} dividers)")
        while shown < args.show and shown < total_lines:
            for before, after in changes:
                if shown >= args.show:
                    break
                print(f"    -  {before}")
                print(f"    +  {after}")
                shown += 1

    verb = "Would update" if args.check else "Updated"
    print(f"\n{verb} {total_lines} dividers across {total_files} files.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
