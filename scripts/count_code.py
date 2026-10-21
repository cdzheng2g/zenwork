"""Count owned C/C++ sources; excludes dependencies, generated code and backups."""
import argparse
import json
import tarfile
from pathlib import Path

EXTENSIONS = {".h", ".hpp", ".cpp", ".cc", ".c"}


def summarize(files):
    groups = {name: {"files": 0, "physical_lines": 0, "nonblank_lines": 0} for name in ("src", "tests")}
    for name, content in files:
        path = Path(name)
        if path.parts[0] not in groups or path.suffix not in EXTENSIONS:
            continue
        # Some original files used CRCRLF. Treat it as one physical newline.
        lines = content.decode("utf-8-sig").replace("\r\r\n", "\n").replace("\r\n", "\n").splitlines()
        count = groups[path.parts[0]]
        count["files"] += 1
        count["physical_lines"] += len(lines)
        count["nonblank_lines"] += sum(bool(line.strip()) for line in lines)
    return groups


def count_directory(root):
    return summarize((p.relative_to(root).as_posix(), p.read_bytes()) for name in ("src", "tests")
                     for p in sorted((root / name).rglob("*")) if p.is_file() and p.suffix in EXTENSIONS)


def count_archive(path):
    with tarfile.open(path) as archive:
        return summarize((member.name, archive.extractfile(member).read()) for member in archive
                         if member.isfile() and Path(member.name).suffix in EXTENSIONS)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--baseline", type=Path, help="Original source .tar.gz backup")
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    result = {"scope": "src/tests C/C++; nonblank includes comments; excludes third_party, resources, build and backup files",
              "current": count_directory(args.root)}
    if args.baseline:
        result["original"] = count_archive(args.baseline)
    rendered = json.dumps(result, ensure_ascii=False, indent=2) + "\n"
    if args.output:
        args.output.write_text(rendered, encoding="utf-8")
    print(rendered, end="")
