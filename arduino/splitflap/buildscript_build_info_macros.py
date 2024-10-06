import platform
import subprocess
import time

Import("env")

def get_firmware_specifier_build_flag():
    ret = subprocess.run(["git", "describe", "--always", "--abbrev=40", "--dirty", "--match=nevermatchanything"], stdout=subprocess.PIPE, text=True)
    build_version = ret.stdout.strip()
    build_flag = "-D BUILD_GIT_HASH=\\\"" + build_version + "\\\""
    print ("Firmware Revision: " + build_version)
    return (build_flag)

def get_build_date_flag():
    today = time.strftime("%Y-%m-%d")
    return "-D BUILD_DATE=\\\"" + today + "\\\""

def get_build_os_flag():
    build_os = platform.system()
    return "-D BUILD_OS=\\\"" + build_os + "\\\""

env.Append(
    BUILD_FLAGS=[
        get_firmware_specifier_build_flag(),
        get_build_os_flag(),
        get_build_date_flag(),
    ]
)
