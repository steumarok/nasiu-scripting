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

#if !defined(__NASIU__SCRIPTING__V8W__ADAPT_CLASS_HPP__)
#define __NASIU__SCRIPTING__V8W__ADAPT_CLASS_HPP__

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/facilities/is_empty.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/slot/counter.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/if.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits/is_abstract.hpp>
#include <boost/config/no_tr1/utility.hpp>

#include <nasiu/scripting/v8w/invocation_scope.hpp>


namespace nasiu { namespace scripting {

namespace v8w {

struct method_dispatcher_base
{
	virtual ~method_dispatcher_base() throw() {}
};

template <typename Class>
struct class_info;

template <typename Class>
struct method_dispatcher;

template <typename Class, int N>
struct dispatchers;

template<typename Class, int N>
struct abstract_finder;

template<int Z, typename Class, int N, typename R>
struct dispatcher_abstract;
template<int Z, typename Class, int N, typename R>
struct dispatcher_concrete;

template <typename Class, int N>
struct class_method;

template <typename Class, int N>
struct class_accessor;

template <typename Class, int N>
struct class_method<Class const, N>
{
    typedef typename class_method<Class, N>::type type;

    static type get_pmf()
    {
        return class_method<Class, N>::get_pmf();
    }
};

}

}}

#define NASIU_V8W_MAX_CALL_ADAPTER_CNT 10

#define NASIU_V8W_PARAM_LIST(z, i, _) BOOST_PP_COMMA_IF(i)					\
	BOOST_PP_CAT(param_type, i) BOOST_PP_CAT(p, i)							\
	/***/

#define NASIU_V8W_PARAM_USE(z, i, _) BOOST_PP_COMMA_IF(i)					\
	BOOST_PP_CAT(p, i)														\
	/***/

#define NASIU_V8W_PARAM_TO_JS(z, i, _) BOOST_PP_COMMA_IF(i)					\
	native_to_js<BOOST_PP_CAT(param_type, i)>()(BOOST_PP_CAT(p, i), scope)	\
	/***/

#define NASIU_V8W_PARAM_TYPE_DECL(z, i, _) \
	typedef typename boost::mpl::next<BOOST_PP_CAT(it, i)>::type 			\
			BOOST_PP_CAT(it, BOOST_PP_INC(i)); 							    \
	typedef typename boost::mpl::deref<										\
				BOOST_PP_CAT(it, BOOST_PP_INC(i))							\
			>::type BOOST_PP_CAT(param_type, i); 							\
	/***/

#define NASIU_V8W_CTOR_PARAM_SIG_DECL(r, _, i, type) , type

#define NASIU_V8W_CTOR_PARAM_USE(r, i, _)									\
	BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(p, i)									\
	/***/

#define NASIU_V8W_CTOR_PARAM_TYPE_DECL(r, _, i, type)						\
	typedef type BOOST_PP_CAT(ctor_param_type, i);							\
	/***/

#define NASIU_V8W_CTOR_PARAM_DECL(z, i, cl_name)							\
	BOOST_PP_COMMA_IF(i) 													\
	typename class_info< cl_name >::BOOST_PP_CAT(ctor_param_type, i) 		\
		BOOST_PP_CAT(p, i)													\
	/***/

#define NASIU_V8W_CALL_ADAPTER(base, fn, cseq_size, i) 						\
	/*namespace BOOST_PP_CAT(*/													\
		/*BOOST_PP_CAT(adapter_, base), BOOST_PP_CAT(_, i)) {*/ 				\
		/*typedef base base_type; 								*/			\
		/*typedef BOOST_TYPEOF(&base_type::fn) fn_type;		*/				\
		/*typedef typename boost::function_types::parameter_types<*/			\
		/*	fn_type>::type param_types; 						*/			\
		/*typedef boost::mpl::size<param_types> param_size; 	*/				\
		/*typedef typename boost::function_types::result_type<	*/			\
		/*	fn_type>::type rtype; 								*/			\
		BOOST_PP_REPEAT(													\
			NASIU_V8W_MAX_CALL_ADAPTER_CNT, 								\
			NASIU_V8W_DEF_CALL_ADAPTER_T, 									\
			(fn, cseq_size, i, base)) 												\
		/*typedef dispatcher_a_t<param_size::value, base_type, i> dispatcher_a;*/  \
		/*typedef dispatcher_b_t<param_size::value, base_type, i> dispatcher_b;*/  \
	/*}*/ 																		\
	/***/


