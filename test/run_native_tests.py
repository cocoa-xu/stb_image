"""Run native regression tests against the checked-out C sources with sanitizers."""
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[1]
source = (repo / "c_src/stb_image_nif.c").read_text()

def function(text, name):
    match = re.search(r"^static [^\n]+\b" + name + r"\(", text, re.M)
    if not match:
        raise RuntimeError(f"Cannot find production function {name}")
    return text[match.start():text.index("\n}\n", match.start()) + 3]

with tempfile.TemporaryDirectory(prefix="stb-native-test-") as directory:
    temporary = Path(directory)
    macros = re.findall(r"^#define STBIR_(?:MALLOC|FREE)\([^\n]+", source, re.M)
    (temporary / "stb_allocator_macros.h").write_text("\n".join(macros) + "\n")
    if (repo / "test/native/gif_allocations.c").exists():
        helper = (repo / "c_src/nif_utils.h").read_text()
        (temporary / "gif_nif_function.h").write_text(
            function(helper, "image_byte_size") + "\n" + function(source, "read_gif_binary")
        )
    for test in sorted((repo / "test/native").glob("*.c")):
        binary = temporary / test.stem
        command = shlex.split(os.environ.get("CC", "cc")) + [
            "-std=c11", "-O1", "-g", "-fno-omit-frame-pointer",
            "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
            "-I" + str(temporary), "-I" + str(repo / "3rd_party/stb"),
            str(test), "-lm", "-o", str(binary)
        ]
        subprocess.run(command, check=True, timeout=60)
        subprocess.run([str(binary)], check=True, timeout=30)
        print(f"PASS {test.name} (ASan + UBSan)", flush=True)
