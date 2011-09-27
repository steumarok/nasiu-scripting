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

#if !defined(__NASIU__SCRIPTING__V8W__NATIVE_CALLER_HPP__)
#define __NASIU__SCRIPTING__V8W__NATIVE_CALLER_HPP__

#include <nasiu/scripting/dynamic_caller.hpp>
#include <nasiu/scripting/v8w/native_to_js.hpp>
#include <nasiu/scripting/v8w/invocation_scope.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

class engine;

template<typename F, typename P, typename R>
struct native_caller : dynamic_caller<F, P, R>
{
	v8::Handle<v8::Value>
	operator()(
			engine_base& e,
			F pmf,
			const P& params)
	{
	    try
	    {
	    	invocation_scope scope(e);
	    	return native_to_js<R>()(dynamic_caller<F, P, R>::operator()(pmf, params), scope);
	    }
	    catch (native_exception& e)
	    {
	    	return v8::ThrowException(v8::String::New(e.what()));
	    }
	}
};

template<typename P, typename R>
template<typename T>
struct native_caller<boost::factory<T*>, P, R> : dynamic_caller<boost::factory<T*>, P, R>
{
	v8::Handle<v8::Value>
	operator()(
			engine_base& e,
			boost::factory<T*> pmf,
			const P& params)
	{
	    try
	    {
	    	invocation_scope scope(e);
	    	return native_to_js<R>()(dynamic_caller<boost::factory<T*>, P, R>::operator()(pmf, params), scope);
	    }
	    catch (native_exception& e)
	    {
	    	return v8::ThrowException(v8::String::New(e.what()));
	    }
	}
};

template<typename F, typename P>
struct native_caller<F, P, void> : dynamic_caller<F, P, void>
{
	v8::Handle<v8::Value>
	operator()(
			engine_base&,
			F pmf,
			const P& params)
	{
	    try
	    {
	    	dynamic_caller<F, P, void>::operator()(pmf, params);
			return v8::Undefined();
		}
	    catch (native_exception& e)
	    {
	    	return v8::ThrowException(v8::String::New(e.what()));
	    }
	}
};

}

}}

#endif /* __NASIU__SCRIPTING__V8W__NATIVE_CALLER_HPP__ */
