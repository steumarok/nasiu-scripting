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

#if !defined(__NASIU__SCRIPTING__V8W__NATIVE_TO_JS_HPP__)
#define __NASIU__SCRIPTING__V8W__NATIVE_TO_JS_HPP__

#include <vector>
#include <map>
#include <list>
#include <boost/date_time.hpp>
#include <v8.h>

#include <nasiu/scripting/v8w/invocation_scope.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

template<typename T>
struct native_to_js
{
	v8::Handle<v8::Value>
	operator()(
			const T& from,
			invocation_scope& scope)
	{
		return v8::Undefined();
	}
};

template<>
struct native_to_js<short>
{
	v8::Handle<v8::Value>
	operator()(
			const short& from,
			invocation_scope& scope)
	{
		return v8::Int32::New(from);
	}
};

template<>
struct native_to_js<unsigned short>
{
	v8::Handle<v8::Value>
	operator()(
			const unsigned short& from,
			invocation_scope& scope)
	{
		return v8::Uint32::New(from);
	}
};

template<>
struct native_to_js<int>
{
	v8::Handle<v8::Value>
	operator()(
			const int& from,
			invocation_scope& scope)
	{
		return v8::Integer::New(from);
	}
};

template<>
struct native_to_js<unsigned int>
{
	v8::Handle<v8::Value>
	operator()(
			const unsigned int& from,
			invocation_scope& scope)
	{
		return v8::Integer::New(from);
	}
};

template<>
struct native_to_js<long long>
{
	v8::Handle<v8::Value>
	operator()(
			const long long& from,
			invocation_scope& scope)
	{
		return v8::Integer::New(from);
	}
};

template<>
struct native_to_js<unsigned long long>
{
	v8::Handle<v8::Value>
	operator()(
			const unsigned long long& from,
			invocation_scope& scope)
	{
		return v8::Integer::New(from);
	}
};

template<>
struct native_to_js<float>
{
	v8::Handle<v8::Value>
	operator()(
			const float& from,
			invocation_scope& scope)
	{
		return v8::Number::New(from);
	}
};

template<>
struct native_to_js<double>
{
	v8::Handle<v8::Value>
	operator()(
			const double& from,
			invocation_scope& scope)
	{
		return v8::Number::New(from);
	}
};

template<>
struct native_to_js<long double>
{
	v8::Handle<v8::Value>
	operator()(
			const long double& from,
			invocation_scope& scope)
	{
		return v8::Number::New(from);
	}
};

template<>
struct native_to_js<bool>
{
	v8::Handle<v8::Value>
	operator()(
			const bool& from,
			invocation_scope& scope)
	{
		return v8::Boolean::New(from);
	}
};

template<>
struct native_to_js<std::string>
{
	v8::Handle<v8::Value>
	operator()(
			const std::string& from,
			invocation_scope& scope)
	{
		return v8::String::New(from.c_str());
	}
};

template<>
struct native_to_js<char*>
{
	v8::Handle<v8::Value>
	operator()(
			const char*& from,
			invocation_scope& scope)
	{
		return v8::String::New(from);
	}
};

template<>
struct native_to_js<std::tm>
{
	v8::Handle<v8::Value>
	operator()(
			const std::tm& from,
			invocation_scope& scope)
	{
		std::time_t raw_time = mktime(const_cast<std::tm*>(&from));

		return v8::Date::New(raw_time * 1000);
	}
};

template<>
template<typename X>
struct native_to_js<std::vector<X> >
{
	v8::Handle<v8::Value>
	operator()(
			const std::vector<X>& from,
			invocation_scope& scope)
	{
		using namespace v8;

		HandleScope handle_scope;

		Context::Scope context_scope(scope.get_engine().get_context());

		Handle<Array> array = Array::New(from.size());

		typedef typename std::vector<X>::const_iterator iterator_type;

		int i = 0;
		for (iterator_type it = from.begin(); it != from.end(); ++it)
		{
			array->Set(i++, native_to_js<X>()(*it, scope));
		}

		return handle_scope.Close(array);
	}
};

template<>
template<typename X>
struct native_to_js<std::list<X> >
{
	v8::Handle<v8::Value>
	operator()(
			const std::list<X>& from,
			invocation_scope& scope)
	{
		using namespace v8;

		HandleScope handle_scope;

		Context::Scope context_scope(scope.get_engine().get_context());

		Handle<Array> array = Array::New(from.size());

		typedef typename std::list<X>::const_iterator iterator_type;

		int i = 0;
		for (iterator_type it = from.begin(); it != from.end(); ++it)
		{
			array->Set(i++, native_to_js<X>()(*it, scope));
		}

		return handle_scope.Close(array);
	}
};

template<>
template<typename K, typename V>
struct native_to_js<std::map<K, V> >
{
	v8::Handle<v8::Value>
	operator()(
			const std::map<K, V>& from,
			invocation_scope& scope)
	{
		using namespace v8;

		HandleScope handle_scope;

		Context::Scope context_scope(scope.get_engine().get_context());

		Handle<Object> object = Object::New();

		typedef typename std::map<K, V>::const_iterator iterator_type;

		for (iterator_type it = from.begin(); it != from.end(); ++it)
		{
			object->Set(
					native_to_js<K>()(it->first, scope),
					native_to_js<V>()(it->second, scope));
		}

		return handle_scope.Close(object);
	}
};

}

}}

#endif /* __NASIU__SCRIPTING__V8W__NATIVE_TO_JS_HPP__ */
