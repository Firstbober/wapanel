Configuration file
==================

Wapanel uses a config file to store and configure panels and applets.
It should sit beside binary file or in **~/.config/** as *wapanel.toml*.

Structure
---------

.. code-block:: toml

    [[panel]]
        position = "bottom"
        height = 32

This is the most basic configuration. We have here one panel with height of 32.
We can add as many panels as we want. Nevertheless, if we don't have any space left on the screen then it may crash ;).


.. code-block:: toml

    [[panel]]
        position = "bottom"
        height = 32
        [[panel.applet]]
            name = "clock"

    [[panel]]
        position = "top"
        height = 16

Now we have two panels with one containing a **clock** applet. Every applet in config has a name that identifies it,
but names can duplicate if we want to have multiple instances of specified applet.

For more sophisticated information go to `TOML specification <https://github.com/toml-lang/toml>`_

Panel
-----

.. code-block:: toml

    [[panel]]
        position = "bottom"
        height = 32

As it is described above, panel has only two unique options, without counting applet table.

========= =========================== ========
Parameter Description                 Content
--------- --------------------------- --------
position  Position of panel on screen bottom or top
height    Height of panel             from 0 to infinity basically
========= =========================== ========

CSS:

.panel - panel window

#panel-id - specific panel window

Applets
-------

.. code-block:: toml

    [[panel.applet]]
        name = "clock"

The only thing that is reserved for applet is name. 
Applet can use any other key for its data. 
It is the most common to define specific applet config after the **name** key as its increases readability. 
I think **name** is self-explanatory.