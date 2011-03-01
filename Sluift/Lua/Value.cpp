/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Value.h"

extern "C" {
	#include <lualib.h>
}
#include <boost/variant/apply_visitor.hpp>
#include <Swiften/Base/foreach.h>

using namespace Swift;
using namespace Swift::Lua;

namespace {
	struct PushVisitor : public boost::static_visitor<> {
		PushVisitor(lua_State* state) : state(state) {
		}

		void operator()(const Nil&) const {
			lua_pushnil(state);
		}

		void operator()(const bool& b) const {
			lua_pushboolean(state, b);
		}

		void operator()(const int& i) const {
			lua_pushnumber(state, i);
		}

		void operator()(const std::string& s) const {
			lua_pushstring(state, s.c_str());
		}

		void operator()(const std::vector<Value>& list) const {
			lua_newtable(state);
			int i = 0;
			foreach(const Value& value, list) {
				boost::apply_visitor(PushVisitor(state), value);
				lua_rawseti(state, -2, i + 1);
			}
		}

		void operator()(const std::map<std::string, boost::shared_ptr<Value> >& table) const {
			lua_createtable(state, 0, table.size());
			for(std::map<std::string, boost::shared_ptr<Value> >::const_iterator i = table.begin(); i != table.end(); ++i) {
				boost::apply_visitor(PushVisitor(state), *i->second);
				lua_setfield(state, -2, i->first.c_str());
			}
		}

		lua_State* state;
	};
}

namespace Swift { namespace Lua {

void pushValue(lua_State* state, const Value& value) {
	boost::apply_visitor(PushVisitor(state), value);
}

}}
