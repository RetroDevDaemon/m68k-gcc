## New: Mega EverDrive Pro tool 
If you have USB, the `tools/everpi` tool should allow you to load and launch<br>
a compiled local .rom file directly to your Genesis/Megaddrive.<br>
`tools/everpi rom path/to/rom.bin`<br>
`tools/everpi memread|memset|memset16|memset32` can be run from the MEDP<br>
interrupt menu while the game is running.<br><br>
Be sure you are on a brand new install of Raspbian if you can, fully updated.<br>
The tool has been tested on Ubuntu 21 64-bit as well.<br>
Due to changes in the way macOS handles serial connections, it will NOT work on newer macs.<br>
If you are having issues accessing the port, ensure that your user has `dialout` group priveleges:<br>
`sudo usermod -a -G dialout $USER`

## If you want to build the GCC compiler yourself:<br>
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
Using a standard GCC build from source, you should be able to compile targeting m68k using the following:<br>
```
./configure  --disable-mcs51-port   \
  --disable-r2k-port     \
  --disable-r2ka-port    \
  --disable-r3ka-port    \
  --disable-tlcs90-port  \
  --disable-ds390-port   \
  --disable-ds400-port   \
  --disable-pic14-port   \
  --disable-pic16-port   \
  --disable-hc08-port    \
  --disable-s08-port     \
  --disable-pdk13-port   \
  --disable-pdk14-port   \
  --disable-pdk15-port   \
  --enable-pdk16-port
```
Good luck and feel free to open issues with questions.<br>

## If you a lazybones:<br>
These are slightly out-of-date. I currently use GCC 9.3 built on both Raspberry Pi and Apple Silicon.<br>
<br>
`-nostdlib` for these builds is a required CFLAG or building will fail <br>

### 6.3.0 (GCC) binary for RPi-arm71:<br>

### http://barelyconsciousgames.com/m68k-toolchain-rpi.tar.gz <br>

<br>
In /tools is an RPi-arm71 build of DGen with debugger on (tilde key).<br>
You can also build it yourself using `install-dgen-dev.sh`.<br>

### 9.1.0 (GCC) binary for MacOS x86_64<br>

### http://barelyconsciousgames.com/m68k-gcc-910-mac-x86_64.zip <br>

<br>
By default, `make` will create `out.md` in the root folder.<br>
