# Wapanel
Simple panel/status bar/task bar for your custom *stacking* Wayland-based desktop.
Documentation: https://firstbober.github.io/wapanel

<img src="/resources/wapa-logo-color.svg?raw=true" width="100" height="100">

## Features
- Good configurability
- Config hot reload
- Exposed API for writing custom applets
- Custom themes with CSS

## Screenshot

<img src="/wapanel-example.webp?raw=true" width="640">

## Maintaining
I will be trying to fix incoming issues in issue tracker but not add new features, if you want some upstream then pull request are open.

## Building
### Dependencies
- Meson *
- C/C++ compiler *
- gtk-layer-shell (if not found, will be build automatically)
- GTK+ 3
- wayland-protocols
- wayland-client
- wayland-scanner *
- libpulse
- xdg-utils
- pkg-config *

Asterisk indicates compile-time dependencies.

### Cloning
```sh
git clone --recurse-submodules https://github.com/Firstbober/wapanel.git
```

### Compiling and installation
```sh
mkdir build
cd build
meson ..
sudo ninja install
```

If you want to not build some applets then use toggle -Dname=false. List of them is in meson_options.txt e.g. -Dactivator=false. This will disable building of an activator applet.

## Contribute
- Issue Tracker: https://github.com/Firstbober/wapanel/issues
- Source Code: https://github.com/Firstbober/wapanel

## License
The project is licensed under the MIT license.
