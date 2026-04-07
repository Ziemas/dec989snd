#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright 2025 polyproxy
# SPDX-License-Identifier: MIT

# I just stole this from the parappa decomp

import argparse
import os
import shutil
import subprocess
import sys
import re
import json

from pathlib import Path
from typing import Dict, List, Set, Union, Any, Literal, cast

import ninja_syntax

import spimdisasm
import splat
import splat.scripts.split as split
from splat.segtypes.linker_entry import LinkerEntry

ROOT = Path(__file__).parent
TOOLS_DIR = ROOT / "tools"

COMPILER_DIR = f"{TOOLS_DIR}/gcc-2.95.2"
COMMON_INCLUDES = "-Iinclude -Iinclude/common -Iinclude/sdk"

COMPILER_LAUNCHER = "wibo"
MASPSXFLAGS = "--aspsx-version=2.78"
MASPSX = f"python {TOOLS_DIR}/maspsx/maspsx.py {MASPSXFLAGS}"
ASFLAGS = "-Iinclude -G0 -g0 -O0"
COMPILE_CMD = f"{COMPILER_LAUNCHER} {COMPILER_DIR}/cc1.exe {COMMON_INCLUDES}"
CPPFLAGS = "-ffreestanding -Iinclude "

CROSS = "mipsel-none-elf-"

build_targets = [
    {
        "config": "config/989snd_d.yaml",
        "basename": "989SND_D.IRX",
        "objdir": "debug",
        "symdir": "config/debug",
        "cflags": "-O0 -G0 -g3",
        "defs": "-DDEBUG",
        "override": {},
    },
    {
        "config": "config/989snd.yaml",
        "basename": "989SND.IRX",
        "objdir": "release",
        "symdir": "config/release",
        "cflags": "-O2 -G0 -g1",
        "defs": "-DRELEASE",
        "override": 
            {"src/LFO.c": "-O3 -G0 -g0"},
    },
]

def exec_shell(command: List[str], stdout=subprocess.PIPE) -> str:
    ret = subprocess.run(command, stdout=stdout, stderr=subprocess.PIPE, text=True)
    return ret.stdout

def clean():
    if os.path.exists(".splache"):
        os.remove(".splache")
    if os.path.exists(".ninja_log"):
        os.remove(".ninja_log")
    if os.path.exists("build.ninja"):
        os.remove("build.ninja")

    for tgt in build_targets:
        if os.path.exists(f"{tgt["basename"]}.ld") :
                os.remove(f"{tgt["basename"]}.ld")

    shutil.rmtree("asm", ignore_errors=True)
    shutil.rmtree("assets", ignore_errors=True)
    shutil.rmtree("build", ignore_errors=True)

def write_rules(ninja):
    ninja.rule(
            "as",
            description="as $in",
            command=f"cpp {COMMON_INCLUDES} $in -o - | {CROSS}as -no-pad-sections -EL -march=3000 -Iinclude -o $out",
    )

    ninja.rule(
            "cc",
            description="cc $in",
            command = f"cpp {CPPFLAGS} $defs $in | {COMPILE_CMD} $cflags -quiet | {MASPSX} | {CROSS}as -no-pad-sections -EL -march=3000 -Iinclude -o $out"
    )

    ninja.rule(
            "ld",
            description="ld $out",
            command=f"{CROSS}ld -EL -Map $mapfile -T $in -o $out $syms",
    )

    ninja.rule(
            "sha1sum",
            description="sha1sum $in",
            command="sha1sum -c $in && touch $out",
    )

    ninja.rule(
            "rom",
            description="rom $out",
            command=f"{CROSS}objcopy $in $out -O binary && truncate $out -s 249588",
    )


