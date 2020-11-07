.. image:: resources/wapa-logo-color.svg
	:width: 100
	:alt: Wapanel logo

=======
Wapanel
=======

The panel all you need.
Simple panel for Wayland with decent XFCE-like applets. Made for my needs but maybe it will fill your needs too.

Features
========

- Decent configurability
- Commonly used applets like task switcher or app finder
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
- ☑ Activator
- ☐ App finder

Installation
============

------------
Dependencies
------------

- Meson build system *
- C and C++ compiler *
- pkg-config *
- GTK+3 with Wayland support
- gtk-layer-shell
- xdg-utils (for Activator applet)

`*` indicates compile-time dependencies

--------------------
Building from source
--------------------

Be sure you have installed all listed dependencies and enter:

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
