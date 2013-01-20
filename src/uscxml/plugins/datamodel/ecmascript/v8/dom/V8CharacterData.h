/*
    This file is part of the Arabica open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef V8CharacterData_h
#define V8CharacterData_h

#include "DOM/Node.hpp"
#include "V8Node.h"
#include "uscxml/plugins/datamodel/ecmascript/v8/dom/V8DOM.h"
#include <v8.h>

namespace Arabica {
namespace DOM {

class V8CharacterData {
public:
	struct V8CharacterDataPrivate {
		V8DOM* dom;
		Arabica::DOM::CharacterData<std::string>* arabicaThis;
	};

	V8_DESTRUCTOR(V8CharacterDataPrivate);
	static bool hasInstance(v8::Handle<v8::Value>);

	static v8::Handle<v8::Value> substringDataCallback(const v8::Arguments&);
	static v8::Handle<v8::Value> appendDataCallback(const v8::Arguments&);
	static v8::Handle<v8::Value> insertDataCallback(const v8::Arguments&);
	static v8::Handle<v8::Value> deleteDataCallback(const v8::Arguments&);
	static v8::Handle<v8::Value> replaceDataCallback(const v8::Arguments&);

	static v8::Handle<v8::Value> dataAttrGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void dataAttrSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> lengthAttrGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);

	static v8::Persistent<v8::FunctionTemplate> Tmpl;
	static v8::Handle<v8::FunctionTemplate> getTmpl() {
		if (Tmpl.IsEmpty()) {
			v8::Handle<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New();
			tmpl->SetClassName(v8::String::New("CharacterData"));
			tmpl->ReadOnlyPrototype();

			v8::Local<v8::ObjectTemplate> instance = tmpl->InstanceTemplate();
			v8::Local<v8::ObjectTemplate> prototype = tmpl->PrototypeTemplate();
			instance->SetInternalFieldCount(1);

			instance->SetAccessor(v8::String::NewSymbol("data"), V8CharacterData::dataAttrGetter, V8CharacterData::dataAttrSetter,
			                      v8::External::New(0), static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None));
			instance->SetAccessor(v8::String::NewSymbol("length"), V8CharacterData::lengthAttrGetter, 0,
			                      v8::External::New(0), static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None));

			prototype->Set(v8::String::NewSymbol("substringData"),
			               v8::FunctionTemplate::New(V8CharacterData::substringDataCallback, v8::Undefined()), static_cast<v8::PropertyAttribute>(v8::DontDelete));
			prototype->Set(v8::String::NewSymbol("appendData"),
			               v8::FunctionTemplate::New(V8CharacterData::appendDataCallback, v8::Undefined()), static_cast<v8::PropertyAttribute>(v8::DontDelete));
			prototype->Set(v8::String::NewSymbol("insertData"),
			               v8::FunctionTemplate::New(V8CharacterData::insertDataCallback, v8::Undefined()), static_cast<v8::PropertyAttribute>(v8::DontDelete));
			prototype->Set(v8::String::NewSymbol("deleteData"),
			               v8::FunctionTemplate::New(V8CharacterData::deleteDataCallback, v8::Undefined()), static_cast<v8::PropertyAttribute>(v8::DontDelete));
			prototype->Set(v8::String::NewSymbol("replaceData"),
			               v8::FunctionTemplate::New(V8CharacterData::replaceDataCallback, v8::Undefined()), static_cast<v8::PropertyAttribute>(v8::DontDelete));


			tmpl->Inherit(V8Node::getTmpl());
			Tmpl = v8::Persistent<v8::FunctionTemplate>::New(tmpl);
		}
		return Tmpl;
	}


};

}
}

#endif // V8CharacterData_h