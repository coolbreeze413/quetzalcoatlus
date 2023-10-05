# quetzalcoatlus

![Quetzalcoatlus](resources/images/quetzalcoatlus_flying_outline.svg "QuetzalCoatlus") 


A Qt-based 'proving-grounds' application to exercise interesting or re-usable parts of Qt, C++17, C++ build strategies (CMake/Makefile), deployment strategies on multiple platforms, including the Docker approach for cross-compilation.

This application is an attempt to preserve the random bits and pieces of advice collected over multiple projects into a (hopefully) cohesive project.

```
ADD MORE!
```


## Usage


### Platforms

Tested on Ubuntu 20.04 with:
- default `apt` provided `Qt 5.12.8`
- Qt Installer provided `Qt 5.15.2` ( base install path used: `/home/${USER}/qt` ==>> so, `5.15.2` path: `/home/${USER}/qt/5.15.2` )

In general, it is recommended to use a base path like: `/home/${USER}/qt` or `/opt/qt` for installation, so it becomes easy to use the same build commands across different machines.


### Get Sources

Clone the repo as usual:
```bash
git clone https://github.com/coolbreeze413/quetzalcoatlus.git
cd quetzalcoatlus
```


### Build Local Install

For distro provided installation of Qt, we can use:

```bash
make
```


If we want to use a separate non-distro-provided installation of Qt, using a Qt Installer, then:

1. Ensure that we remove the CMake `build` directory once (if built earlier) to clear the cache and any artifacts first:
   
   ```bash
   make distclean
   ```


2. Call `make` with the `qmake` path of the desired Qt Installation to use:

   ```bash
   make QMAKE_PATH=/path/to/bin/qmake
   ```

   For example, if we have `Qt 5.15.2` installation at: `/home/${USER}/qt/5.15.2`  
   Then, qmake path for gcc, x86_64 will be at: `/home/${USER}/qt/5.15.2/gcc_64/bin/qmake`  
   and then we use:  
   ```bash
   make QMAKE_PATH=/home/${USER}/qt/5.15.2/gcc_64/bin/qmake
   ```


### Run Local Install

For distro provided installation of Qt, we can use:

```bash
./install/bin/quetzalcoatlus
```


If we want to use a separate non-distro-provided installation of Qt, using a Qt Installer, then to run the built application, set the `LD_LIBRARY_PATH` variable to the corresponding Qt install `lib/` path:

```bash
LD_LIBRARY_PATH=/path/to/qt/install/gcc_64/lib:$LD_LIBRARY_PATH ./install/bin/quetzalcoatlus
```

For example, if we have `Qt 5.15.2` installation at: `/home/${USER}/qt/5.15.2`  
Then, lib path for gcc, x86_64 will be at: `/home/${USER}/qt/5.15.2/gcc_64/lib`  
and then we use:  
```bash
LD_LIBRARY_PATH=/home/${USER}/qt/5.15.2/gcc_64/lib:$LD_LIBRARY_PATH ./install/bin/quetzalcoatlus
```


### Build `deploy` Package

Currently, we use [linuxdeployqt](https://github.com/probonopd/linuxdeployqt) for creating a deploy package and an AppImage.

Note that, linuxdeployqt will add a `DT_RUNPATH`( == `$ORIGIN/../lib` )to the ELF so that the libraries can be found without explicitly setting `LD_LIBRARY_PATH` while running the binary from the `deploy` package.

This can be seen by using `readelf -d ./quetzalcoatlus` in the deploy directory.
![quetzalcoatlus_readelf](./images/quetzalcoatlus_deploy_readelf.png)


For distro provided installation of Qt, we can use:

```bash
make deploy
```

If we want to use a separate non-distro-provided installation of Qt, using a Qt Installer, then call `make` with the `qmake` path of the desired Qt Installation to use:

```bash
make deploy QMAKE_PATH=/path/to/bin/qmake
```

For example, if we have `Qt 5.15.2` installation at: `/home/${USER}/qt/5.15.2`  
Then, qmake path for gcc, x86_64 will be at: `/home/${USER}/qt/5.15.2/gcc_64/bin/qmake`  
and then we use:  
```bash
make deploy QMAKE_PATH=/home/${USER}/qt/5.15.2/gcc_64/bin/qmake
```

Internally, we set the `$PATH` variable to ensure that `linuxdeployqt` can find the correct `qmake` to use for packaging, according to this: https://github.com/probonopd/linuxdeployqt#qmake-configuration



### Run `deploy` Package

#### `deploy` Package Directory

The `deploy` package as a directory is available at: `deploy/quetzalcoatlus_<current_version>`

To run from the deploy package directory, we don't need any special steps, for both distro provided Qt install or manually installed Qt installs:

```bash
cd deploy/quetzalcoatlus_<current_version>/bin
./quetzalcoatlus
```

#### `deploy` Package Tarball

The deploy package is also available as a tarball that can be extracted and used as the directory usage above: `deploy/quetzalcoatlus_<current_version>.tar.gz`

#### `deploy` Package AppImage

There is also an AppImage created: `deploy/QuetzalCoatlus-${SHA1}-x86_64` which can just be run directly.  



## Attributions

The awesome quetzalcoatlus SVG used in the application is derived from:  
https://thenounproject.com/icon/quetzalcoatlus-4818424/  
CC BY 3.0  
https://creativecommons.org/licenses/by/3.0/us/legalcode  


The Black Hole Accretion Disk icon in the buttons is by Hrvoje Tomic from the Noun Project:  
https://thenounproject.com/icon/black-hole-5551295/  
CC BY 3.0  
https://creativecommons.org/licenses/by/3.0/us/legalcode  


The Accretion Disk gif in the application is from NASA:  
https://svs.gsfc.nasa.gov/13326  
Credit: NASA’s Goddard Space Flight Center/Jeremy Schnittman  


The planet SVGs are derived from:  
https://www.freepik.com/free-vector/flat-universe-infographic-template_6359775.htm  
FreePik License
