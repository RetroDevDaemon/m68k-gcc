## New: Mega EverDrive Pro tool 
If you have USB, the `tools/everpi` tool should allow you to load and launch<br>
a compiled local .rom file directly to your Genesis/Megaddrive.<br>
`tools/everpi rom path/to/rom.bin`<br>
`tools/everpi memread|memset|memset16|memset32` can be run from the MEDP<br>
interrupt menu while the game is running.<br><br>
Be sure you are on a brand new install of Raspbian if you can, fully updated<br>
## If you want to build yourself:<br>
<br>
On linux, you can try to build using this repo by kentosama:<br>
https://github.com/kentosama/m68k-elf-gcc<br>
Or you might have better luck using this guide:<br>
https://darkdust.net/writings/megadrive/crosscompiler<br>
For MacOS try to build with Retro68:<br>
https://github.com/autc04/Retro68<br>
You may have to download and build the Bison extension from source<br>

It installs to `'/usr/local/bison'`

I had to change line 24 of Rez/CMakeLists.txt to this and clean/redo the build:<br>

`set(CMAKE_PROGRAM_PATH ${CMAKE_PROGRAM_PATH} "/usr/local/bison/bin")`

<br>

## If you a lazybones:<br>

<br>
`-nostdlib` is a required CFLAG or building will fail <br>

### 6.3.0 (GCC) binary for RPi-arm71:<br>

### http://barelyconsciousgames.com/m68k-toolchain-rpi.tar.gz <br>

<br>
In /tools is an RPi-arm71 build of DGen with debugger on (tilde key).<br>
You can also build it yourself using `install-dgen-dev.sh`.<br>

### 9.1.0 (GCC) binary for MacOS x86_64<br>

### http://barelyconsciousgames.com/m68k-gcc-910-mac-x86_64.zip <br>

<br>
By default, `make` will create `out.md` in the root folder.<br>