def build_stuff(tgt: dict[str, Any], ninja, linker_entries: List[LinkerEntry]):
    built_objects: Set[Path] = set()

    def build(
        object_paths: Union[Path, List[Path]],
        src_paths: List[Path],
        task: str,
        variables: Dict[str, str] = {},
        implicit_outputs: List[str] = [],
    ):
        if not isinstance(object_paths, list):
            object_paths = [object_paths]

        object_strs = [str(obj) for obj in object_paths]

        for object_path in object_paths:
            if object_path.suffix == ".o":
                built_objects.add(object_path)
            ninja.build(
                outputs=object_strs,
                rule=task,
                inputs=[str(s) for s in src_paths],
                variables=variables,
                implicit_outputs=implicit_outputs,
            )

    object_paths = set()
    for entry in linker_entries:
        seg = entry.segment

        if seg.type[0] == ".":
            continue

        if entry.object_path is None:
            continue

        if entry.object_path in object_paths:
            print(f"Removing duplicate entry for: {entry.object_path}")
            continue
        object_paths.add(entry.object_path)

        if isinstance(seg, splat.segtypes.common.asm.CommonSegAsm) or isinstance(
            seg, splat.segtypes.common.data.CommonSegData
        ):
            build(entry.object_path, entry.src_paths, "as")

        elif isinstance(seg, splat.segtypes.common.c.CommonSegC):
            src = entry.src_paths[0]
            paths = entry.src_paths

            cflags = tgt["cflags"]
            if str(src) in tgt["override"]:
                cflags = tgt["override"][str(src)]

            build(entry.object_path, paths, "cc", variables = {"cflags": cflags, "defs": tgt["defs"]})

        elif isinstance(seg, splat.segtypes.common.databin.CommonSegDatabin) or isinstance(seg, splat.segtypes.common.rodatabin.CommonSegRodatabin):
            build(entry.object_path, entry.src_paths, "as")

        else:
            print(f"ERROR: Unsupported build segment type {seg.type}")
            sys.exit(1)

    elf_path = f"build/{tgt["basename"]}.elf"
    ld_path = f"{tgt["basename"]}.ld"
    map_path = f"build/{tgt["basename"]}.map"
    target_ld_args = f"-T config/{tgt["objdir"]}/undefined_syms_auto.txt -T config/{tgt["objdir"]}/undefined_funcs_auto.txt -T config/{tgt["objdir"]}/undefined_syms.txt"

    ninja.build(
        elf_path,
        "ld",
        ld_path,
        implicit=[str(obj) for obj in built_objects],
        variables={"mapfile": map_path, "syms": target_ld_args},
    )

    rom_path = f"build/{tgt["basename"]}.rom"

    ninja.build(
        rom_path,
        "rom",
        elf_path,
    )

    checksum_path = f"config/{tgt["objdir"]}/checksum.sha1"

    ninja.build(
        rom_path + ".ok",
        "sha1sum",
        checksum_path,
        implicit=[rom_path],
    )

def build_objdiff_units(tgt: dict[str, Any], config: dict[str, Any]) -> list[dict[str, Any]]:
    """
    Generate `objdiff.json` configuration from splat YAML config.

    Parse splat YAML config to get a list of the TUs that need to
    be diffed and create appropriate `units` for objdiff to process.

    Target objects need to be extracted separately (see the
    `make <lang>-make-asm` command) in order for objdiff to find the
    target files.
    """
    segments: list[Any] = config["segments"]

    tu_to_diff: list[tuple[Literal["asm", "c"], str]] = []

    for segment in segments:
        if not (isinstance(segment, dict) and segment["name"] == "main"):
            # we are looking for the main segment
            continue

        subsegments = cast(list[Any], segment["subsegments"])

        for subsegment in subsegments:
            if isinstance(subsegment, list):
                if len(subsegment) != 3:
                    # entry is not complete => skip it
                    continue

                _, subs_type, subs_name = cast(tuple[int, str, str], subsegment)

            elif isinstance(subsegment, dict):
                subs_type = cast(int, subsegment["type"])
                subs_name = cast(str, subsegment["name"])

            else:
                raise RuntimeError("invalid subsegment type")

            if subs_type in ("asm", "c", "cpp"):
                tu_to_diff.append((subs_type, subs_name))

    units: list[dict[str, Any]] = []

    decomp_tu_count: int = 0

    for tu_type, tu_name in tu_to_diff:
        tu_obj_suffix = f".{tu_type}.o" # .c.o or .cpp.o

        target_path = Path("obj", tgt["objdir"], tu_name).with_suffix(tu_obj_suffix)

        # since we only compile fully decompiled TUs, the
        # "c" type implies that the TU is complete
        is_decompiled = tu_type in ("c", "cpp")

        category = Path(tu_name).parts[0]

        if is_decompiled:
            # compose the build path as "build/src/path/of/tu.{c,cpp}.o"
            base_path = Path("build", tgt["objdir"], "src", tu_name).with_suffix(tu_obj_suffix)
            decomp_tu_count += 1
        else:
            # use dummy object for incomplete (not yet decompiled) TUs:
            # expected/obj/dummy.c.o
            base_path = Path("obj", "dummy").with_suffix(".c.o")

        units.append(
            {
                "name": f"{tgt["objdir"]}/{tu_name}",
                "target_path": str(target_path),
                "base_path": str(base_path),
                "scratch": {
                    "platform": "ps2",
                    "compiler": "iop-gcc2.8.1",
                    "c_flags": "-O3 -G0",
                },
                "metadata": {"progress_categories": [category]},
            }
        )

    return units

def write_objdiff_configuration(units: list[dict[str, Any]]):
    objdiff_json: dict[str, Any] = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "custom_make": "ninja",
        "custom_args": [],
        "build_target": False,
        "build_base": True,
        "watch_patterns": [
             "src/**/*.h",
             "src/**/*.c",
             "src/**/*.cpp"
        ],
        "units": units,
    }

    objdiff_path = Path("objdiff.json")

    with objdiff_path.open(mode="w") as fw:
        json.dump(objdiff_json, fw, indent=2)

    print(
        f"Wrote objdiff configuration ({len(units)} units) to {objdiff_path}"
    )

