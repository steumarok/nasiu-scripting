This project allow the interoperability between C++ and scripting languages. C++ features like virtual functions, inheritance, exceptions are exposed to script language.

Multiple scripting language are supported (for now only Google V8 is implemented, next step is Python).

Main features are:
  * Use C++ class and functions from script
  * No required changes of existing classes
  * Derivation of classes (even abstract) in script and use in C++
  * Automatic conversion of native types and STL containers (list, map, vector)
  * User defined type conversions
  * Throw exception from script and catch in C++ and viceversa
  * Simple API designed for easy usage in mind.

See how to [interface C++ class and functions](use.md) to script engine.

For more details, see [Reference](reference.md).

email << "stefano.marocco" << "@" << "gmail.com";