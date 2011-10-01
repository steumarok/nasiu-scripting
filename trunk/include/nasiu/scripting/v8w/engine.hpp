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

#if !defined(__NASIU__SCRIPTING__V8W__ENGINE_HPP__)
#define __NASIU__SCRIPTING__V8W__ENGINE_HPP__

#include <list>
#include <typeinfo>
#include <map>
#include <v8.h>
#include <boost/shared_ptr.hpp>

#include <nasiu/scripting/v8w/engine_base.hpp>
#include <nasiu/scripting/v8w/adapters.hpp>
#include <nasiu/scripting/v8w/script.hpp>
#include <nasiu/scripting/v8w/exceptions.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

std::string
get_stack_trace(
		int frame_limit)
{
	using namespace v8;

	HandleScope handle_scope;

	Handle<StackTrace> stack_trace =
			StackTrace::CurrentStackTrace(frame_limit, StackTrace::kOverview);

	std::ostringstream oss;
	for (int i = 0; i < stack_trace->GetFrameCount(); ++i)
	{
		Handle<StackFrame> frame = stack_trace->GetFrame(i);

		oss << *String::Utf8Value(frame->GetFunctionName())
			<< " [line: " << frame->GetLineNumber()
			<< ", column: " << frame->GetColumn() << "]"
			<< std::endl;
	}
	return oss.str();
}

void
check_exception(
		engine_base& e,
		v8::TryCatch& try_catch)
{
	using namespace v8;

	if (try_catch.HasCaught())
	{
		std::string message;
		invocation_scope scope(e);

		Local<Value> js_exception = try_catch.Exception();
		if (js_exception->IsObject())
		{
			Local<Object> js_object = Local<Object>::Cast(js_exception);

			if (js_object->InternalFieldCount() == 2)
			{
				class_adapter_base* adapter =
						static_cast<class_adapter_base*>(js_object->GetPointerFromInternalField(1));
				adapter->cast_and_throw(js_object->GetPointerFromInternalField(0));
				return;
			}
		}

		js_to_native<std::string>()(message, js_exception, scope);
		throw v8w_exception(
				message + "\nStack trace: " + get_stack_trace(),
				e,
				Persistent<Value>::New(js_exception));
	}
}

class default_interceptor : public interceptor<tags::v8w>
{
public:
	virtual
	v8::Handle<v8::Value>
	on_native_call(
			native_caller_base& caller,
			invocation_scope& scope)
	{
		return caller.make_call(scope);
	}
};

class engine : public engine_base
{
	v8::Persistent<v8::Context> context_;

	typedef std::map<std::string, boost::shared_ptr<adapter_base> > adapter_map_type;
	adapter_map_type adapter_map_;

	interceptor<tags::v8w>* interceptor_;
	std::list<class_adapter_base*> exceptions_;

public:
	engine(interceptor<tags::v8w>* i = 0)
	: interceptor_(i)
	{
		using namespace v8;

		context_ = Context::New();
	}

	v8::Handle<v8::Value>
	catch_and_throw(
			native_caller_base& caller,
			invocation_scope& scope) const
	{
		if (exceptions_.size() > 0)
		{
			std::list<class_adapter_base*>::const_iterator it = exceptions_.begin();
			return (*it)->catch_and_throw(
					caller, scope, ++it, exceptions_.end());
		}
		else
		{
			return caller.make_call(scope);
		}
	}

	interceptor<tags::v8w>&
	get_interceptor() const
	{
		static default_interceptor def_interc;
		if (interceptor_)
		{
			return *interceptor_;
		}
		else
		{
			return def_interc;
		}
	}

	template<typename T>
	void
	set(
			const std::string& name,
			T value)
	{
		v8::HandleScope handle_scope;

		v8::Context::Scope context_scope(context_);

		invocation_scope scope(*this);
		context_->Global()->Set(
				v8::String::New(name.c_str()),
				native_to_js<T>()(value, scope));
	}

	template<typename T>
	T
	get(
			const std::string& name)
	{
		v8::HandleScope handle_scope;

		v8::Context::Scope context_scope(context_);

		invocation_scope scope(*this);
		T result;
		v8::Local<v8::Value> value = context_->Global()->Get(
				v8::String::New(name.c_str()));
		js_to_native<T>()(result, value, scope);
		return result;
	}

	script<tags::v8w>
	compile(
			const std::string& source);

	void
	exec(
			const std::string& source);

	template<typename R>
	R
	eval(
			const std::string& source);

	template<typename C>
	class_adapter_base*
	bind_class();

	template<typename C>
	void
	bind_exception();

	adapter_base*
	get_adapter(
			const std::string& name) const;

	template<typename F>
	void
	bind_function(
			const std::string& name,
			F func);

	v8::Persistent<v8::Context> get_context()
	{
		return context_;
	}

};

script<tags::v8w>
engine::compile(
		const std::string& source)
{
	using namespace v8;

	HandleScope handle_scope;

	Context::Scope context_scope(context_);

	TryCatch try_catch;

	Handle<String> script_source = String::New(source.c_str());
	Handle<Script> js_script = Script::Compile(script_source);

	check_exception(*this, try_catch);

	return script<tags::v8w>(*this, handle_scope.Close(js_script));
}

