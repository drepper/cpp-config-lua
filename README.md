Configuration for C++ code using Lua
====================================

Enable the user to configure a program is usually necessary.  Im GUI programs
it is often sufficient to provide a way to change the configuration through
the user interface. But for all program it is generally better to also provide
a means to provide a configuration through files.

Many formats exist, implemented through libraries with bindings for all
languages.  Formats like YAML or INI are commonly used but they have one
limitation: they provide only static configuration possibilities.

Maybe the configuration should be dynamic.  If the configuration allows
defining functions the user can arbitrarily extend the configuration files.
Implementing such an environment would be overkill.  We could just use an
existing scripting language.

Entry Lua
---------

One possible implementation is to use Lua. Lua is often used in C/C++ code
as an extension language.  The implementation is available in the form of
a library and the code and runtime requirements are not big.

The code in this project allows to use Lua easily in C++ code.  The code in
`test.cc` shows the use of the code and the `Makefile` shows how to compile
the code.  One possible way to use the code:

* install a file like `config` in a read-only in a data directory (e.g., in a
  per-project directory under `/usr/share`)
* changable configuration files for the system (under `/etc`) and in the user's
  home directory (under `$HOME/.config`) allow ample opportunity for customization

The locations of the configuration files and the files themselves can be predictable,
as it is the case in the provided `config` file.

Caution
-------

Providing the possibility to add code to the configuration files adds risk. This
form of configuration handling is not the right approach for all programs and/or
situations. The risks and benefits must be weight.
