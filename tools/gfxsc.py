import os
import subprocess
import argparse
import struct
from enum import Enum
from typing import Dict
import tempfile
import uuid

class SlangTarget(Enum):
    SPIRV = "spirv"
    METAL = "metal"

def compile_shader(input_files: list[str], target: SlangTarget) -> bytes:
        command = ["slangc", "-target", target.value] + input_files

        try:
            result = subprocess.run(command, check=True, capture_output=True)
            return result.stdout
        except subprocess.CalledProcessError as e:
            if hasattr(e, 'stderr') and e.stderr:
                print(e.stderr.decode('utf-8', errors='replace'))
            exit(1)

def compile_metal_shader(input_code: bytes) -> bytes:
    tmp_dir = tempfile.gettempdir()
    unique_id = str(uuid.uuid4())

    msl_file = os.path.join(tmp_dir, f"shader_{unique_id}.metal")
    air_file = os.path.join(tmp_dir, f"shader_{unique_id}.air")
    lib_file = os.path.join(tmp_dir, f"shader_{unique_id}.metallib")

    with open(msl_file, "w") as f:
        f.write(input_code.decode('utf-8'))

    try:
        subprocess.run(["xcrun", "-sdk", "macosx", "metal", "-c", "-Wall", "-Werror", "-Wextra", "-o", air_file, msl_file], check=True)
        subprocess.run(["xcrun", "-sdk", "macosx", "metallib", "-o", lib_file, air_file], check=True)
    except subprocess.CalledProcessError as e:
        if hasattr(e, 'stderr') and e.stderr:
            print(e.stderr.decode('utf-8', errors='replace'))
        exit(1)

    with open(lib_file, "rb") as f:
        return f.read()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compile and pack Slang shaders.")
    parser.add_argument("-t", "--targets", nargs="+", required=True, help="Compilation targets.")
    parser.add_argument("-o", "--output", required=True, help="Output packed file.")
    parser.add_argument("input", nargs="+", help="Input Slang shader files.")
    args = parser.parse_args()

    compiled_shaders: Dict[SlangTarget, bytes] = {}
    for target in [SlangTarget(t) for t in args.targets]:
        compiled_shaders[target] = compile_shader(args.input, target)
        if target == SlangTarget.METAL:
            compiled_shaders[target] = compile_metal_shader(compiled_shaders[target])

    with open(args.output, "wb") as f:
        f.write(b"GFX_SHADER_PACKAGE")
        f.write(struct.pack("I", len(compiled_shaders)))
        for target, shader_data in compiled_shaders.items():
            f.write(struct.pack("I", len(target.value)))
            f.write(target.value.encode('utf-8'))
            f.write(struct.pack("I", len(shader_data)))
            f.write(shader_data)
