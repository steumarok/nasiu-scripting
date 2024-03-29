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

#if !defined(__NASIU__SCRIPTING__V8W__ENGINE_BASE_HPP__)
#define __NASIU__SCRIPTING__V8W__ENGINE_BASE_HPP__

#include <nasiu/scripting/v8w/tags.hpp>
#include <string>
#include <v8.h>

namespace nasiu { namespace scripting {

namespace v8w {

class adapter_base;

class invocation_scope;

struct native_caller_base
{
	virtual
	v8::Handle<v8::Value>
	make_call(
			invocation_scope& scope) = 0;
};

class engine_base
{
public:
	virtual
	v8::Persistent<v8::Context>
	get_context() = 0;

	virtual
	adapter_base*
	get_adapter(
			const std::string& name) const = 0;

	virtual
	adapter_base*
	get_class_adapter(
			const std::string& fqn) const = 0;

	virtual
	interceptor<tags::v8w>&
	get_interceptor() const = 0;

	virtual v8::Handle<v8::Value>
	catch_and_throw(
			native_caller_base& caller,
			invocation_scope& scope) const = 0;
};

class adapter_base
{
	engine_base& engine_;

public:
	adapter_base(engine_base& e)
	: engine_(e)
	{
	}

	virtual
	~adapter_base() {}

	engine_base&
	get_engine() const
	{
		return engine_;
	}
};

class class_adapter_base : public adapter_base
{
public:
	class_adapter_base(engine_base& e)
	: adapter_base(e)
	{
	}

	virtual void
	cast_and_throw(
			void* obj) = 0;

	virtual v8::Handle<v8::Value>
	catch_and_throw(
			native_caller_base& caller,
			invocation_scope& scope,
			std::list<class_adapter_base*>::const_iterator curr,
			std::list<class_adapter_base*>::const_iterator end) const = 0;
};

void
check_exception(
		engine_base& e,
		v8::TryCatch& try_catch);

std::string
get_stack_trace(
		int frame_limit = 10);

}

template<>
class interceptor<tags::v8w>
{
public:
	virtual
	v8::Handle<v8::Value>
	on_native_call(
			v8w::native_caller_base& caller,
			v8w::invocation_scope& scope) = 0;
};

}}

#endif /* __NASIU__SCRIPTING__V8W__ENGINE_HPP__ */