def build_objdiff_objects():
    objdiff_path = Path("objdiff.json")

    dummy_path = Path("obj", "dummy").with_suffix(".c.o")

    objdiff_conf = json.loads(objdiff_path.read_text())

    units = objdiff_conf["units"]

    build_jobs: list[tuple[Path, Path]] = []

    for unit in units:
        # name: str = unit["name"]
        target_path: Path = Path(unit["target_path"])
        base_path: Path = Path(unit["base_path"])

        if base_path == dummy_path:
            continue

        asm_path = Path("asm", *target_path.parts[1:]).with_suffix("").with_suffix(".s")
        print(target_path)
        print(asm_path)

        assert asm_path.exists()

        build_jobs.append((asm_path, target_path))

    # compile objects
    dummy_c_path = Path("obj", "dummy.c")
    dummy_o_path = Path("obj", "dummy.c.o")

    # create the empty source (touch)
    dummy_c_path.parent.mkdir(parents=True, exist_ok=True)
    dummy_c_path.open(mode="a").close()
    command = f"{COMPILE_CMD} {dummy_c_path} -o {dummy_o_path} && {CROSS}strip {dummy_o_path} -N dummy-symbol-name"
    subprocess.run(command, shell=True)
    dummy_c_path.unlink()

    for asm_path, target_path in build_jobs:
        command = (
            f"cpp {COMMON_INCLUDES} {asm_path} -o - | "
            f"{CROSS}as -no-pad-sections -EL -march=3000 -Iinclude -o {target_path}"
        )

        target_path.parent.mkdir(parents=True, exist_ok=True)

        subprocess.run(command, shell=True)

# clangd is stupid and cries about everything
def fix_compile_commands():
    with open("compile_commands.json", "r") as f:
        data = json.load(f)
    
    for entry in data[:]:
        file_path = Path(entry["file"])
        #
        # Remove stuff that clangd complains about:
        #  - '-Gx' flag
        #  - 'strip' command
        #  - '-gstabs' flag (now replaced with regular '-g')
        #
        if file_path.suffix == ".c" or file_path.suffix == ".cpp":
            entry["command"] = f"cc {CPPFLAGS}"
            entry["command"] = entry["command"].replace(" -quiet", "")
            entry["command"] = entry["command"].replace(" -G0", "")


            entry["command"] += " -m32"

        if file_path.suffix == ".cpp":
            entry["command"] += " -std=c++98"

        if file_path.suffix == ".s":
            data.remove(entry)

    with open("compile_commands.json", "w") as f:
        json.dump(data, f, indent=2)

def prepare_rom_from_elfs(elfs: list[Path]):
    for elf in elfs:
        assert elf.exists(), f"{elf} must exist!"
        command = f"{CROSS}objcopy -O binary --gap-fill=0x00 {elf} {elf}.rom"
        subprocess.run(command, shell=True, check=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Configure the project")
    parser.add_argument(
        "-noc",
        "--noclean",
        help="Skip cleaning of split and build artifacts",
        action="store_true",
    )
    parser.add_argument(
        "-csrc",
        "--cleansrc",
        help="Clean the 'src' folder",
        action="store_true",
    )
    parser.add_argument(
        "-objdiff",
        "--objdiff",
        help="Output objdiff JSONs (requires existing build)",
        action="store_true"
    )
    args = parser.parse_args()

    if not args.noclean:
        if args.objdiff:
            # No clean with objdiff
            args.noclean = True
        else:
            clean()

    if args.cleansrc:
        shutil.rmtree("src", ignore_errors=True)

    ninja = ninja_syntax.Writer(open(str(ROOT / "build.ninja"), "w"), width=9999)
    write_rules(ninja)

    units: list[dict[str, Any]] = []

    for tgt in build_targets:
        prepare_rom_from_elfs([Path(f"in/{tgt["basename"]}")])

        yaml_file = Path(f"{tgt["config"]}")
        split.main([Path(yaml_file)], modes=["all"], verbose=False)
        linker_entries = split.linker_writer.entries

        build_stuff(tgt, ninja, linker_entries)

        units.extend(build_objdiff_units(tgt, split.config))

        # Hack to avoid splat & spimdisasm from
        # leaking symbols from the previous split.
        splat.util.symbols.spim_context = spimdisasm.common.Context()
        splat.util.symbols.reset_symbols()

    ninja.close()
    exec_shell(["ninja", "-t", "compdb"], open("compile_commands.json", "w"))
    fix_compile_commands()

    if args.objdiff:
        write_objdiff_configuration(units)
        build_objdiff_objects()
