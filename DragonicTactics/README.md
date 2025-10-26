# CS200 Computer Graphics I

## How to Build and Run

First Setup your [Development Environment](docs/DevEnvironment.md)

### How to build and run

- open this folder(DragonicTactics) with visual studio code, and select configure preset with window (debug/developer release/ release). Now you can keep working with visual studio code , or you can move your ide with Visual Studio by open ./build/window*/*sln file.Then build with cmake(F7 shortkey may works) in VSCode or in VS(ctrl+shift+B) and run it!



### Project explanation

- You can check our base system works well by go Dragonic tactics in mainmenu. 
  
  Wheter all system is worked well is logged in console(VS) or debug console(VSCode).
  
  Keys to test:
  
  - F : eventbus check
  
  - E : dice manager
  
  - T : dragon and fighter status with fighter turn
  
  - Y : dragon and fighter status with dragon turn
  
  - D : dragon attack
  
  - H : fighter heal
  
  - S : grid
  
  - P : turn manager
  
  - J : test json
  
  - R: test json reload
  
  - L : test json log
  
  - Enter : test all combat system

- You can watch visualize demo in Test2 state. 
  - move character with arrow keys


- You can exit test states by press esc key

- You can also check how our engine works well by open demos provided in CS200 with Imgui panel, and also CS230 Final project in mainmenu. In final project, you should check particle(shining, ..), gamastate component(grid,..), gameobject component(ziggle,...), animation(passenger) working well.

### Command Line Build

**Windows**

```sh
# configure / generate build files
cmake --preset windows-debug
cmake --preset windows-developer-release
cmake --preset windows-release
cmake --preset web-debug-on-windows

# Build exe
cmake --build --preset windows-debug
cmake --build --preset windows-developer-release
cmake --build --preset windows-release
cmake --build --preset web-debug-on-windows
```