#define NASIU_V8W_DEF_CALL_ADAPTER_T(z, i, data /*(fn,cseq-size,method-idx,cl-name)*/) 		\
	template<typename R> 												\
	struct dispatcher_concrete< BOOST_PP_INC(i), BOOST_PP_TUPLE_ELEM(4, 3, data), BOOST_PP_TUPLE_ELEM(4, 2, data), R > 							\
		: virtual public BOOST_PP_TUPLE_ELEM(4, 3, data) 												\
	{																		\
		typedef BOOST_PP_TUPLE_ELEM(4, 3, data) base_type;\
		typedef typename class_method<base_type, BOOST_PP_TUPLE_ELEM(4, 2, data)>::rtype rtype; \
		typedef typename class_method<base_type, BOOST_PP_TUPLE_ELEM(4, 2, data)>::param_types param_types; \
		\
		dispatcher_concrete(BOOST_PP_REPEAT_ ## z(  								\
				BOOST_PP_TUPLE_ELEM(4, 1, data), 							\
				NASIU_V8W_CTOR_PARAM_DECL, 									\
				base_type)) 														\
		: base_type(BOOST_PP_REPEAT_ ## z(BOOST_PP_TUPLE_ELEM(4, 1, data),		\
				NASIU_V8W_CTOR_PARAM_USE, ~)) {} 							\
																			\
		typedef typename boost::mpl::begin<param_types>::type it0; 			\
		BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_TYPE_DECL, ~) 				\
																			\
		rtype BOOST_PP_TUPLE_ELEM(4, 0, data)(								\
				BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_LIST, ~)) 			\
		{ 																	\
			using namespace v8; 											\
			HandleScope handle_scope; 										\
			Persistent<Object> object = get_object(); 						\
			invocation_scope scope(get_engine()); 							\
			Local<Object> prototype = object->GetPrototype().As<Object>(); 	\
			Local<Function> function = 										\
					prototype->Get(String::New(								\
							BOOST_STRINGIZE(								\
									BOOST_PP_TUPLE_ELEM(4, 0, data))))		\
					.As<Function>(); 										\
			Handle<Value> args[] = {										\
					BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_TO_JS, ~) };	\
			return js_caller<rtype>()(										\
					get_engine(), 											\
					function, 												\
					object, 												\
					sizeof(args) / sizeof(Handle<Value>), 					\
					args);													\
		} 																	\
																			\
		rtype BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(4, 0, data),_base)			\
			(BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_LIST, _)) 			\
		{ 																	\
			return base_type::BOOST_PP_TUPLE_ELEM(4, 0, data)					\
				(BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_USE, ~)); 		\
		} 																	\
																			\
		typedef BOOST_TYPEOF(&dispatcher_concrete::BOOST_PP_TUPLE_ELEM(4, 0, data)) pmf_type; 				\
		static pmf_type get_pmf()											\
		{ 																	\
			return &dispatcher_concrete::BOOST_PP_TUPLE_ELEM(4, 0, data); 						\
		}												 					\
		static pmf_type get_base_pmf()										\
		{ 																	\
			return &dispatcher_concrete::BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(4, 0, data),_base); 	\
		} 																	\
																			\
		virtual v8::Persistent<v8::Object> get_object() const = 0;			\
		virtual engine_base& get_engine() const = 0;						\
	}; 																		\
	template<typename R> 												\
	struct dispatcher_abstract< BOOST_PP_INC(i), BOOST_PP_TUPLE_ELEM(4, 3, data), BOOST_PP_TUPLE_ELEM(4, 2, data), R > 	\
		: virtual public BOOST_PP_TUPLE_ELEM(4, 3, data) 												\
	{																		\
		typedef BOOST_PP_TUPLE_ELEM(4, 3, data) base_type;\
		typedef typename class_method<base_type, BOOST_PP_TUPLE_ELEM(4, 2, data)>::rtype rtype; \
		typedef typename class_method<base_type, BOOST_PP_TUPLE_ELEM(4, 2, data)>::param_types param_types; \
		\
		dispatcher_abstract(BOOST_PP_REPEAT_ ## z(  								\
				BOOST_PP_TUPLE_ELEM(4, 1, data), 							\
				NASIU_V8W_CTOR_PARAM_DECL, 									\
				base_type)) 														\
		: base_type(BOOST_PP_REPEAT_ ## z(BOOST_PP_TUPLE_ELEM(4, 1, data),		\
				NASIU_V8W_CTOR_PARAM_USE, ~)) {} 							\
																			\
		typedef typename boost::mpl::begin<param_types>::type it0; 			\
		BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_TYPE_DECL, ~) 				\
																			\
		rtype BOOST_PP_TUPLE_ELEM(4, 0, data)(								\
				BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_LIST, ~)) 			\
		{ 																	\
			using namespace v8; 											\
			HandleScope handle_scope; 										\
			Persistent<Object> object = get_object(); 						\
			invocation_scope scope(get_engine()); 							\
			Local<Object> prototype = object->GetPrototype().As<Object>(); 	\
			Local<Function> function = 										\
					prototype->Get(String::New(								\
							BOOST_STRINGIZE(								\
									BOOST_PP_TUPLE_ELEM(4, 0, data))))		\
					.As<Function>(); 										\
			Handle<Value> args[] = {										\
					BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_TO_JS, ~) };	\
			return js_caller<rtype>()(										\
					get_engine(), 											\
					function, 												\
					object, 												\
					sizeof(args) / sizeof(Handle<Value>), 					\
					args);													\
		} 																	\
																			\
		rtype BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(4, 0, data),_base)			\
			(BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_LIST, _)) 			\
		{ 																	\
			return base_type::BOOST_PP_TUPLE_ELEM(4, 0, data)					\
				(BOOST_PP_REPEAT_ ## z(i, NASIU_V8W_PARAM_USE, ~)); 		\
		} 																	\
																			\
		typedef BOOST_TYPEOF(&dispatcher_abstract::BOOST_PP_TUPLE_ELEM(4, 0, data)) pmf_type; 				\
		static pmf_type get_pmf()											\
		{ 																	\
			return &dispatcher_abstract::BOOST_PP_TUPLE_ELEM(4, 0, data); 						\
		}												 					\
		static pmf_type get_base_pmf()										\
		{ 																	\
			return 0;														\
		} 																	\
																			\
		virtual v8::Persistent<v8::Object> get_object() const = 0;			\
		virtual engine_base& get_engine() const = 0;						\
	}; 																		\
	/***/


