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

#ifndef __NASIU__SCRIPTING__V8W__COMMON_HPP__
#define __NASIU__SCRIPTING__V8W__COMMON_HPP__

#include <nasiu/scripting/v8w/v8w.hpp>

#include <exception>
#include <string>
#include <vector>
#include <list>
#include <map>

struct checker
{
	static int instances_;

	checker(const checker& other)
	{
		instances_++;
		*this = other;
	}

	checker()
	{
		instances_++;
	}

	virtual ~checker()
	{
		instances_--;
	}
};

int checker::instances_ = 0;

class test_exception : public std::exception, virtual public checker
{
	std::string msg_;

public:
	test_exception()
	{
	}

	test_exception(
			const std::string& msg)
	: msg_(msg)
	{
	}

	virtual
	~test_exception() throw()
	{
	}

	const char*
	what() const throw()
	{
		return msg_.c_str();
	}
};

NASIU_V8W_ADAPT_CLASS(
	test_exception,
	,
	,
	(what),
);

class first_exception : public test_exception
{
public:
	first_exception()
	{
	}

	first_exception(
			const std::string& msg)
	: test_exception(msg)
	{
	}

	virtual
	~first_exception() throw()
	{
	}
};

NASIU_V8W_ADAPT_CLASS(
	first_exception,
	test_exception,
	,
	(what),
);

class second_exception : public test_exception
{
public:
	second_exception()
	{
	}

	second_exception(
			const std::string& msg)
	: test_exception(msg)
	{
	}

	virtual
	~second_exception() throw()
	{
	}
};

NASIU_V8W_ADAPT_CLASS(
	second_exception,
	test_exception,
	,
	(what),
);


struct abstract_class : virtual public checker
{
	virtual ~abstract_class() {}

	virtual int method_a(bool) = 0;
	virtual std::string method_b()
	{
		return "abstract_class::method_b";
	}
	virtual std::string method_b(const std::string& n)
	{
		return "abstract_class::method_b(const std::string&)";
	}
};

NASIU_V8W_RESOLVE_OVERLOADS(
	abstract_class,
	(method_b,std::string,(const std::string&))
)

NASIU_V8W_ADAPT_CLASS(
	abstract_class,
	,
	,
	(method_a)
	(method_b),
)

class class_a : public abstract_class
{
	int n_;
	std::string m_;

public:
	class_a()
	: n_(0)
	{
	}

	class_a(int n, const std::string& m)
	: n_(n), m_(m)
	{
	}

	virtual ~class_a()
	{
	}

	virtual int method_a(bool)
	{
		return n_ * n_;
	}

	std::string method_b()
	{
		return abstract_class::method_b();
	}

	void method_c(int a)
	{
	}

	void method_d(const std::vector<int>& v)
	{
	}

	void method_e(std::list<int> l)
	{
	}

	void method_f(const std::map<int, std::string>& m)
	{
		throw second_exception("method_f");
	}

	int get_n() const
	{
		return n_;
	}

	void set_n(int n)
	{
		n_ = n;
	}

	std::string get_m() const
	{
		return m_;
	}
};


NASIU_V8W_ADAPT_CLASS(
	class_a,
	abstract_class,
	(int)(const std::string&),
	(method_a)
	(method_b)
	(method_c)
	(method_d)
	(method_e)
	(method_f),
	("n", get_n, set_n)
	("m", get_m,)
);

int func_a();
abstract_class* func_b();
int func_c(class_a* a);

#endif /* __NASIU__SCRIPTING__V8W__COMMON_HPP__ */
