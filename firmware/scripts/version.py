# Embeds `git describe` as ECHAIN_VERSION (FW-5).
import subprocess

Import("env")  # noqa: F821 (provided by PlatformIO)


def git_describe():
    try:
        return subprocess.check_output(
            ["git", "describe", "--tags", "--always", "--dirty"],
            cwd=env.subst("$PROJECT_DIR"),  # noqa: F821
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "unknown"


env.Append(CPPDEFINES=[("ECHAIN_VERSION", env.StringifyMacro(git_describe()))])  # noqa: F821
