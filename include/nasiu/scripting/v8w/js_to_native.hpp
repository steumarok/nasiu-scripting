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

#if !defined(__NASIU__SCRIPTING__V8W__JS_TO_NATIVE_HPP__)
#define __NASIU__SCRIPTING__V8W__JS_TO_NATIVE_HPP__

#include <list>
#include <vector>
#include <map>
#include <boost/date_time.hpp>
#include <v8.h>

#include <nasiu/scripting/v8w/invocation_scope.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

template<typename T>
struct js_to_native
{
	bool
	operator()(
			T& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		return false;
	}
};

template<>
struct js_to_native<std::tm>
{
	bool operator()(
			std::tm& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		v8::Local<v8::Date> date = v8::Local<v8::Date>::Cast(from);

		time_t raw_time = from->NumberValue()/1000;
		to = *localtime(&raw_time);

		return true;
	}
};


template<>
struct js_to_native<short>
{
	bool operator()(
			short& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->Int32Value();

		return true;
	}
};

template<>
struct js_to_native<unsigned short>
{
	bool operator()(
			short& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->Uint32Value();

		return true;
	}
};

template<>
struct js_to_native<int>
{
	bool operator()(
			int& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->IntegerValue();

		return true;
	}
};

template<>
struct js_to_native<unsigned int>
{
	bool operator()(
			unsigned int& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->IntegerValue();

		return true;
	}
};

template<>
struct js_to_native<bool>
{
	bool operator()(
			bool& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->BooleanValue();

		return true;
	}
};

template<>
struct js_to_native<double>
{
	bool
	operator()(
			double& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->NumberValue();

		return true;
	}
};

template<>
struct js_to_native<float>
{
	bool operator()(
			float& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->NumberValue();

		return true;
	}
};

template<>
struct js_to_native<long long>
{
	bool operator()(
			long long& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->IntegerValue();

		return true;
	}
};

template<>
struct js_to_native<unsigned long long>
{
	bool operator()(
			long long& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		to = from->IntegerValue();

		return true;
	}
};

template<>
struct js_to_native<std::string>
{
	bool
	operator()(
			std::string& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		v8::String::Utf8Value utf8_value(from);
		to = std::string(*utf8_value);

		return true;
	}
};


template<typename T>
struct js_to_native<T*>
{
	bool
	operator()(
			T*& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		if (from->IsNull())
		{
			to = 0;
			return true;
		}

		if (!from->IsObject())
		{
			return false;
		}

		v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(from);

		if (object.IsEmpty())
		{
			return false;
		}

		void* ptr = object->GetPointerFromInternalField(0);

		if (!ptr)
		{
			return false;
		}

		to = static_cast<T*>(ptr);

		return true;
	}
};

template<typename C, typename T>
struct js_to_native_seq
{
	bool
	operator()(
			C& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		if (from.IsEmpty())
		{
			return true;
		}

		if (!from->IsArray())
		{
			return false;
		}

		v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(from);
		for (uint32_t i = 0; i < array->Length(); i++)
		{
			v8::Local<v8::Value> js_el = array->Get(i);
			T n_el;
			js_to_native<T>()(n_el, js_el, scope);
			to.push_back(n_el);
		}

		return true;
	}
};


template<>
template<typename X>
struct js_to_native<std::vector<X> > : js_to_native_seq<std::vector<X>, X>
{
};

template<>
template<typename X>
struct js_to_native<std::list<X> > : js_to_native_seq<std::list<X>, X>
{
};

template<>
template<typename K, typename V>
struct js_to_native<std::map<K, V> >
{
	bool
	operator()(
			std::map<K, V>& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		if (from.IsEmpty())
		{
			return true;
		}

		if (!from->IsObject())
		{
			return false;
		}

		v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(from);
		v8::Local<v8::Array> prop_names = obj->GetPropertyNames();

		for (uint32_t i = 0; i < prop_names->Length(); ++i)
		{
			v8::Local<v8::Value> js_name = prop_names->Get(i);
			v8::Local<v8::Value> js_value = obj->Get(js_name);

			K key;
			js_to_native<K>()(key, js_name, scope);

			V value;
			js_to_native<V>()(value, js_value, scope);

			typedef typename std::map<K, V>::value_type value_type;
			to.insert(value_type(key, value));
		}

		return true;
	}
};


}

}}

#endif /* __NASIU__SCRIPTING__V8W__JS_TO_NATIVE_HPP__ */
