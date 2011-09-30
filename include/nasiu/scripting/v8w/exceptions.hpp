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

#if !defined(__NASIU__SCRIPTING__V8W__EXCEPTIONS_HPP__)
#define __NASIU__SCRIPTING__V8W__EXCEPTIONS_HPP__

#include <v8.h>
#include <nasiu/scripting/v8w/engine_base.hpp>
#include <nasiu/scripting/v8w/native_to_js.hpp>
#include <nasiu/scripting/exceptions.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

class exception_wrapper_base
{
public:
	virtual
	v8::Handle<v8::Value>
	to_js(
			engine_base& e) const = 0;
};

template<class T>
class exception_wrapper : public exception_wrapper_base
{
	T* value_;

public:
	exception_wrapper(const T& value)
	: value_(new T(value))
	{
	}

	v8::Handle<v8::Value>
	to_js(
			engine_base& e) const
	{
		invocation_scope scope(e);
		return v8::ThrowException(native_to_js<const T*>()(value_, scope));
	}
};

class v8w_exception : public script_exception
{
	engine_base* engine_;
	v8::Persistent<v8::Value> value_;

public:
	v8w_exception()
	{
	}

	v8w_exception(
			const std::string& msg,
			engine_base& e,
			v8::Persistent<v8::Value> value)
	: script_exception(msg),
	  engine_(&e),
	  value_(value)
	{
	}

	virtual
	~v8w_exception() throw()
	{
	}

	template<class T>
	T cast() const
	{
		using namespace v8;

		invocation_scope scope(*engine_);
		Context::Scope context_scope(engine_->get_context());

		T result;
    	js_to_native<T>()(result, value_, scope);

		return result;
	}
};

}

}}

#endif /* __NASIU__SCRIPTING__V8W__EXCEPTIONS_HPP__ */
