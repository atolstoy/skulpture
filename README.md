# Welcome to the humble Skulpture-five (and six!) repository...
![Skulpture-6 is almost here!](skulpture-qt6.png)

## What is it all about
This is a re-worked and partially updated code of the original Skulpture style 
that was once developed by Christoph Feck. Specifically, this repo is a continuation of the
[`skulpture-five`](https://github.com/cfeck/skulpture/tree/five) (a Qt5/KF5 port) branch from 2016.

## What is available
As of now, the following features are present:

- Qt5 support
- Qt6 support
- Configuration GUI for Qt5 and Plasma 5

## Dropped features
Compared to the original Skulpture style for KDE4, this version lacks the following features:

- no Kwin decorator part

## Compilation and first steps

### Dependencies

- Qt >= 5.15.2 for Qt5 style;
- Qt >= 6.2.0 for Qt6 style;
- Cmake >= 3.17.

### Building Qt5 style
```bash
mkdir build-qt5
cd build-qt5
cmake ..
make
sudo make install
```
### Building Qt6 style
```bash
mkdir build-qt6
cd build-qt6
cmake -DUSE_QT6=ON -DUSE_GUI_CONFIG=OFF ..
make
```
Then manually copy `skulpture.so` to the Qt6 styles directory (eg. `/usr/lib64/qt6/plugins/styles`).

### Using the style
The style is available for use once there is the `skulpture.so` file in the Qt Plugins directory in the `styles` subdirectory.
Eg., it is `/usr/lib64/qt5/plugins/styles/` for Qt5 version and `/usr/lib64/qt6/plugins/styles/skulpture.so` for Qt6
in my system, though your setup may have different paths.

If you're running Plasma 5, you'll be then able to find Skulpture under pedestrian Style settings in KDE's `systemsettings5`. For Qt6 apps you'll probably need
to use [Qt6ct](https://github.com/trialuser02/qt6ct). Don't forget to cast `export QT_QPA_PLATFORMTHEME=qt6ct` to make it work.

## Motivation
The purpose of this repo is to provide a working Qt6 port of Skulpture. Even though it's not a pixel-perfect
copy of its namesake for KDE4, it already provides great consistency between Qt5 and Qt6 apps. As the time goes by,
more and more apps are getting ported to Qt6 (qBittorrent, Strawberry etc), so that we need one style to fit 'em all.
Think of Skulpture as an alternative to Kvantum, which seems to be the only third-party style supporting both Qt5 and Qt6.
Your contribution and improvement in the Skulpture style will be strongly welcomed and appreciated!

## Credits

- [Christoph Feck](https://github.com/cfeck), initial Qt5 version
- [Michael Abaev](https://github.com/Lemontyash), initial Qt6 version, code refactoring
- [Pedro López-Cabanillas](https://github.com/pedrolcl), code refactoring, fixing build warnings
- [Miroslav Bendík](https://github.com/mireq), bringing back GUI configuration tool
- [Alexander Tolstoy](https://github.com/atolstoy), project management