void
engine::exec(const std::string& source)
{
	using namespace v8;

	HandleScope handle_scope;

	Context::Scope context_scope(context_);

	TryCatch try_catch;

	Handle<String> script_source = String::New(source.c_str());
	Handle<Script> js_script = Script::New(script_source);

	check_exception(*this, try_catch);

	js_script->Run();

	check_exception(*this, try_catch);
}

template<typename R>
R
engine::eval(const std::string& source)
{
	using namespace v8;

	HandleScope handle_scope;

	Context::Scope context_scope(context_);

	TryCatch try_catch;

	Handle<String> script_source = String::New(source.c_str());
	Handle<Script> script = Script::New(script_source);

	check_exception(*this, try_catch);

	Local<Value> script_result = script->Run();

	check_exception(*this, try_catch);

	R result;
	invocation_scope inv_scope(*this);
	js_to_native<R>()(result, script_result, inv_scope);

	return result;
}

template<typename C>
class_adapter_base*
engine::bind_class()
{
	using namespace v8;

	HandleScope handle_scope;

	class_adapter<C>* adapter = new class_adapter<C>(*this);

	adapter_map_.insert(typename adapter_map_type::value_type(
			class_info<C>::get_name(),
			boost::shared_ptr<adapter_base>(adapter)));

	Handle<FunctionTemplate> function_template = adapter->get_function_template();

	Context::Scope context_scope(context_);

	context_->Global()->Set(
			String::New(adapter->get_name()),
			function_template->GetFunction());

	//
	// Set this function template as derived from parent, is specified.
	//
	typedef typename class_info<C>::parent_class parent_class;
	if (!boost::mpl::is_void_<parent_class>::value)
	{
		inherit_setter<parent_class>()(*this, function_template);
	}

	return adapter;
}

template<typename C>
void
engine::bind_exception()
{
	class_adapter_base* adapter = bind_class<C>();

	exceptions_.push_back(adapter);
}

adapter_base*
engine::get_adapter(
		const std::string& name) const
{
	typename adapter_map_type::const_iterator it(adapter_map_.find(name));

	if (it != adapter_map_.end())
	{
		return it->second.get();
	}
	else
	{
		return 0;
	}
}

template<typename F>
void
engine::bind_function(
		const std::string& name,
		F func)
{
	using namespace v8;

	HandleScope handle_scope;

	function_adapter<F>* adapter = new function_adapter<F>(*this, func);

	adapter_map_.insert(typename adapter_map_type::value_type(
			name,
			boost::shared_ptr<adapter_base>(adapter)));

	Handle<FunctionTemplate> function_template = adapter->get_function_template();

	Context::Scope context_scope(context_);

	context_->Global()->Set(
			String::New(name.c_str()),
			function_template->GetFunction());
}

template<typename T>
struct native_to_js<T*>
{
	v8::Handle<v8::Value>
	operator()(
			T* from,
			invocation_scope& scope)
	{
		class_adapter<T>* adapter = dynamic_cast<class_adapter<T>*>(
				scope.get_engine().get_adapter(class_info<T>::get_name()));

		if (!adapter)
		{
			throw native_exception(std::string("Adapter for class ") + class_info<T>::get_name() + " not found");
		}

		return adapter->wrap(from, true);
	}
};

template<typename T>
struct native_to_js<const T*>
{
	v8::Handle<v8::Value>
	operator()(
			const T* from,
			invocation_scope& scope)
	{
		class_adapter<T>* adapter = dynamic_cast<class_adapter<T>*>(
				scope.get_engine().get_adapter(class_info<T>::get_name()));

		if (!adapter)
		{
			throw native_exception(std::string("Adapter for class ") + class_info<T>::get_name() + " not found");
		}

		return adapter->wrap(from, true);
	}
};

template<typename T>
struct native_to_js<T&>
{
	v8::Handle<v8::Value>
	operator()(
			T& from,
			invocation_scope& scope)
	{
		class_adapter<T>* adapter = dynamic_cast<class_adapter<T>*>(
				scope.get_engine().get_adapter(class_info<T>::get_name()));

		if (!adapter)
		{
			throw native_exception(std::string("Adapter for class ") + class_info<T>::get_name() + " not found");
		}

		return adapter->wrap(&from, false);
	}
};

template<typename T>
struct native_to_js<const T&>
{
	v8::Handle<v8::Value>
	operator()(
			const T& from,
			invocation_scope& scope)
	{
		class_adapter<T>* adapter = dynamic_cast<class_adapter<T>*>(
				scope.get_engine().get_adapter(class_info<T>::get_name()));

		if (!adapter)
		{
			throw native_exception(std::string("Adapter for class ") + class_info<T>::get_name() + " not found");
		}

		return adapter->wrap(&from, false);
	}
};

template<>
struct native_to_js<undefined>
{
	v8::Handle<v8::Value>
	operator()(
			undefined,
			invocation_scope&)
	{
		return v8::Undefined();
	}
};

template<>
struct native_to_js<nil>
{
	v8::Handle<v8::Value>
	operator()(
			nil,
			invocation_scope&)
	{
		return v8::Null();
	}
};


}

}}

#endif /* __NASIU__SCRIPTING__V8W__ENGINE_HPP__ */
