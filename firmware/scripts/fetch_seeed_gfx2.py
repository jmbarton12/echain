# Fetches Seeed_GFX2 at a pinned commit into .pio/vendor/Seeed_GFX2.
#
# lib_deps can't install it on Windows: the repo's example paths exceed
# MAX_PATH, so `git clone` fails. We only need the library itself, so this
# downloads the commit tarball and extracts just src/, the manifests and LICENSE.
import io
import shutil
import tarfile
import urllib.request
from pathlib import Path

Import("env")  # noqa: F821 (provided by PlatformIO)

REPO = "Seeed-Studio/Seeed_GFX2"
COMMIT = "5737488810b9e4acfd01e9ad328a583fb3ea19fa"
KEEP_FILES = {"library.json", "library.properties", "LICENSE"}

vendor_dir = Path(env.subst("$PROJECT_DIR")) / ".pio" / "vendor"  # noqa: F821
lib_dir = vendor_dir / "Seeed_GFX2"
stamp = lib_dir / ".commit"


def wanted(rel):
    return rel in KEEP_FILES or rel.startswith("src/")


def fetch():
    url = f"https://codeload.github.com/{REPO}/tar.gz/{COMMIT}"
    print(f"Fetching Seeed_GFX2 @ {COMMIT[:7]}")
    with urllib.request.urlopen(url, timeout=120) as resp:
        data = resp.read()

    tmp_dir = vendor_dir / "Seeed_GFX2.tmp"
    shutil.rmtree(tmp_dir, ignore_errors=True)
    with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as tar:
        for member in tar.getmembers():
            # Strip the "Seeed_GFX2-<sha>/" prefix GitHub adds.
            parts = member.name.split("/", 1)
            if len(parts) != 2 or not member.isfile() or not wanted(parts[1]):
                continue
            dest = tmp_dir / parts[1]
            dest.parent.mkdir(parents=True, exist_ok=True)
            with tar.extractfile(member) as src, open(dest, "wb") as out:
                shutil.copyfileobj(src, out)

    (tmp_dir / ".commit").write_text(COMMIT)
    shutil.rmtree(lib_dir, ignore_errors=True)
    tmp_dir.rename(lib_dir)


if not stamp.exists() or stamp.read_text().strip() != COMMIT:
    fetch()
