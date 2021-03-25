Using wap_t_applet_config in your applet
========================================

You have probably seen `wap_applet_new_instance` function before as you was trying
to make your first applet and saw **wap_t_applet_config applet_config**. Here is
little tutorial how to use it to make your applet configurable.

.. code-block:: c

    if (wapi_key_exists(&applet_config.root, "your_name")) {
        _wap_t_config_variable *variable = \
            wapi_get_var_from_table(&applet_config.root, "your_name");
        const char *name;

        if (variable->type == WAP_CONF_VAR_TYPE_STRING) {
            name = wapi_var_as_string(variable);
        }
    }

As you can see, this is very simple code. Checking if key exists,
checking variable type and packing it into our string. There is more
**wapi_var_as** functions, go to AAPI reference for more.

Some keys are reserved, they start with `__`. Here is a list of them:

============== ============================================
**Name**       **Description**
-------------- --------------------------------------------
__panel_height Height of panel which applet is contained in
============== ============================================

Basically, there is nothing more for you to know. You can
eventually use **wapi_error** and **wapi_error_print** to handle more errors,
but if everything is checked before you will be safe.