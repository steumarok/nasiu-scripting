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

#if !defined(__NASIU__SCRIPTING__V8W__CLASS_TYPE_HPP__)
#define __NASIU__SCRIPTING__V8W__CLASS_TYPE_HPP__

#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/repeat_from_to.hpp>

#include <nasiu/scripting/class_type.hpp>
#include <nasiu/scripting/v8w/tags.hpp>
#include <nasiu/scripting/v8w/js_to_native.hpp>
#include <nasiu/scripting/v8w/native_to_js.hpp>
#include <nasiu/scripting/v8w/invocation_scope.hpp>
#include <nasiu/scripting/v8w/engine_base.hpp>


#define NASIU_V8W_CLASS_TYPE_NEW_INSTANCE_MAX_PARAMS 10

#define NASIU_V8W_CLASS_TYPE_NEW_INSTANCE() 								\
	BOOST_PP_REPEAT_FROM_TO( 												\
			1, NASIU_V8W_CLASS_TYPE_NEW_INSTANCE_MAX_PARAMS,				\
			NASIU_V8W_CLASS_TYPE_NEW_INSTANCE_R, 							\
			~)																\
	/***/

#define NASIU_V8W_FUNCTION_CALL_ARGS(z, i, unused)							\
	v8w::native_to_js<BOOST_PP_CAT(P, i)>()(BOOST_PP_CAT(p, i), scope)		\
	/***/

#define NASIU_V8W_CLASS_TYPE_NEW_INSTANCE_R(z, n, unused)					\
	template<BOOST_PP_ENUM_PARAMS(n, typename P)>							\
	T* new_instance(BOOST_PP_ENUM_BINARY_PARAMS(n, P, p))					\
	{																		\
		using namespace v8;													\
		self_check();														\
		HandleScope handle_scope;											\
		v8w::invocation_scope scope(*engine_);								\
		Handle<Value> args[] =												\
		{																	\
				BOOST_PP_ENUM(n, NASIU_V8W_FUNCTION_CALL_ARGS, ~) 			\
		};																	\
		return new_instance(n, args);										\
	}																		\
	/***/

namespace nasiu { namespace scripting {

template<typename T>
class class_type<tags::v8w, T>
{
	v8w::engine_base* engine_;
	v8::Persistent<v8::Function> function_;

public:
	class_type()
	: engine_(0)
	{
	}

	class_type(
			v8w::engine_base* e,
			v8::Persistent<v8::Function> function)
	: engine_(e),
	  function_(function)
	{
	}

	T*
	new_instance()
	{
		using namespace v8;

		self_check();

		HandleScope handle_scope;

		Handle<Value> args[] = {};
		return new_instance(0, args);
	}

	NASIU_V8W_CLASS_TYPE_NEW_INSTANCE()

protected:
	T*
	new_instance(
			int argc,
			v8::Handle<v8::Value> args[])
	{
		using namespace v8;

		HandleScope handle_scope;

		Context::Scope context_scope(engine_->get_context());

		TryCatch try_catch;

		Local<Object> object = function_->NewInstance(argc, args);

		check_exception(*engine_, try_catch);

		v8w::invocation_scope scope(*engine_);

		T* native_object;
		v8w::js_to_native<T*>()(native_object, object, scope);

		check_exception(*engine_, try_catch);

		return native_object;
	}

	void self_check()
	{
		if (!engine_ || function_.IsEmpty())
		{
			throw native_exception("Class type not valid.");
		}
	}
};

namespace v8w {

template<>
template<typename T>
struct js_to_native<class_type<tags::v8w, T> >
{
	bool
	operator()(
			class_type<tags::v8w, T>& to,
			v8::Local<v8::Value> from,
			invocation_scope& scope)
	{
		using namespace v8;

		if (from.IsEmpty() || !from->IsFunction())
		{
			return false;
		}

		Persistent<Function> persistent_function = Persistent<Function>::New(Handle<Function>::Cast(from));
		to = class_type<tags::v8w, T>(&scope.get_engine(), persistent_function);

		return true;
	}
};

}

}}

#endif /* __NASIU__SCRIPTING__CLASS_TYPE_HPP__ */