#define NASIU_V8W_ADAPT_CLASS(cl_name, parent, cseq, mseq, aseq)			\
    NASIU_V8W_ADAPT_CLASS_I(												\
    	cl_name,                    										\
        parent, 															\
        cseq,   															\
		BOOST_PP_CAT(NASIU_V8W_ADAPT_CLASS_M_X mseq, 0),					\
		BOOST_PP_CAT(NASIU_V8W_ADAPT_CLASS_A_X aseq, 0))					\
    /***/

#define NASIU_V8W_ADAPT_CLASS_M_X(x) ((x)) NASIU_V8W_ADAPT_CLASS_M_Y
#define NASIU_V8W_ADAPT_CLASS_M_Y(y) ((y)) NASIU_V8W_ADAPT_CLASS_M_X
#define NASIU_V8W_ADAPT_CLASS_M_X0
#define NASIU_V8W_ADAPT_CLASS_M_Y0

#define NASIU_V8W_ADAPT_CLASS_A_X(x,y,z) ((x,y,z)) NASIU_V8W_ADAPT_CLASS_A_Y
#define NASIU_V8W_ADAPT_CLASS_A_Y(x,y,z) ((x,y,z)) NASIU_V8W_ADAPT_CLASS_A_X
#define NASIU_V8W_ADAPT_CLASS_A_X0
#define NASIU_V8W_ADAPT_CLASS_A_Y0

// NASIU_V8W_ADAPT_CLASS_I generates the overarching structure and uses
// SEQ_FOR_EACH_I to generate the "linear" substructures.

