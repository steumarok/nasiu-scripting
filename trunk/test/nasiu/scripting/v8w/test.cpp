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

#include <typeinfo>
#include <map>
#include "nasiu/scripting/v8w/common.hpp"
#include "gtest/gtest.h"

using namespace nasiu::scripting;
using namespace std;

int func_a()
{
	return 42;
}

abstract_class* func_b()
{
	return new class_a();
}

int func_c(class_a* a)
{
	a->method_a(false);
	a->get_n();
}

void func_d()
{
	throw "An exception";
}

void func_e()
{
	throw first_exception("An exception");
}

class test_interceptor : public v8w::default_interceptor
{
public:
	v8::Handle<v8::Value> on_native_call(v8w::native_caller_base& caller, v8w::invocation_scope& scope)
	{
		try
		{
			return default_interceptor::on_native_call(caller, scope);
		}
		catch (const first_exception& e)
		{
			throw v8w::exception_wrapper<first_exception>(e);
		}
		catch (const second_exception& e)
		{
			throw v8w::exception_wrapper<second_exception>(e);
		}
	}
};

class v8w_test : public ::testing::Test
{
protected:
	script_engine<tags::v8w> se;
	class_type<tags::v8w, abstract_class> js_class_1;
	abstract_class* object_1;


	void SetUp()
	{
		se.bind_class<abstract_class>("abstract_class");
		se.bind_class<class_a>("class_a");
		se.bind_function("func_a", func_a);
		se.bind_function("func_b", func_b);
		se.bind_function("func_c", func_c);
		se.bind_function("func_d", func_d);
	}

	void TearDown()
	{
	}
};

TEST_F(v8w_test, functions)
{
	EXPECT_EQ(se.eval<int>("func_a()"), 42);
	EXPECT_NE(se.eval<abstract_class*>("func_b()"), (abstract_class*)0);
	EXPECT_EQ(se.eval<int>("func_c(new class_a(10))"), 10);
}

TEST_F(v8w_test, classes)
{
	//
	// Define a JS class derived from abstract_class.
	//
	js_class_1 = se.eval<class_type<tags::v8w, abstract_class> >(
				"(function() {"
					"js_class = function()"
					"{"
					"	var base = new abstract_class();"
					"	base.__proto__ = js_class.prototype;"
					"	return base;"
					"};"
					"js_class.prototype.__proto__ = abstract_class.prototype;"
					"js_class.prototype.method_a = function(val)"
					"{"
					"   if (val)"
					"		abstract_class.prototype.method_a.call(this);"
					"};"
					"js_class.prototype.method_b = function()"
					"{"
					"	var r = abstract_class.prototype.method_b.call(this);"
					"	return r.replace('abstract_class', 'js_class');"
					"};"
					"return js_class;"
				"})()"
	);

	//
	// Create a new instance of JS class and perform tests.
	//
	ASSERT_NO_THROW(
	{
		object_1 = js_class_1.new_instance();
	});

	EXPECT_NE(object_1, (abstract_class*)0);
	EXPECT_EQ(object_1->method_b("prova"), "js_class::method_b(const std::string&)");

	//
	// Check exception on virtual abstract function call.
	//
	ASSERT_THROW(
	{
		object_1->method_a(true);
	},
	script_exception);
}


TEST_F(v8w_test, exceptions)
{
	script_engine<tags::v8w> se;

	se.bind_class<class_a>("class_a");
	se.bind_exception<test_exception>("test_exception");
	se.bind_exception<first_exception>("first_exception");
	se.bind_exception<second_exception>("second_exception");
	se.bind_function("func_d", func_d);
	se.bind_function("func_e", func_e);

	ASSERT_THROW(
	{
		se.eval<int>("throw 'exception';");
	},
	script_exception);

	EXPECT_EQ(se.eval<std::string>(
			"(function(){"
				"try "
				"{"
				"	func_d();"
				"}"
				"catch (e)"
				"{"
				"	return e;"
				"};"
			"})()"), "Unknown exception");

	EXPECT_EQ(se.eval<std::string>(
			"(function(){"
				"try "
				"{"
				"	func_e();"
				"}"
				"catch (e)"
				"{"
				"	return e.what();"
				"};"
			"})()"), "An exception");

	ASSERT_THROW(
	{
		se.exec(
			"(function(){"
				"try "
				"{"
				"	func_e();"
				"}"
				"catch (e)"
				"{"
				"	throw e"
				"};"
			"})()");
	},
	first_exception);

	ASSERT_THROW(
	{
		se.exec("func_e()");
	},
	first_exception);

	try
	{
		se.exec("throw {'a': 10}");
	}
	catch (const v8w::v8w_exception& ex)
	{
		typedef map<string, int> map_t;
		map_t ex_map = ex.cast<map_t>();

		EXPECT_EQ(ex_map["a"], 10);
	}

	ASSERT_THROW(
	{
		se.exec("(new class_a()).method_f()");
	},
	second_exception);

	typedef std::map<int, std::string> map_t;
	ASSERT_THROW(
	{
		class_a* a = se.eval<class_a*>("new class_a()");
		map_t m;
		a->method_f(m);
	},
	second_exception);

	ASSERT_THROW(
	{
		se.exec("I_not_exists");
	},
	script_exception);

	ASSERT_THROW(
	{
		se.exec("var var n = 10;");
	},
	script_exception);
}

TEST_F(v8w_test, gc)
{
	v8w::invoke_gc();

	EXPECT_EQ(checker::instances_, 0);
}
