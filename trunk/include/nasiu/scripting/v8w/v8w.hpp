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

#if !defined(__NASIU__SCRIPTING__V8W__V8W_HPP__)
#define __NASIU__SCRIPTING__V8W__V8W_HPP__

#include <nasiu/scripting/script_engine.hpp>
#include <nasiu/scripting/v8w/tags.hpp>
#include <nasiu/scripting/v8w/engine.hpp>
#include <nasiu/scripting/v8w/class_type.hpp>
#include <nasiu/scripting/v8w/function.hpp>

namespace nasiu { namespace scripting {

template<>
struct engine_traits<tags::v8w>
{
	typedef v8w::engine engine_type;
};

namespace v8w
{

void invoke_gc()
{
	while (!v8::V8::IdleNotification()) {};
}

}

}}

#endif /* __NASIU__SCRIPTING__V8W__V8W_HPP__ */