#define NASIU_V8W_ADAPT_CLASS_I(cl_name, parent, cseq, mseq, aseq)			\
    namespace nasiu { namespace scripting { namespace v8w {					\
        template <>															\
        struct class_info<cl_name> {										\
        	typedef boost::mpl::int_<BOOST_PP_SEQ_SIZE(mseq)>				\
					method_count;											\
        	typedef boost::mpl::int_<BOOST_PP_SEQ_SIZE(aseq)> 				\
        			accessor_count;											\
    		BOOST_PP_SEQ_FOR_EACH_I(										\
    				NASIU_V8W_CTOR_PARAM_TYPE_DECL, ~, cseq); 				\
        	typedef cl_name (*ctor_sig) 									\
        			(engine_base* e, v8::Persistent<v8::Object> 			\
    				 BOOST_PP_SEQ_FOR_EACH_I(								\
    						 NASIU_V8W_CTOR_PARAM_SIG_DECL, ~, cseq));		\
        	static const char* get_name() {									\
    				return BOOST_STRINGIZE(cl_name); }						\
    		typedef BOOST_PP_IF(											\
    			BOOST_PP_IS_EMPTY(parent),									\
    			boost::mpl::void_,											\
    			parent) parent_class; 										\
		};																	\
        BOOST_PP_SEQ_FOR_EACH_I(NASIU_V8W_ADAPT_CLASS_M,					\
        		(cl_name, BOOST_PP_SEQ_SIZE(cseq)), mseq)					\
        BOOST_PP_SEQ_FOR_EACH_I(NASIU_V8W_ADAPT_CLASS_A,					\
        		cl_name, aseq)												\
        BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(mseq),							\
        		NASIU_V8W_FIND_ABSTRACT,									\
        		(cl_name, BOOST_PP_SEQ_SIZE(mseq))) 						\
		template <>															\
		class method_dispatcher<cl_name>									\
			: public method_dispatcher_base									\
			BOOST_PP_SEQ_FOR_EACH_I(										\
					NASIU_V8W_METHOD_DISPATCHER_INHERITANCE, 				\
					cl_name, mseq)											\
		{   																\
			engine_base& engine_;											\
			v8::Persistent<v8::Object> object_; 							\
																			\
		public:  															\
			typedef cl_name class_type;																\
			method_dispatcher(engine_base* e,								\
					v8::Persistent<v8::Object> object						\
					BOOST_PP_COMMA_IF(BOOST_PP_SEQ_SIZE(cseq)) 				\
					BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(cseq),				\
							NASIU_V8W_CTOR_PARAM_DECL, cl_name)) 			\
					: cl_name(BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(cseq),		\
							NASIU_V8W_CTOR_PARAM_USE, ~)) 					\
					  	BOOST_PP_SEQ_FOR_EACH_I(							\
					  			NASIU_V8W_METHOD_DISPATCHER_INIT,			\
					  			(cl_name,BOOST_PP_SEQ_SIZE(cseq)), mseq)\
					  , engine_(*e), object_(object) 						\
			{																\
			}																\
																			\
			~method_dispatcher() throw()									\
			{																\
				using namespace v8; 										\
				if (!object_.IsNearDeath())									\
					object_.Dispose();										\
			} 																\
																			\
			v8::Persistent<v8::Object> get_object() const					\
			{ 																\
				return object_;												\
			}																\
																			\
			engine_base& get_engine() const									\
			{ 																\
				return engine_;												\
			}																\
		}; 																	\
    }}}        																\
    /***/

#define NASIU_V8W_FIND_ABSTRACT(z, i, data /*cl_name,mseq-size*/) 			\
	/*namespace BOOST_PP_CAT(adapter_, BOOST_PP_TUPLE_ELEM(2, 0, data)) {*/ 	\
    	/*struct BOOST_PP_CAT(dummy, i) {};*/									\
    	template<> struct abstract_finder<BOOST_PP_TUPLE_ELEM(2, 0, data), i> { \
    		typedef BOOST_PP_TUPLE_ELEM(2, 0, data) class_type;\
    		struct dummy {}; \
    		struct test : virtual class_type				\
				BOOST_PP_REPEAT_FROM_TO(									\
						0,													\
						/*BOOST_PP_DEC(*/i/*)*/, 									\
						NASIU_V8W_FIND_ABSTRACT_INHERITANCE,				\
						class_type) 					\
				BOOST_PP_REPEAT_FROM_TO(									\
						BOOST_PP_INC(i), 									\
						BOOST_PP_TUPLE_ELEM(2, 1, data),					\
						NASIU_V8W_FIND_ABSTRACT_INHERITANCE,				\
						class_type) 					\
			{ 																	\
				v8::Persistent<v8::Object> get_object() const { throw 0; }		\
				engine_base& get_engine() const { throw 0; }					\
			}; 																	\
			typedef typename boost::mpl::if_<									\
					boost::is_abstract<test>,					\
					dispatcher_abstract<\
							class_method<class_type, i>::param_size::value, class_type, i, int>, 					\
					dispatcher_concrete<\
							class_method<class_type, i>::param_size::value, class_type, i, int> 					\
					>::type dispatcher; 										\
	};																		\
    /***/

