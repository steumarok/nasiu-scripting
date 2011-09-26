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

#if !defined(__NASIU__SCRIPTING__V8W__SCRIPT_HPP__)
#define __NASIU__SCRIPTING__V8W__SCRIPT_HPP__

#include <nasiu/scripting/v8w/tags.hpp>
#include <nasiu/scripting/v8w/js_to_native.hpp>
#include <nasiu/scripting/v8w/native_to_js.hpp>
#include <nasiu/scripting/v8w/invocation_scope.hpp>

namespace nasiu { namespace scripting {

template<>
class script<tags::v8w>
{
	v8w::engine_base& engine_;
	v8::Local<v8::Script> script_;

public:
	script(
			v8w::engine_base& e,
			v8::Local<v8::Script> script)
	: engine_(e), script_(script)
	{
	}

};

}}

#endif /* __NASIU__SCRIPTING__SCRIPT_HPP__ */
