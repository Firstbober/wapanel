.. image:: resources/wapa-logo-color.svg
	:width: 100
	:alt: Wapanel logo

=======
Wapanel
=======

The panel all you need.
Probably first non-tiling-targeted panel for Wayland with decent XFCE-like applets. Until we get port of some lightweight DE to Wayland :)

Features
========

- Decent configurability
- Useful applets like task switcher or app finder
- Config hot reloading
- Wayland compatible
- Exposed API for writing custom applets

Applets
=======

- ☑ Clock
- ☑ Separator
- ☐ System tray (Freedesktop StatusNotifierItem implementation)
- ☐ Volume control (PulseAudio, PipeWire in future)
- ☑ Task switcher
- ☐ Activator
- ☐ App finder

Installation
============

------------
Dependencies
------------

- Meson build systemt
- GTK+3 with Wayland support
- gtk-layer-shell
- pkg-config
- xdg-utils (for Activator applet)
- C and C++ compiler

--------------------
Building from source
--------------------

Be sure you have all listed dependencies and enter:

.. code-block:: sh

	$ mkdir build
	$ cd build
	$ meson ..
	$ sudo ninja install

Everything should build and install.

Contribute
==========

- Issue Tracker: https://github.com/Firstbober/wapanel/issues
- Source Code: https://github.com/Firstbober/wapanel

Support
=======
If you are having issues, you can ask about it in issue tracker or contact me directly on Discord (Firstbober#1822) or Telegram (@Firstbober)

License
=======

The project is licensed under the MIT license.
