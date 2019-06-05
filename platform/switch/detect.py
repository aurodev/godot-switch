import os
import platform
import sys
from methods import get_compiler_version, using_gcc

def is_active():
    return True


def get_name():
    return "Switch"


def can_build():
    if "DEVKITPRO" in os.environ:
        return True
    return False


def get_opts():
    from SCons.Variables import BoolVariable, EnumVariable
    return [
        ("DEVKITPRO", "Path to the devkitpro toolchain", "")
    ]


def get_flags():

    return [
        ("tools", False),
        ("builtin_bullet", False),
        ("builtin_mbedtls", False),

        # TODO (fhidalgo): Check these. Can't compile
        # no <sys/un.h>
        ('module_upnp_enabled', False),
        # thirdparty/libvpx/vpx_util/vpx_thread.c:86:12: error: conflicting types for 'sync'
        ('module_webm_enabled', False),
        # thirdparty/libwebsockets/lib/plat/unix/private.h:44:10: fatal error: sys/mman.h: No such file or directory
        ('module_websocket_enabled', False),
        # no dload
        ('module_gdnative_enabled', False),
        ## x509_crt.c:(.text.mbedtls_x509_crt_parse_der+0x380): undefined reference to `mbedtls_pk_parse_subpubkey'
        ('module_mbedtls_enabled', False),
        
    ]

def configure(env):
    # TODO (fhidalgo): cleanup this mess

    ## DEVKITPRO Stuff
    devkitpro_path = os.environ.get("DEVKITPRO")
    compiler_tools_path = devkitpro_path + "/devkitA64/aarch64-none-elf/bin"
    tools_path = devkitpro_path + "/tools/bin"
    libnx_path = devkitpro_path + "/libnx"
    compiler_path = devkitpro_path + "/devkitA64/bin/"
    pkg_config_path = devkitpro_path + "/portlibs/switch/lib/pkgconfig"

    env['ENV']['DEVKITPRO'] = devkitpro_path  # used in linking stage
    os.environ["PKG_CONFIG_PATH"] = pkg_config_path
    env['ENV']['PKG_CONFIG_PATH'] = pkg_config_path

    env["RANLIB"] = compiler_tools_path + "/ranlib"
    env["AR"] = compiler_tools_path + "/ar"
    env['CC'] = compiler_path + "aarch64-none-elf-gcc"
    env['CXX'] = compiler_path + "aarch64-none-elf-g++"


    ## Build type
    if (env["target"] == "release"):
        if (env["optimize"] == "speed"): #optimize for speed (default)
            env.Prepend(CCFLAGS=['-O3'])
        else: #optimize for size
            env.Prepend(CCFLAGS=['-Os'])

        if (env["debug_symbols"] == "yes"):
            env.Prepend(CCFLAGS=['-g1'])
        if (env["debug_symbols"] == "full"):
            env.Prepend(CCFLAGS=['-g2'])
    elif (env["target"] == "release_debug"):
        if (env["optimize"] == "speed"): #optimize for speed (default)
            env.Prepend(CCFLAGS=['-O2', '-DDEBUG_ENABLED'])
        else: #optimize for size
            env.Prepend(CCFLAGS=['-Os', '-DDEBUG_ENABLED'])

        if (env["debug_symbols"] == "yes"):
            env.Prepend(CCFLAGS=['-g1'])
        if (env["debug_symbols"] == "full"):
            env.Prepend(CCFLAGS=['-g2'])
    elif (env["target"] == "debug"):
        env.Prepend(CCFLAGS=['-g3', '-DDEBUG_ENABLED', '-DDEBUG_MEMORY_ENABLED'])

    ## Architecture
    ARCH = ["-march=armv8-a", "-mtune=cortex-a57", "-mtp=soft", "-fPIE", "-mlittle-endian"]
    env["bits"] = "64"
    env["arch"] = "arm64"

    ## Compiler configuration

    env.Append(CCFLAGS=['-pipe'])
    env.Append(LINKFLAGS=['-pipe'])

    ## Dependencies

    # Bullet
    import subprocess
    bullet_version = subprocess.check_output(["pkg-config", "bullet", "--modversion"]).strip()
    if bullet_version < "2.88":
        # Abort as system bullet was requested but too old
        print("Bullet: System version {0} does not match minimal requirements ({1}). Aborting.".format(bullet_version, "2.88"))
        sys.exit(255)

    env.ParseConfig("pkg-config bullet --cflags --libs")

    env.Append(CPPPATH=[devkitpro_path + "/portlibs/switch/include/GLES3"])
    env.Append(LIBS=["EGL"])
    env.Append(LIBS=["GLESv2"])
    env.Append(LIBS=["drm_nouveau"])
    env.Append(LIBS=["glad"])
    env.Append(LIBS=["glapi"])
    env.Append(LIBS=['m'])

    # mbedtls
    # env.Append(CPPPATH=[devkitpro_path + "/portlibs/switch/include"])
    # env.Append(LIBPATH=[devkitpro_path + "/portlibs/switch/lib"])
    # env.Append(LIBS=['mbedtls'])
    # env.Append(LIBS=['mbedcrypto'])
    # env.Append(LIBS=['mbedx509'])

    # libnx
    env.Append(LIBPATH=[libnx_path + "/lib"])    
    env.Append(CPPPATH=[libnx_path + "/include"])
    env.Append(LIBS=["nx"])

    ## Flags
    # devkitpro
    env.Append(CPPFLAGS=["-ffunction-sections"])
    env.Append(CPPFLAGS=ARCH)

    env.Append(CPPPATH=['#platform/switch'])
    env.Append(CPPFLAGS=['-DSWITCH_ENABLED', '-DUNIX_ENABLED', "-D__SWITCH__", "-DNO_NETWORK", "-DNO_SYSLOG", "-DNO_DL", "-DOPENGL_ENABLED", "-DGLES_ENABLED", "-D__FreeBSD__", "-DGLAD_ENABLED", "-DGLES_OVER_GL"])

    ## Cross-compilation
    env.Append(LINKFLAGS=["-specs={}/switch.specs".format(libnx_path), "-g"])
    env.Append(LINKFLAGS=ARCH)
