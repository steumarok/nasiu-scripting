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

#if !defined(__NASIU__SCRIPTING__V8W__JS_CALLER_HPP__)
#define __NASIU__SCRIPTING__V8W__JS_CALLER_HPP__

#include <nasiu/scripting/v8w/js_to_native.hpp>
#include <nasiu/scripting/v8w/engine_base.hpp>
#include <nasiu/scripting/exceptions.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

template<typename R>
struct js_caller
{
	R
	operator()(
			engine_base& e,
			v8::Local<v8::Function> function,
			v8::Local<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		invocation_scope scope(e);
		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		Local<Value> js_result = function->Call(recv, argc, argv);

		check_exception(e, try_catch);

		R result;
    	js_to_native<R>()(result, js_result, scope);

		check_exception(e, try_catch);

		return result;
	}

	R
	operator()(
			engine_base& e,
			v8::Local<v8::Function> function,
			v8::Persistent<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		invocation_scope scope(e);
		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		Local<Value> js_result = function->Call(recv, argc, argv);

		check_exception(e, try_catch);

		R result;
		js_to_native<R>()(result, js_result, scope);

		return result;
	}

	R
	operator()(
			engine_base& e,
			v8::Persistent<v8::Function> function,
			v8::Local<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		invocation_scope scope(e);
		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		Local<Value> js_result = function->Call(recv, argc, argv);

		check_exception(e, try_catch);

		R result;
    	js_to_native<R>()(result, js_result, scope);

		check_exception(e, try_catch);

		return result;
	}

	R
	operator()(
			engine_base& e,
			v8::Persistent<v8::Function> function,
			v8::Persistent<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		invocation_scope scope(e);
		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		Local<Value> js_result = function->Call(recv, argc, argv);

		check_exception(e, try_catch);

		R result;
    	js_to_native<R>()(result, js_result, scope);

		check_exception(e, try_catch);

		return result;
	}
};

template<>
struct js_caller<void>
{
	void
	operator()(
			engine_base& e,
			v8::Local<v8::Function> function,
			v8::Local<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		function->Call(recv, argc, argv);

		check_exception(e, try_catch);
	}

	void
	operator()(
			engine_base& e,
			v8::Local<v8::Function> function,
			v8::Persistent<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		function->Call(recv, argc, argv);

		check_exception(e, try_catch);
	}

	void
	operator()(
			engine_base& e,
			v8::Persistent<v8::Function> function,
			v8::Local<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		function->Call(recv, argc, argv);

		check_exception(e, try_catch);
	}

	void
	operator()(
			engine_base& e,
			v8::Persistent<v8::Function> function,
			v8::Persistent<v8::Object> recv,
			int argc,
			v8::Handle<v8::Value> argv[])
	{
		using namespace v8;

		Context::Scope context_scope(e.get_context());
		TryCatch try_catch;

		function->Call(recv, argc, argv);

		check_exception(e, try_catch);
	}
};

}

}}

#endif /* __NASIU__SCRIPTING__V8W__JS_CALLER_HPP__ */
