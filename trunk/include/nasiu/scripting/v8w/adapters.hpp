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

#if !defined(__NASIU__SCRIPTING__V8W__ADAPTERS_HPP__)
#define __NASIU__SCRIPTING__V8W__ADAPTERS_HPP__

#include <list>
#include <v8.h>
#include <nasiu/scripting/v8w/js_caller.hpp>
#include <nasiu/scripting/v8w/adapt_class.hpp>
#include <nasiu/scripting/v8w/invokers.hpp>
#include <nasiu/scripting/v8w/engine_base.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

template<typename T>
class class_adapter;

template<typename T, int I>
struct method_binder
{
	typedef method_binder<T, I+1> next;

	static v8::Handle<v8::Value>
	invocation_callback(
			const v8::Arguments& args)
	{
		using namespace boost;
		using namespace v8;

		class_adapter<T>* adapter = static_cast<class_adapter<T>*>(External::Unwrap(args.Data()));

		Local<Object> self = args.Holder();
	    void* ptr = self->GetPointerFromInternalField(0);
	    T* object = static_cast<T*>(ptr);

	    if (!object)
	    {
	    	return ThrowException(String::New("Pointer is null"));
	    }

	    typedef typename class_method<T, I>::type pmf_type;

   		pmf_type pmf = class_method<T, I>::get_pmf();

	    arguments_wrapper aw(args);

	    method_dispatcher<T>* disp = dynamic_cast<method_dispatcher<T>*>(object);

	    invocation_scope scope(adapter->get_engine());

	    //
	    // If the prototype of the callee is equal to prototype defined by class_adapter,
	    // we are on C++ side.
	    //
   		Handle<FunctionTemplate> ft = adapter->get_function_template();
   		Handle<Function> fn = ft->GetFunction();
   		Local<Value> p = fn->GetPrototype();
   		if (disp && args.Callee()->GetPrototype()->Equals(p))
   		{
   	   		//
   	   		// Retrieve base pmf (defined in dispatcher_t structs).
   	   		//
   		    typedef typename abstract_finder<T, I>::dispatcher dispatcher;
   		    typedef typename dispatcher::pmf_type base_pmf_type;
   		    base_pmf_type base_pmf = dispatcher::get_base_pmf();

   		    if (base_pmf == 0)
   		    {
   		    	std::string message(std::string(class_method<T, I>::get_name()) + " is abstract");
   		    	return ThrowException(String::New(message.c_str()));
   		    }
   		    else
   		    {
   		    	return invoke_member_function(disp, base_pmf, aw, scope);
   		    }
   		}
   		else
   		{
   			return invoke_member_function(object, pmf, aw, scope);
   		}
	}

	void
	operator()(
			class_adapter<T>& adapter,
			v8::Handle<v8::ObjectTemplate> prototype)
	{
		using namespace v8;

		HandleScope handle_scope;

		Handle<FunctionTemplate> function_template =
				FunctionTemplate::New(&invocation_callback, External::Wrap(&adapter));

		prototype->Set(class_method<T, I>::get_name(), function_template);

		next()(adapter, prototype);
	}
};

template<typename T>
struct method_binder<T, int(class_info<T>::method_count::value)>
{
	void operator()(
			class_adapter<T>& adapter,
			v8::Handle<v8::ObjectTemplate>)
	{
	}
};


template<typename T, int I>
struct accessor_binder;

template<typename T, int I, typename S>
struct setter_chooser
{
	static v8::AccessorSetter
	get_setter()
	{
		return accessor_binder<T, I>::setter;
	}

};

template<typename T, int I>
struct setter_chooser<T, I, boost::mpl::false_>
{
	static v8::AccessorSetter
	get_setter()
	{
		return 0;
	}
};

template<typename T, int I>
struct accessor_binder
{
	static T*
	get_object(
			const v8::AccessorInfo& args)
	{
		using namespace v8;

		Local<Object> self = args.Holder();
	    void* ptr = self->GetPointerFromInternalField(0);
	    return static_cast<T*>(ptr);
	}

