// Copyright (c) 2011, Stefano Marocco
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY STEFANO MAROCCO ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL STEFANO MAROCCO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <nasiu/scripting/v8w/v8w.hpp>

#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <list>
#include <map>


using namespace std;
using namespace v8;


struct Base
{
	virtual ~Base() {}
	virtual void method0() = 0;
};

NASIU_V8W_ADAPT_CLASS(
	Base, ,
	,
	(method0),);

class A
{
	int n_;

public:
	A()
	: n_(0)
	{
		std::cout << "A::A()" << std::endl;
	}

	A(int n)
	: n_(n)
	{
		std::cout << "A::A(" << n << ")" << std::endl;
	}

	virtual ~A()
	{
		std::cout << "A::~A(" << n_ << ")" << std::endl;
	}

	virtual void method0()
	{
		std::cout << "A::method0()" << std::endl;
	}

	virtual void method1(
			int a/*,
			const std::vector<int>& v,
			std::list<int>,
			const std::map<int, std::string>&*/)
	{
		std::cout << "A::method1("<<a<<")" << std::endl;
	}

	void method2(int n)
	{
		std::cout << "A::method2(" << n << ")" << std::endl;
		n_ = n;
	}

	int method3()
	{
		std::cout << "A::method3()" << std::endl;
		return n_;
	}

	int get_n() const
	{
		return n_;
	}

	void set_n(int n)
	{
		n_ = n;
	}

};


NASIU_V8W_ADAPT_CLASS(
	A, Base,
	(int),
	(method0)
	(method1)
	(method2)
	(method3),
	("n", get_n, set_n)
	("n1", get_n,));


class B : public A
{
public:
	B() {}

	B(int n, float)
	: A(n)
	{
		std::cout << "B::B()" << std::endl;
	}

	~B()
	{
		std::cout << "B::~B()" << std::endl;
	}

	void method1(int a)
	{
		std::cout << "B::method1("<< a <<")" << std::endl;

		A::method1(a);
	}

	void method4(const B& b)
	{
		std::cout << "B::method4()" << std::endl;
	}

	std::vector<int> method5(A* a)
	{
		std::cout << "B::method5()" << std::endl;
		std::vector<int> v;
		v.push_back(100);
		return v;
	}

	A* method6()
	{
		std::cout << "B::method6()" << std::endl;
		return new A(80);
	}

	A& method7()
	{
		static A a(8);
		return a;
	}

	int method8()
	{
		static int n = 10;
		return n;
	}

	std::vector<A*> method9()
	{
		std::cout << "B::method9()" << std::endl;
		std::vector<A*> v;
		v.push_back(new A(90));
		return v;
	}

	std::tm method10(std::tm t)
	{
		std::cout << "B::method10()" << std::endl;
		std::cout << t.tm_min << std::endl;
		t.tm_mon = 10;
		return t;
	}
};


NASIU_V8W_ADAPT_CLASS(
	B, A,
	(int)
	(float),
	(method0)
	(method1)
	(method2)
	(method3)
	(method5)
	(method6)
	(method7)
	(method8)
	(method9)
	(method10),
	("n", get_n, set_n)
	("n1", get_n,));



int func1(A* a)
{
	cout << "func1 " << a->get_n() << endl;
	return a->get_n();
}

int func2(int n)
{
	return n*n;
}

void func3()
{
	throw nasiu::scripting::native_exception("native error");
}

string get_content(const string& f)
{
	ostringstream stream_source;
	{
		string line;
		ifstream myfile(f.c_str());
		if (myfile.is_open())
		{
			while (myfile.good())
			{
				getline(myfile, line);
				stream_source << line << endl;
			}
		}
	}
	return stream_source.str();
}



