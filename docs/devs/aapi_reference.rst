Applet API reference
====================

=========
Functions
=========

----------------------
Checking key existance
----------------------

.. doxygenfunction:: wapi_key_exists
.. doxygenfunction:: wapi_index_exists

----------------------------------------
Extracting variable from key directories
----------------------------------------

.. doxygenfunction:: wapi_get_var_from_table
.. doxygenfunction:: wapi_get_var_from_array


---------------------------
Converting variable to type
---------------------------


.. doxygenfunction:: wapi_var_as_string
.. doxygenfunction:: wapi_var_as_integer
.. doxygenfunction:: wapi_var_as_floating
.. doxygenfunction:: wapi_var_as_boolean


--------------
Error handling
--------------

.. doxygenfunction:: wapi_error
.. doxygenfunction:: wapi_error_print


=======
Structs
=======

.. doxygenstruct:: _wap_t_config_variable
    :members:
    :private-members:
    :undoc-members:

.. doxygenstruct:: wap_t_applet_config
    :members:
    :private-members:
    :undoc-members:

.. doxygenstruct:: wap_t_applet_info
    :members:
    :private-members:
    :undoc-members:

=====
Enums
=====

.. doxygenenum:: wap_t_config_var_type
.. doxygenenum:: wap_t_error_type