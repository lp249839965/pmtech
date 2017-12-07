import os
import os.path
import shutil

print("\n")
print("--------------------------------------------------------------------------------------------------------------")
print("pmtech audio compression and conversion ----------------------------------------------------------------------")
print("--------------------------------------------------------------------------------------------------------------")

platform_name = "win32"
if os.name == "posix":
    platform_name = "osx"

audio_dir = os.path.join(os.getcwd(), "assets", "audio")
build_dir = os.path.join(os.getcwd(), "bin", platform_name, "data", "audio")
bin_dir = os.path.join(os.getcwd(), "bin", platform_name)

# make directories
if not os.path.exists(build_dir):
    os.makedirs(build_dir)

dll = "fmod.dll"
if platform_name == "osx":
    dll = "libfmod.dylib"

# copy fmod dll / dylib
print("copying dynamic library to binary dir")
src_file = os.path.join(os.getcwd(), "..", "pen", "third_party", "fmod", "lib", platform_name, dll)
shutil.copy(src_file, bin_dir)

# copy audio files
print("copying audio to data dir")
for f in os.listdir(audio_dir):
    src_file = os.path.join(audio_dir,f)
    shutil.copy(src_file, build_dir)