	static void
	setter(
			v8::Local<v8::String> property,
			v8::Local<v8::Value> value,
			const v8::AccessorInfo& args)
	{
		using namespace v8;

		class_adapter<T>* adapter = static_cast<class_adapter<T>*>(External::Unwrap(args.Data()));

	    typedef typename class_accessor<T, I>::setter_type setter_pmf_type;

		setter_pmf_type pmf = class_accessor<T, I>::get_setter_pmf();

	    invocation_scope scope(adapter->get_engine());

	    single_value_wrapper svw(value);
	    invoke_member_function(get_object(args), pmf, svw, scope);
	}

	static v8::Handle<v8::Value>
	getter(
			v8::Local<v8::String> property,
			const v8::AccessorInfo& args)
	{
		using namespace v8;

		class_adapter<T>* adapter = static_cast<class_adapter<T>*>(External::Unwrap(args.Data()));

		typedef typename class_accessor<T, I>::getter_type getter_pmf_type;

	    getter_pmf_type pmf = class_accessor<T, I>::get_getter_pmf();

	    invocation_scope scope(adapter->get_engine());

	    empty_wrapper ew;
	    return invoke_member_function(get_object(args), pmf, ew, scope);
	}

	typedef accessor_binder<T, I+1> next;

	void operator()(
			class_adapter<T>& adapter,
			v8::Handle<v8::ObjectTemplate> object_template)
	{
		using namespace v8;

		object_template->SetAccessor(
				String::New(class_accessor<T, I>::get_name()),
				getter,
				setter_chooser<T, I, typename class_accessor<T, I>::setter_type>::get_setter(),
				External::Wrap(this));

		next()(adapter, object_template);
	}
};

template<typename T>
struct accessor_binder<T, int(class_info<T>::accessor_count::value)>
{
	void operator()(
			class_adapter<T>& adapter,
			v8::Handle<v8::ObjectTemplate>)
	{
	}
};

template<typename T>
struct inherit_setter
{
	void
	operator()(
			engine_base& e,
			v8::Handle<v8::FunctionTemplate> function_template)
	{
		using namespace v8;

		adapter_base* parent_adapter =
				e.get_adapter(class_info<T>::get_name());

		if (parent_adapter)
		{
			class_adapter<T>* parent_class_adapter =
					dynamic_cast<class_adapter<T>*>(parent_adapter);

			Handle<FunctionTemplate> parent_function_template =
					parent_class_adapter->get_function_template();

			function_template->Inherit(parent_function_template);
		}
	}
};

template<>
struct inherit_setter<boost::mpl::void_>
{
	void
	operator()(
			engine_base&,
			v8::Handle<v8::FunctionTemplate>)
	{
	}
};

template<typename T, bool abstract>
struct thrower
{
	void operator()(const T& object) const
	{
		throw object;
	}
};

template<typename T>
struct thrower<T, true>
{
	void operator()(const T&) const
	{
	}
};

template<typename T, bool abstract>
struct wrapper_thrower
{
	void operator()(const T& e) const
	{
		throw v8w::exception_wrapper<T>(e);
	}
};

template<typename T>
struct wrapper_thrower<T, true>
{
	void operator()(const T&) const
	{
		throw std::exception();
	}
};

template<typename T>
class class_adapter : public class_adapter_base
{
	v8::Persistent<v8::FunctionTemplate> function_template_;

public:
	class_adapter(engine_base& e)
	: class_adapter_base(e)
	{
	}

	const char*
	get_name() const
	{
		return class_info<T>::get_name();
	}

	void
	cast_and_throw(
			void* obj)
	{
		assert(obj != 0);

		T* object = static_cast<T*>(obj);

		thrower<T, boost::is_abstract<T>::value >()(*object);
	}

	v8::Handle<v8::Value>
	catch_and_throw(
			native_caller_base& caller,
			invocation_scope& scope,
			std::list<class_adapter_base*>::const_iterator next,
			std::list<class_adapter_base*>::const_iterator end) const
	{
		try
		{
			if (next == end)
			{
				return caller.make_call(scope);
			}
			else
			{
				return (*next)->catch_and_throw(caller, scope, ++next, end);
			}
		}
		catch (const T& e)
		{
			wrapper_thrower<T, boost::is_abstract<T>::value >()(e);

			// quiet compilers.
			return v8::Undefined();
		}
	}

	v8::Handle<v8::FunctionTemplate>
	get_function_template()
	{
		using namespace v8;

		if (function_template_.IsEmpty())
		{
			function_template_ = Persistent<FunctionTemplate>::New(create_function_template());

			setup_prototype(function_template_);
		}

		return function_template_;
	}

