

A script engine for C++

This project allow the interoperability between C++ and scripting languages. C++ features like virtual functions, inheritance, exceptions are exposed to script language.

Multiple scripting language are supported (for now only Google V8 is implemented, next step is Python).

Main features are: * Use C++ class and functions from script * No required changes of existing classes * Derivation of classes (even abstract) in script and use in C++ * Automatic conversion of native types and STL containers (list, map, vector) * User defined type conversions * Throw exception from script and catch in C++ and viceversa * Simple API designed for easy usage in mind.

See how to interface C++ class and functions to script engine.

For more details, see Reference.

email << "stefano.marocco" << "@" << "gmail.com";




How to interface C++ and scripting languages

This example show how:

    Bind classes and functions to script engine
    Class instantiation and usage
    Derive classes and override functions in Javascript
    Exception handling

Full source: http://code.google.com/p/nasiu-scripting/source/browse/trunk/examples/nasiu/scripting/v8w/example1.cpp
Classes and functions binding

Our example class and functions definition:

``` class A { int n_;

public: A(); A(int n); virtual ~A();

void method0(); void method1(int a); int get_n() const; void set_n(int n); };

int func1(A* a) { std::cout << "func1 " << a->get_n() << std::endl; return a->get_n(); } ```

Adapt class for script: ```cpp

NASIU_V8W_ADAPT_CLASS( A, , (int), (method0) (method1), ("n", get_n, set_n) ("n1", get_n, )); ```

Bind class and function to script engine: ``` using namespace nasiu::scripting;

script_engine se; se.bind_class(); se.bind_function("func1", func1); ```

Assign script scope variables: se.set("var1", "var1-content"); se.set("var2", 10000); se.set("var3", new A(84));
Instantiation and invocation

Use the function func1 and a script variable: se.exec("func1(new A(var2))");

Instantiate A from Javascript and use in C++. ``` A* a1 = se.eval("new A(20)");

cout << "a1.n=" << a1->get_n() << endl; ```

Return the class from Javascript, instantiate, use, delete in C++: ``` class_type type_A = se.eval >("A");

A* a2 = type_A.new_instance(30); cout << "a2.n=" << a2->get_n() << endl;

delete a2; ```
Function overriding

Override some functions of A in Javascript, return the class and use in C++: ``` class_type type_ExtA = se.eval >( "ExtA = function(n)" "{" " var base = new A(n);" " base.proto = ExtA.prototype;" " return base;" "};" "ExtA.prototype.proto = A.prototype;" "ExtA.prototype.method1 = function(n)" "{" " A.prototype.method1.call(this, n*n);" "};" "ExtA.prototype.method0 = function(n)" "{" " A.prototype.method0.call(this);" "};" "ExtA" );

A* a3 = type_ExtA.new_instance(40); cout << "a3.n=" << a3->get_n() << endl; cout << "a3.method0() "; a3->method0(); cout << endl; cout << "a3.method1() "; a3->method1(40); cout << endl;

delete a3; ```
Exception handling

Define a Javascript function that throw a exception and catch it in C++: function<tags::v8w> js_func = se.eval<function<tags::v8w> >( "(function (a, b)" "{" " throw a + b;" "})" ); try { js_func.call<void>(10, 30); } catch (script_exception& e) { cout << "js_func exception: " << e.what() << endl; }

Catch C++ exceptions from Javascript: ``` void func2(int n) { throw "native exception"; } se.bind_function("func2", func2);

se.exec( "try " "{" " func2(20);" "}" "catch (e)" "{" " /* ... */" "};" ); ```
