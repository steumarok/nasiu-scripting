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

#if !defined(__NASIU__SCRIPTING__SCRIPT_ENGINE_HPP__)
#define __NASIU__SCRIPTING__SCRIPT_ENGINE_HPP__

#include <string>

namespace nasiu { namespace scripting {

struct nil { };

struct undefined { };

template<typename Tag>
class script_engine;

template<typename Tag>
struct engine_traits;

template<typename Tag>
class script;

template<typename Tag>
class script_engine
{
	typedef engine_traits<Tag> traits;
	typedef typename traits::engine_type engine_type;

	engine_type engine_;

public:
	script_engine()
	{
	}

	template<typename T>
	void
	set(
			const std::string& name,
			T value)
	{
		engine_.set(name, value);
	}

	void
	set(
			const std::string& name,
			const char* value)
	{
		engine_.set(name, std::string(value));
	}

	template<typename T>
	T
	get(
			const std::string& name)
	{
		return engine_.get<T>(name);
	}

	void
	exec(
			const std::string& source)
	{
		engine_.exec(source);
	}

	template<typename R>
	R
	eval(
			const std::string& source)
	{
		return engine_.eval<R>(source);
	}

	script<Tag>
	compile(
			const std::string& source)
	{
		return engine_.compile(source);
	}

	template<class C>
	void
	bind_class()
	{
		engine_.bind_class<C>();
	}

	template<class F>
	void
	bind_function(
			const std::string& name,
			F func)
	{
		engine_.bind_function(name, func);
	}

	engine_type&
	get_engine()
	{
		return engine_;
	}

};

template<typename Tag>
class script
{

};

}}

#endif /* __NASIU__SCRIPTING__SCRIPT_ENGINE_HPP__ */