int main()
{
	{
		using namespace nasiu::scripting;

		script_engine<tags::v8w> se;

		se.bind_class<Base>();
		se.bind_class<A>();
		se.bind_class<B>();
		se.bind_function("func1", func1);
		se.bind_function("func2", func2);

		se.set("var1", "var1-content");
		se.set("var2", 10000);
		se.set("var3", new A(84));

		int h = se.eval<int>("func2(20)");

		cout << "func2 " << h << endl;

		{
			se.exec("func1(new A(var2))");
			se.exec("func1(var3)");

			v8w::invoke_gc();
		}

		A* a1 = se.eval<A*>("new A(20)");
		cout << "a1.n=" << a1->get_n() << endl;

		class_type<tags::v8w, A> type_A = se.eval<class_type<tags::v8w, A> >("A");
		A* a2 = type_A.new_instance(30);
		cout << "a2.n=" << a2->get_n() << endl;
		delete a2;

		class_type<tags::v8w, Base> type_ExtBase = se.eval<class_type<tags::v8w, Base> >(
				"ExtBase = function()"
				"{"
				"	var base = new Base();"
				"	base.__proto__ = ExtBase.prototype;"
				"	return base;"
				"};"
				"ExtBase.prototype.__proto__ = Base.prototype;"
				"ExtBase.prototype.method0 = function(n)"
				"{"
				"	/*Base.prototype.method0.call(this);*/"
				"};"
				"ExtBase"
		);
		Base* base = type_ExtBase.new_instance();
		cout << "base.method0() "; base->method0(); cout << endl;
//		delete base;


		class_type<tags::v8w, A> type_ExtA = se.eval<class_type<tags::v8w, A> >(
				"ExtA = function(n)"
				"{"
				"	var base = new A(n);"
				"	base.__proto__ = ExtA.prototype;"
				"	return base;"
				"};"
				"ExtA.prototype.__proto__ = A.prototype;"
				"ExtA.prototype.method1 = function(n)"
				"{"
				"	A.prototype.method1.call(this, n*n);"
				"};"
				"ExtA.prototype.method0 = function(n)"
				"{"
				"	A.prototype.method0.call(this);"
				"};"
				"ExtA"
		);
		A* a3 = type_ExtA.new_instance(40);
		cout << "a3.n=" << a3->get_n() << endl;
		cout << "a3.method0() "; a3->method0(); cout << endl;
		cout << "a3.method1() "; a3->method1(40); cout << endl;
		delete a3;

		class_type<tags::v8w, B> type_ExtB = se.eval<class_type<tags::v8w, B> >(
				"ExtB = function(n, f)"
				"{"
				"	var base = new B(n, f);"
				"	base.__proto__ = ExtB.prototype;"
				"	return base;"
				"};"
				"ExtB.prototype.__proto__ = B.prototype;"
				"ExtB.prototype.method1 = function(n)"
				"{"
				"	B.prototype.method1.call(this, n*n);"
				"};"
				"ExtB"
		);
		B* b1 = type_ExtB.new_instance(50, 100.03);
		cout << "b1.n=" << b1->get_n() << endl;
		cout << "b1.method1() "; b1->method1(50); cout << endl;
		//delete a3;

		cout << "ExtB instanceof B: " << se.eval<bool>("(new ExtB(60,120.3)) instanceof B") << endl;

		v8w::invoke_gc();


		function<tags::v8w> func_jsfn1 = se.eval<function<tags::v8w> >(
				"(function (a, b)"
				"{"
				"	return a + b;"
				"})"
		);
		cout << "jsfn1(10,30) = " << func_jsfn1.call<std::string>(10, 30) << endl;

		function<tags::v8w> func_jsfn2 = se.eval<function<tags::v8w> >(
				"(function (fn, a)"
				"{"
				"	return fn(a, a);"
				"})"
		);
		cout << "jsfn2(jsfn1, 40) = " << func_jsfn2.call<int>(func_jsfn1, 40) << endl;


		try
		{
			se.exec("throw 'error';");
		}
		catch (script_exception& e)
		{
			cout << e.what() << endl;
		}

		se.bind_function("func3", func3);
		cout << "result: " << se.eval<std::string>(
				"result = '';"
				"try "
				"{"
				"	func3();"
				"}"
				"catch (e)"
				"{"
				"	result = e;"
				"};"
				"result"
		) << endl;

		function<tags::v8w> func_jsfn3 = se.eval<function<tags::v8w> >(
				"(function (a, b)"
				"{"
				"	throw a + b;"
				"})"
		);
		try
		{
			func_jsfn3.call<void>(10, 30);
		}
		catch (script_exception& e)
		{
			cout << "jsfn3: " << e.what() << endl;
		}

		class_type<tags::v8w, B> type_ExtB2 = se.eval<class_type<tags::v8w, B> >(
				"ExtB2 = function(n, f)"
				"{"
				"	var base = new B(n, f);"
				"	base.__proto__ = ExtB2.prototype;"
				"	return base;"
				"};"
				"ExtB2.prototype.__proto__ = B.prototype;"
				"ExtB2.prototype.method1 = function(n)"
				"{"
				"	throw n*n;"
				"};"
				"ExtB2"
		);
		B* b2 = type_ExtB2.new_instance(60, 100.009);

		try
		{
			cout << "b2.method1() "; b2->method1(50); cout << endl;
		}
		catch (script_exception& e)
		{
			cout << "b2.method1 raise: " << e.what() << endl;
		}


		cout << "string-to-int: " << se.eval<int>("'300'") << endl;
		cout << "string-to-float: " << se.eval<float>("'10.44'") << endl;
		cout << "string-to-double: " << se.eval<double>("'10.440000089'") << endl;

		cout << "undefined-to-double: " << se.eval<double>("undefined") << endl;
		cout << "undefined-to-int: " << se.eval<int>("undefined") << endl;

		cout << "null-to-double: " << se.eval<double>("null") << endl;
		cout << "null-to-int: " << se.eval<int>("null") << endl;
		cout << "null-to-string: " << se.eval<std::string>("null") << endl;
		cout << "null-to-bool: " << se.eval<bool>("null") << endl;

		function<tags::v8w> func_jsfn4 = se.eval<function<tags::v8w> >(
				"(function (a)"
				"{"
				"	return a;"
				"})"
		);
		func_jsfn4.call<bool>(nil());
		func_jsfn4.call<bool>(undefined());

		std::map<int, std::string> m, mr;
		m.insert(std::map<int, std::string>::value_type(200, "item1"));
		mr = func_jsfn4.call<std::map<int, std::string> >(m);

		cout << "result map size: " << mr.size() << endl;


		std::list<std::string> l, lr;
		l.push_back("item1");
		lr = func_jsfn4.call<std::list<std::string> >(l);

		cout << "result list size: " << lr.size() << endl;


		return 0;
	}

	return 0;
}
