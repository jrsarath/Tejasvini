#!/usr/bin/env python3
"""
Repository-wide validation script for Tejasvini.

Enforces architecture rules:
1. Exactly one canonical shared directory exists at `firmware/shared`.
2. Standard Arduino `library.properties` exists in `firmware/shared`.
3. Zero symlinks exist in the repository tree.
4. No duplicate protocol, types, error codes, or config header files exist.
5. No obsolete include patterns (e.g. `../shared/`, `shared/`, `#if __has_include`) in source files.
6. EEZ Studio export destination is configured to `firmware/Tejasvini_UI/src/ui`.
"""

import os
import sys
import json
import re

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

IGNORE_DIRS = {".git", "build", "build_sim", ".cache", ".pio", ".vscode"}
SHARED_HEADERS = {
    "Config.h",
    "Types.h",
    "ErrorCodes.h",
    "Protocol.h",
    "Serialization.h",
    "ProtocolVersion.h"
}

def check_single_shared_directory():
    errors = []
    shared_dirs = []
    for root, dirs, _ in os.walk(REPO_ROOT):
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
        if os.path.basename(root) == "shared":
            rel_path = os.path.relpath(root, REPO_ROOT)
            shared_dirs.append(rel_path)

    expected = "firmware/shared"
    if shared_dirs != [expected]:
        errors.append(
            f"Expected exactly one shared directory at '{expected}', found: {shared_dirs}"
        )
    return errors

def check_library_properties():
    errors = []
    props_path = os.path.join(REPO_ROOT, "firmware", "shared", "library.properties")
    if not os.path.isfile(props_path):
        errors.append(f"Missing Arduino library manifest: {props_path}")
    else:
        with open(props_path, "r", encoding="utf-8") as f:
            content = f.read()
        if "name=Tejasvini_Shared" not in content:
            errors.append("firmware/shared/library.properties missing 'name=Tejasvini_Shared'")
    return errors

def check_zero_symlinks():
    errors = []
    for root, dirs, files in os.walk(REPO_ROOT):
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
        for name in dirs + files:
            path = os.path.join(root, name)
            if os.path.islink(path):
                rel_path = os.path.relpath(path, REPO_ROOT)
                errors.append(f"Disallowed symlink found: {rel_path} -> {os.readlink(path)}")
    return errors

def check_no_duplicate_headers():
    errors = []
    header_locations = {h: [] for h in SHARED_HEADERS}
    for root, dirs, files in os.walk(REPO_ROOT):
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
        for f in files:
            if f in SHARED_HEADERS:
                rel_path = os.path.relpath(os.path.join(root, f), REPO_ROOT)
                header_locations[f].append(rel_path)

    for h, locs in header_locations.items():
        expected = [f"firmware/shared/{h}"]
        if locs != expected:
            errors.append(f"Header '{h}' must only exist in firmware/shared. Found: {locs}")
    return errors

def check_include_patterns():
    errors = []
    stale_pattern = re.compile(r'#include\s+["<](?:\.\./)+shared/|#include\s+["<]shared/')
    has_include_pattern = re.compile(r'__has_include\s*\(\s*["<](?:\.\./)*shared/')

    source_exts = {".h", ".hpp", ".c", ".cpp", ".ino"}
    for root, dirs, files in os.walk(REPO_ROOT):
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
        for f in files:
            ext = os.path.splitext(f)[1].lower()
            if ext in source_exts:
                filepath = os.path.join(root, f)
                rel_path = os.path.relpath(filepath, REPO_ROOT)
                # Skip validation script itself or documentation
                with open(filepath, "r", encoding="utf-8", errors="ignore") as src:
                    for line_num, line in enumerate(src, 1):
                        if stale_pattern.search(line):
                            errors.append(
                                f"Stale include path in {rel_path}:{line_num}: {line.strip()}"
                            )
                        if has_include_pattern.search(line):
                            errors.append(
                                f"Obsolete __has_include workaround in {rel_path}:{line_num}: {line.strip()}"
                            )
    return errors

def check_eez_destination():
    errors = []
    eez_path = os.path.join(REPO_ROOT, "ui.eez-project")
    if not os.path.isfile(eez_path):
        errors.append("Missing ui.eez-project file")
        return errors

    with open(eez_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    destination = data.get("settings", {}).get("build", {}).get("destinationFolder", "")
    expected = "firmware/Tejasvini_UI/src/ui"
    if destination != expected:
        errors.append(
            f"ui.eez-project destinationFolder is '{destination}', expected '{expected}'"
        )
    return errors

def main():
    all_errors = []
    print("Running Tejasvini Repository Architecture Validation...")

    all_errors.extend(check_single_shared_directory())
    all_errors.extend(check_library_properties())
    all_errors.extend(check_zero_symlinks())
    all_errors.extend(check_no_duplicate_headers())
    all_errors.extend(check_include_patterns())
    all_errors.extend(check_eez_destination())

    if all_errors:
        print(f"\nFAILED: {len(all_errors)} architecture violation(s) detected:")
        for err in all_errors:
            print(f"  - {err}")
        return 1

    print("\nSUCCESS: All architecture checks passed!")
    print("  ✓ Exactly 1 canonical shared directory (firmware/shared)")
    print("  ✓ Standard Arduino library.properties present")
    print("  ✓ Zero symlinks across the repository")
    print("  ✓ Zero duplicate headers")
    print("  ✓ Standardized direct include conventions")
    print("  ✓ EEZ Studio configured to firmware/Tejasvini_UI/src/ui")
    return 0

if __name__ == "__main__":
    sys.exit(main())
