# BUILD CUSTOM MODULES

Developers can add extention modules such as data processors, filtering, features modelling, strategies and more.

### how to compile modules

you might need to update APP_DIR in samples/sources/CMakeLists.txt if the application is installed in custom path.
`set(APP_DIR "/usr/local/Stelgic Fintech/Stelgic")` for linux or `set(APP_DIR "C:/Program Files/Stelgic Fintech/Stelgic")` for windows

##### WINDOWS: 
- Visual Studio 2019 required
- git 2.39


```
cd sources
mkdir build && build
cmake ..
```

- navigate to build folder and open qcraftor.sln
- Under Solution Exploer, expand CMakePredifinedTargets, right click on INSTALL and build

##### LINUX: 
- GCC10
- git 2.39