#define NASIU_V8W_FIND_ABSTRACT_INHERITANCE(z, i, _) 					\
	, public dispatcher_concrete<class_method<class_type, i>::param_size::value, class_type, i, int>						\
	    /***/


/*#define NASIU_V8W_METHOD_DISPATCHER_METHOD_INFO(r, cl_name, i, xy)			\
	template <> 															\
	struct dispatchers<cl_name, i> {										\
    	typedef BOOST_PP_CAT(adapter_, cl_name)								\
				::BOOST_PP_CAT(dispatcher, i) type;							\
    }; 																		\
	*/

#define NASIU_V8W_METHOD_DISPATCHER_INHERITANCE(r, cl_name, i, xy)			\
	, public abstract_finder<cl_name, i>::dispatcher										\
	/***/

#define NASIU_V8W_METHOD_DISPATCHER_INIT(r, data, i, xy)					\
		, abstract_finder<class_type, i>::dispatcher(									\
					BOOST_PP_REPEAT(										\
							BOOST_PP_TUPLE_ELEM(2, 1, data), 				\
							NASIU_V8W_CTOR_PARAM_USE, ~))     				\
	/***/

#define NASIU_V8W_ADAPT_CLASS_M(r, data, i, xy)								\
	template <>																\
	struct class_method<BOOST_PP_TUPLE_ELEM(2, 0, data), i>					\
	{\
		typedef BOOST_PP_TUPLE_ELEM(2, 0, data) class_type;\
		static const char* get_name()										\
		{ 																	\
			return BOOST_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, 0, xy)); 			\
		}																	\
		typedef BOOST_TYPEOF(&class_type::BOOST_PP_TUPLE_ELEM(1, 0, xy)) type;    					\
		static type get_pmf()                                          		\
		{																	\
			return &BOOST_PP_TUPLE_ELEM(2, 0, data)							\
					::BOOST_PP_TUPLE_ELEM(1, 0, xy);  				 		\
		};																	\
		typedef BOOST_TYPEOF(&class_type::BOOST_PP_TUPLE_ELEM(1, 0, xy)) fn_type;						\
		typedef typename boost::function_types::parameter_types<			\
			fn_type>::type param_types; 									\
		typedef boost::mpl::size<param_types> param_size; 					\
		typedef typename boost::function_types::result_type<				\
			fn_type>::type rtype; 											\
	};																		\
	NASIU_V8W_CALL_ADAPTER(													\
			BOOST_PP_TUPLE_ELEM(2, 0, data), /*cl_name*/ 					\
			BOOST_PP_TUPLE_ELEM(1, 0, xy),   /*method_name*/ 				\
			BOOST_PP_TUPLE_ELEM(2, 1, data), /*cseq-size*/					\
			i); 															\
	/***/

#define NASIU_V8W_ADAPT_CLASS_A(r, cl_name, i, xy)							\
	template <>																\
	struct class_accessor<cl_name, i>										\
	{																		\
		static const char* get_name() 										\
		{ 																	\
			return BOOST_PP_TUPLE_ELEM(3, 0, xy);							\
		}																	\
		typedef BOOST_TYPEOF(&cl_name::BOOST_PP_TUPLE_ELEM(3, 1, xy)) 		\
				getter_type;    											\
		static getter_type get_getter_pmf()									\
		{																	\
			return &cl_name::BOOST_PP_TUPLE_ELEM(3, 1, xy);					\
		}																	\
		BOOST_PP_IF(														\
			BOOST_PP_IS_EMPTY(BOOST_PP_TUPLE_ELEM(3, 2, xy)),				\
			NASIU_V8W_ADAPT_CLASS_A_NO_SETTER(cl_name),						\
			NASIU_V8W_ADAPT_CLASS_A_SETTER(cl_name, 						\
					BOOST_PP_TUPLE_ELEM(3, 2, xy))) 						\
	};																		\
	/***/

#define NASIU_V8W_ADAPT_CLASS_A_SETTER(cl_name, s) 							\
	typedef BOOST_TYPEOF(&cl_name::s) setter_type;    						\
	static setter_type get_setter_pmf()										\
	{																		\
		return &cl_name::s;													\
	};																		\
	/***/

#define NASIU_V8W_ADAPT_CLASS_A_NO_SETTER(cl_name) 							\
	typedef boost::mpl::false_ setter_type;    								\
	/***/

#endif /* __NASIU__SCRIPTING__V8W__ADAPT_CLASS_HPP__ */
