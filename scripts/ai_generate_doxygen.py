import os
from pathlib import Path
from openai import OpenAI

PROJECT_RULES = r"""
You are updating C source/header files according to this customized Doxygen standard.

Hard rules:
1. Do not modify existing logic, control flow, conditions, loops, assignments, initialization, or static qualifiers.
2. Do not remove original comments.
3. Do not remove commented-out includes.
4. Do not remove printf, fprintf, LOG_* or other debug output.
5. Suggestions may only be added as:
   // Suggestion: ...
6. All Doxygen documentation must be written in English.
7. Original non-Doxygen comments must remain exactly as they are.
8. Header files should use full public API style with @file, @brief, @defgroup if appropriate.
9. Source files should use @file, @brief, @ingroup if appropriate.
10. main/test .c files should use minimal main/test style, but function docs should still be correct.
11. Document structs, typedefs, arrays, memory ownership, params, return values, preconditions, postconditions, warnings, and notes where applicable.
12. Preserve formatting and code as much as possible.

Output:
Return only the full updated file contents. No explanations. No markdown fences.
"""

def detect_file_style(path: Path, text: str) -> str:
    suffix = path.suffix.lower()
    if suffix == ".h":
        return "header"
    if suffix == ".c":
        if "int main(" in text or "\nmain(" in text:
            return "main_test_c"
        return "source_c"
    return "unknown"

def build_prompt(file_path: str, content: str, style: str) -> str:
    return f"""
Target file: {file_path}
Detected style: {style}

Task:
Update this file so it complies with the customized Doxygen documentation standard.

Important:
- Preserve all logic exactly.
- Preserve all original comments exactly.
- Preserve all commented-out includes exactly.
- Preserve all printf/debug output exactly.
- Only add documentation/comments required by the standard.
- If documentation already exists, improve/update it rather than duplicating it.

File content below:
<<<<<<<< FILE START
{content}
>>>>>>>> FILE END
"""

def process_file(client: OpenAI, file_path: Path) -> bool:
    original = file_path.read_text(encoding="utf-8")
    style = detect_file_style(file_path, original)

    response = client.responses.create(
        model="gpt-5",
        instructions=PROJECT_RULES,
        input=build_prompt(str(file_path), original, style),
    )

    updated = response.output_text

    if not updated or updated.strip() == original.strip():
        return False

    file_path.write_text(updated, encoding="utf-8")
    return True

def main():
    api_key = os.environ.get("OPENAI_API_KEY")
    if not api_key:
        raise RuntimeError("OPENAI_API_KEY is not set")

    client = OpenAI(api_key=api_key)

    changed_list = Path("changed_files.txt")
    if not changed_list.exists():
        print("No changed_files.txt found")
        return

    changed_any = False

    for raw_line in changed_list.read_text(encoding="utf-8").splitlines():
        rel = raw_line.strip()
        if not rel:
            continue

        path = Path(rel)
        if not path.exists():
            print(f"Skipping missing file: {rel}")
            continue

        if path.suffix.lower() not in {".c", ".h"}:
            continue

        print(f"Processing {rel} ...")
        modified = process_file(client, path)
        if modified:
            print(f"Updated {rel}")
            changed_any = True
        else:
            print(f"No changes needed for {rel}")

    if not changed_any:
        print("No documentation updates were required.")

if __name__ == "__main__":
    main()