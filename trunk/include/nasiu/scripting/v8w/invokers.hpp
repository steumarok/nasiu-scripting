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

#if !defined(__NASIU__SCRIPTING__V8W__INVOKERS_HPP__)
#define __NASIU__SCRIPTING__V8W__INVOKERS_HPP__

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/replace.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/clear.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/single_view.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/container/vector.hpp>
#include <v8.h>
#include <assert.h>

#include <nasiu/scripting/exceptions.hpp>
#include <nasiu/scripting/v8w/native_caller.hpp>
#include <nasiu/scripting/v8w/invocation_scope.hpp>
#include <nasiu/scripting/v8w/js_to_native.hpp>

namespace nasiu { namespace scripting {

namespace v8w {

class engine;

class values_wrapper
{
public:
	virtual int
	length() const = 0;

	virtual v8::Local<v8::Value>
	operator[](
			int i) const = 0;
};

class arguments_wrapper : public values_wrapper
{
	const v8::Arguments& args_;

public:
	arguments_wrapper(
			const v8::Arguments& args)
	: args_(args)
	{
	}

	int
	length() const
	{
		return args_.Length();
	}

	v8::Local<v8::Value>
	operator[](
			int i) const
	{
		return args_[i];
	}
};

class single_value_wrapper : public values_wrapper
{
	v8::Local<v8::Value> value_;

public:
	single_value_wrapper(
			v8::Local<v8::Value> )
	: value_(value_)
	{
	}

	int
	length() const
	{
		return 1;
	}

	v8::Local<v8::Value>
	operator[](
			int i) const
	{
		return value_;
	}
};

class empty_wrapper : public values_wrapper
{
public:
	empty_wrapper()
	{
	}

	int
	length() const
	{
		return 0;
	}

	v8::Local<v8::Value>
	operator[](
			int i) const
	{
		assert(false);
	}
};




struct function_param_setter
{
	const values_wrapper& values_;
	mutable int index_;
	mutable int pass_;
	int skip_;
	mutable invocation_scope& scope_;

	function_param_setter(
			const values_wrapper& values,
			int skip,
			invocation_scope& scope)
	: values_(values),
	  index_(0),
	  pass_(0),
	  skip_(skip),
	  scope_(scope)
	{
	}

	template<typename T>
	void
	operator()(
			T& val) const
	{
		if (pass_ < skip_)
		{
			++pass_;
		}
		else
		{
			if (index_ < values_.length())
			{
				if (!values_[index_].IsEmpty())
				{
					if (!js_to_native<T>()(val, values_[index_], scope_))
					{
						throw native_exception("Bad type of parameter");
					}
				}

				++index_;
			}
		}
	}
};


template<typename T, typename F>
v8::Handle<v8::Value>
invoke_member_function(
		T* object,
		F func,
		const values_wrapper& values,
		invocation_scope& scope)
{
	using namespace boost;
	using namespace v8;

    //
    // Decompose pmf signature.
    //
    typedef typename function_types::parameter_types<F>::type param_types;
    typedef typename function_types::result_type<F>::type result_type;

	//
	// Make a args-only types view (without the class specifier).
	//
    typedef mpl::iterator_range<
    		typename mpl::next<
    			typename mpl::begin<param_types>::type
    		>::type,
    		typename mpl::end<param_types>::type
    	> args_types;

    //
    // Remove const and reference indicators (JS is a param-by-value lang).
    //
    typedef mpl::transform_view<
    		args_types,
    		boost::remove_const<boost::remove_reference<mpl::_1> >
    	> valuized_args_types;

    //
    // Make the correct call param types.
    //
    typedef typename mpl::joint_view<
    		mpl::single_view<T*>,
    		valuized_args_types
    	>::type new_param_types;

    //
    // Make the fusion vector type for hold param values.
    //
    typedef typename fusion::result_of::as_vector<
    		new_param_types
    	>::type param_vector_type;


    //
    // Fill param vector.
    //
    param_vector_type params;

    boost::fusion::at_c<0>(params) = object;

    try
    {
    	function_param_setter s(values, 1, scope);
    	fusion::for_each(params, s);
    }
    catch (const native_exception& e)
    {
    	return ThrowException(String::New(e.what()));
    }

    //
    // And finally call the function.
    //
	return native_caller<F, param_vector_type, result_type>()(scope.get_engine(), func, params);
}

template<typename F>
v8::Handle<v8::Value>
invoke_function(
		F func,
		const values_wrapper& values,
		invocation_scope& scope)
{
	using namespace boost;
	using namespace v8;

    typedef typename function_types::parameter_types<F>::type param_types;
    typedef typename function_types::result_type<F>::type result_type;

    typedef mpl::transform_view<
    		param_types,
    		boost::remove_const<boost::remove_reference<mpl::_1> >
    	> valuized_args_types;

    typedef typename fusion::result_of::as_vector<
    		valuized_args_types
    	>::type param_vector_type;

    param_vector_type params;

    try
    {
    	function_param_setter s(values, 0, scope);
    	fusion::for_each(params, s);
	}
	catch (const native_exception& e)
	{
		return ThrowException(String::New(e.what()));
	}

	return native_caller<F, param_vector_type, result_type>()(scope.get_engine(), func, params);
}

template<typename T, typename F, typename R>
v8::Handle<v8::Value>
invoke_constructor(
		v8::Persistent<v8::Object> object,
		const values_wrapper& values,
		invocation_scope& scope)
{
	using namespace boost;
	using namespace v8;

    typedef typename function_types::parameter_types<F>::type param_types;
    typedef typename function_types::result_type<F>::type result_type;

    typedef mpl::transform_view<
    		param_types,
    		boost::remove_const<boost::remove_reference<mpl::_1> >
    	> valuized_args_types;

    typedef typename fusion::result_of::as_vector<
    		valuized_args_types
    	>::type param_vector_type;

    param_vector_type params;

    boost::fusion::at_c<0>(params) = &scope.get_engine();
    boost::fusion::at_c<1>(params) = object;

    try
    {
    	function_param_setter s(values, 2, scope);
    	fusion::for_each(params, s);
	}
	catch (const native_exception& e)
	{

		return ThrowException(String::New(e.what()));
	}

	R* native_object = dynamic_caller<factory<T*>, param_vector_type, R*>()(boost::factory<T*>(), params);

	object->SetPointerInInternalField(0, native_object);

	return object;
}

}

}}

#endif /* __NASIU__SCRIPTING__V8W__INVOKERS_HPP__ */
