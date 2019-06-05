## Godot engine port for the Nintendo Switch Homebrew

This is an unofficial port of the godot engine for the Nintendo Switch.

#### Status
* Only one joycon supported
* No sound
* No network
* No gdnative module

#### Requirements
Working devkitpro environment (https://devkitpro.org)

#### Compiling
Compilation is done in the same way that you would compile any of the devkitpro examples. Requirements are also the same. The following devkitpro packages are used:
switch-dev
switch-bulletphysics
switch-glm
switch-libdrm_nouveau
switch-libopus
switch-mbedtls
switch-opusfile
switch-zlib

Example command: `DEVKITPRO="/opt/devkitpro" scons platform=switch verbose=1 -j4 target=release`

#### Using the "exporter"
While there are no exporters created for the switch, the release provides a precompiled exporter for the switch. In order to make things easier I decided to make it a requirement to bundle the game pack into romfs with the godot executable. The main pack should be named main.pck and reside in <PACK_FILE_DIR>.

The following steps can be used to generate an nro of the game:
1. Export a pack of the game you want to run to <PACK_FILE_DIR>/main.pck (use an existing expoter using s3tc textures, for example OSX)
2. Create the nacp file: `/opt/devkitpro/tools/bin/nacptool --create application <GAME_NAME> <VERSION> <NACP_NAME>.nacp`
3. Create nro file: `/opt/devkitpro/tools/bin/elf2nro godot_switch.switch.opt.arm64 <NAME>.nro --icon=/opt/devkitpro/libnx/default_icon.jpg --nacp=<NACP_NAME>.nacp --romfsdir=<PACK_FILE_DIR>`
4. Copy file to switch and run, nxlink can be used to send file over the network: `/opt/devkitpro/tools/bin/nxlink <NAME>.nro -s` ('-s' will create a local server that will listen for logs if using a debug version)

#### Contributing
When I started this project I had little idea about C++ or Godot, so there are likely a lot of things that can be improved, any help is welcome.
