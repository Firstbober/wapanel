How to write applets for Wapanel
================================

So you want to write applet for Wapanel, right?

You will need at least basic knowledge of C or/and C++,
understanding of Meson build system will be useful too.
But C/C++ knowledge should be enough.

Let's start with creating directory for our applet
in this guide I will name it `example`. Then create some basic files:

- meson.build
- example.c

They will be our basis for further operations.

**meson.build** should look something like this:

.. code-block::

    project('example', 'c')

    library(
        'wapanel-applet-example',
        'example.c',
        dependencies: [
            dependency('gtk+-3.0'),
            dependency('wapanel-appletapi')
        ],
        install: true,
        install_dir: 'lib/wapanel/applets',
    )

We have here our project name, source filename **example.c**, required dependencies
**gtk+3** and **wapanel-appletapi** (these two will be installed if you have wapanel in your system).
Our install directory is `lib/wapanel/applets` that meson resolve to `/usr/lib/wapanel/applets` -
in this directory all applets are installed system-wide. You can also install applets to:

- ~/.local/share/wapanel/applets
- besides binary in **wapanel/applets** directory

Let's take a look at **example.c**

.. code-block:: c

    #include <wapanel-appletapi/appletapi.h>
    #include <gtk/gtk.h>

    // Called to get handy info about this applet.
    wap_t_applet_info wap_applet_info() {
        wap_t_applet_info info = { .name = "example", .version = 1 };
        return info;
    }

    // Called when some panel need a new instance of your applet.
    GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
        return gtk_label_new("Hello, world!");
    }

    // Called when requested to remove all existing instances. GtkWidget should already be disposed by panel.
    void wap_event_remove_instances() {}

    // Called when panel exits.
    void wap_event_exit() {}

There is slightly more code. Here is small explanation:

Includes:

- #include <wapanel-appletapi/appletapi.h>
    Include directive for applet API required for applets.
- #include <gtk/gtk.h>
    Just basic GTK include. You will need this anyways.

Callbacks:

- wap_t_applet_info wap_applet_info()
    This function sends information about your applet to panel.
- GtkWidget \*wap_applet_new_instance(wap_t_applet_config applet_config)
    Panel calls this to get brand new GtkWidget, you can do some fancy stuff — remember to clean it up later.
- void wap_event_remove_instances()
    Here you can make cleaning of unspeakable things you done before.
- void wap_event_exit()
    You probably never need to write any code here.

When we have our source and build files we can make our project live!
Make **build** directory, go to it and then call `meson ..` and `ninja`.
Everything should build and you will be greeted with beautiful shared object file.
Then you can put it in system path or local path as described above.

**Congratulations! You just created your first applet!**