	v8::Handle<v8::Object>
	wrap(
			const T* obj,
			bool weak)
	{
		using namespace v8;

		const method_dispatcher<T>* disp = dynamic_cast<const method_dispatcher<T>*>(obj);

		if (disp)
		{
			return disp->get_object();
		}
		else
		{
			Handle<FunctionTemplate> function_template = get_function_template();
			Handle<Function> function = function_template->GetFunction();
			Handle<Value> param = External::New((void*)obj);
			Handle<Object> object = function->NewInstance(1, &param);

			Persistent<Object> persistent_object = Persistent<Object>::New(object);

			//persistent_object->SetPointerInInternalField(0, obj);

			if (weak)
			{
				persistent_object.MakeWeak((void*)obj, weak_callback);
			}

			return persistent_object;
		}
	}

	/*static class_adapter&
	instance()
	{
		static class_adapter s_instance;
		return s_instance;
	}*/

private:
	static void
	weak_callback(
			v8::Persistent<v8::Value> pv,
			void*)
	{
		using namespace v8;

		Persistent<Object> pobj = Persistent<Object>::Cast(pv);

	    void* ptr = pobj->GetPointerFromInternalField(0);
	    T* object = static_cast<T*>(ptr);

		delete object;

		pv.Dispose();
	}

	static v8::Handle<v8::Value>
	constructor_callback(
			const v8::Arguments& args)
	{
		using namespace v8;

		class_adapter* adapter = static_cast<class_adapter*>(External::Unwrap(args.Data()));

		if (args.Length() == 1 && args[0]->IsExternal())
		{
			args.This()->SetPointerInInternalField(0, External::Unwrap(args[0]));
			args.This()->SetPointerInInternalField(1, adapter);
			return args.This();
		}
		else
		{
			Persistent<Object> object = Persistent<Object>::New(args.This());

			args.This()->SetPointerInInternalField(1, adapter);
			object.MakeWeak(0, weak_callback);

			invocation_scope scope(adapter->get_engine());

			arguments_wrapper aw(args);
			return invoke_constructor<
						method_dispatcher<T>,
						typename class_info<T>::ctor_sig,
						T>(object, aw, scope);
		}
	}


protected:
	v8::Handle<v8::FunctionTemplate>
	create_function_template()
	{
		using namespace v8;

		Handle<FunctionTemplate> function_template =
				FunctionTemplate::New(&constructor_callback, External::Wrap((void*)this));
		function_template->SetClassName(String::New(class_info<T>::get_name()));

		Handle<ObjectTemplate> instance_template = function_template->InstanceTemplate();
		instance_template->SetInternalFieldCount(2);

		accessor_binder<T, 0>()(*this, instance_template);

		return function_template;
	}

	void
	setup_prototype(
			v8::Handle<v8::FunctionTemplate> function_template)
	{
		using namespace v8;

		Handle<ObjectTemplate> prototype_template = function_template->PrototypeTemplate();

		method_binder<T, 0>()(*this, prototype_template);
	}
};

template<typename F>
class function_adapter : public adapter_base
{
	mutable v8::Persistent<v8::FunctionTemplate> function_template_;
	F function_;

public:
	function_adapter(engine_base& e, F function)
	: adapter_base(e),
	  function_(function)
	{
	}

	v8::Handle<v8::FunctionTemplate>
	get_function_template()
	{
		using namespace v8;

		if (function_template_.IsEmpty())
		{
			function_template_ =
					Persistent<FunctionTemplate>::New(create_function_template());
		}

		return function_template_;
	}

protected:
	v8::Handle<v8::FunctionTemplate>
	create_function_template()
	{
		using namespace v8;

		return FunctionTemplate::New(
				&invocation_callback,
				External::Wrap(this));
	}

private:
	static v8::Handle<v8::Value>
	invocation_callback(
			const v8::Arguments& args)
	{
		using namespace v8;

		function_adapter* adapter =
				static_cast<function_adapter*>(External::Unwrap(args.Data()));

		invocation_scope scope(adapter->get_engine());

		arguments_wrapper aw(args);
		return invoke_function(
				adapter->function_,
				aw,
				scope);
	}
};



}

}}

#endif /* __NASIU__SCRIPTING__V8W__ADAPTERS_HPP__ */
