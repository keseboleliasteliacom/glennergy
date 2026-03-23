import re
import subprocess
import sys
from pathlib import Path

def git_show(rev_spec: str, file_path: str) -> str:
    result = subprocess.run(
        ["git", "show", f"{rev_spec}:{file_path}"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        return ""
    return result.stdout

def count_debug_prints(text: str) -> int:
    patterns = [
        r'\bprintf\s*\(',
        r'\bfprintf\s*\(',
        r'\bLOG_[A-Z_]+\s*\(',
    ]
    return sum(len(re.findall(p, text)) for p in patterns)

def count_commented_includes(text: str) -> int:
    return len(re.findall(r'^\s*//\s*#include\b', text, flags=re.MULTILINE))

def has_file_tag(text: str) -> bool:
    return "@file" in text

def has_brief_tag(text: str) -> bool:
    return "@brief" in text

def has_main_function(text: str) -> bool:
    return "int main(" in text or "\nmain(" in text

def validate_file(path_str: str) -> list[str]:
    errors = []
    path = Path(path_str)

    if not path.exists():
        errors.append(f"{path_str}: file does not exist in workspace")
        return errors

    current = path.read_text(encoding="utf-8")
    previous = git_show("HEAD~1", path_str)

    if not has_file_tag(current):
        errors.append(f"{path_str}: missing @file")

    if not has_brief_tag(current):
        errors.append(f"{path_str}: missing @brief")

    if path.suffix == ".h":
        if "@defgroup" not in current:
            errors.append(f"{path_str}: header missing @defgroup")

    if path.suffix == ".c":
        if not has_main_function(current) and "@ingroup" not in current:
            errors.append(f"{path_str}: source missing @ingroup")

    if previous:
        old_prints = count_debug_prints(previous)
        new_prints = count_debug_prints(current)
        if new_prints < old_prints:
            errors.append(
                f"{path_str}: debug output count decreased ({old_prints} -> {new_prints})"
            )

        old_commented_includes = count_commented_includes(previous)
        new_commented_includes = count_commented_includes(current)
        if new_commented_includes < old_commented_includes:
            errors.append(
                f"{path_str}: commented includes count decreased "
                f"({old_commented_includes} -> {new_commented_includes})"
            )

    return errors

def main():
    changed_file_list = Path("changed_files.txt")
    if not changed_file_list.exists():
        print("changed_files.txt not found")
        sys.exit(1)

    files = [
        line.strip()
        for line in changed_file_list.read_text(encoding="utf-8").splitlines()
        if line.strip().endswith((".c", ".h"))
    ]

    all_errors = []
    for file_path in files:
        all_errors.extend(validate_file(file_path))

    if all_errors:
        print("Validation failed:")
        for err in all_errors:
            print(f" - {err}")
        sys.exit(1)

    print("Validation passed.")

if __name__ == "__main__":
    main()