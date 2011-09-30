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

#if !defined(__NASIU__SCRIPTING__V8W__FUNCTION_HPP__)
#define __NASIU__SCRIPTING__V8W__FUNCTION_HPP__

#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/comma_if.hpp>

#include <nasiu/scripting/function.hpp>
#include <nasiu/scripting/v8w/tags.hpp>
#include <nasiu/scripting/v8w/js_caller.hpp>
#include <nasiu/scripting/v8w/js_to_native.hpp>
#include <nasiu/scripting/v8w/native_to_js.hpp>
#include <nasiu/scripting/v8w/invocation_scope.hpp>

#define NASIU_V8W_FUNCTION_CALL_MAX_PARAMS 10

#define NASIU_V8W_FUNCTION_CALL() \
	BOOST_PP_REPEAT( 														\
			NASIU_V8W_FUNCTION_CALL_MAX_PARAMS,								\
			NASIU_V8W_FUNCTION_CALL_R, 										\
			~)																\
	/***/

#define NASIU_V8W_FUNCTION_CALL_ARGS(z, i, unused)							\
	v8w::native_to_js<BOOST_PP_CAT(P, i)>()(BOOST_PP_CAT(p, i), scope)		\
	/***/

#define NASIU_V8W_FUNCTION_CALL_R(z, n, unused)								\
	template<typename R BOOST_PP_COMMA_IF(n) 								\
		BOOST_PP_ENUM_PARAMS(n, typename P)>								\
	R call(BOOST_PP_ENUM_BINARY_PARAMS(n, P, p))							\
	{																		\
		using namespace v8;													\
		HandleScope handle_scope;											\
		v8w::invocation_scope scope(*engine_);								\
		Handle<Value> args[] =												\
		{																	\
				BOOST_PP_ENUM(n, NASIU_V8W_FUNCTION_CALL_ARGS, ~) 			\
		};																	\
		return v8w::js_caller<R>()(*engine_, function_, function_, n, args);\
	}																		\
	/***/


namespace nasiu { namespace scripting {

template<>
class function<tags::v8w>
{
	v8w::engine_base* engine_;
	v8::Persistent<v8::Function> function_;

public:
	function()
	{
	}

	function(
			v8w::engine_base* e,
			v8::Persistent<v8::Function> function)
	: engine_(e), function_(function)
	{
	}

	v8::Persistent<v8::Function> get_function()
	{
		return function_;
	}

	NASIU_V8W_FUNCTION_CALL()
};

namespace v8w {

template<>
struct js_to_native<function<tags::v8w> >
{
	bool
	operator()(
			function<tags::v8w>& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		using namespace v8;

		Persistent<Function> persistent_function = Persistent<Function>::New(Handle<Function>::Cast(from));
		to = function<tags::v8w>(&scope.get_engine(), persistent_function);

		return true;
	}
};

template<>
struct native_to_js<function<tags::v8w> >
{
	v8::Handle<v8::Value>
	operator()(
			function<tags::v8w>& from,
			invocation_scope& scope)
	{
		using namespace v8;

		return from.get_function();
	}
};


}

}}

#endif /* __NASIU__SCRIPTING__CLASS_TYPE_HPP__ */
