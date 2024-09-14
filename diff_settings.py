import argparse


def apply(config, args):
    config["arch"] = "mipsel"
    config["baseimg"] = f"in/989SND_D.IRX"
    config["myimg"] = f"build/989SND_D.IRX"
    config["mapfile"] = f"build/989SND_D.IRX.map"
    config["source_directories"] = [
        "src",
        "asm",
        "include",
        "assets",
    ]
    config["objdump_flags"] = ["-Mreg-names=n32"]
    config["expected_dir"] = f"expected/"
