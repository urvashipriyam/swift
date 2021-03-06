/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerDeleteConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>


#include <Sluift/LuaElementConvertors.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubOwnerDeleteConvertor::PubSubOwnerDeleteConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubOwnerDelete>("pubsub_owner_delete"),
		convertors(convertors) {
}

PubSubOwnerDeleteConvertor::~PubSubOwnerDeleteConvertor() {
}

boost::shared_ptr<PubSubOwnerDelete> PubSubOwnerDeleteConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubOwnerDelete> result = boost::make_shared<PubSubOwnerDelete>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "redirect");
	if (!lua_isnil(L, -1)) {
		if (boost::shared_ptr<PubSubOwnerRedirect> payload = boost::dynamic_pointer_cast<PubSubOwnerRedirect>(convertors->convertFromLuaUntyped(L, -1, "pubsub_owner_redirect"))) {
			result->setRedirect(payload);
		}
	}
	lua_pop(L, 1);
	return result;
}

void PubSubOwnerDeleteConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubOwnerDelete> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
	if (convertors->convertToLuaUntyped(L, payload->getRedirect()) > 0) {
		lua_setfield(L, -2, "redirect");
	}
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerDeleteConvertor::getDocumentation() const {
	return Documentation(
		"PubSubOwnerDelete",
		"This table has the following fields:\n\n"
		"- `node`: string\n"
		"- `redirect`: @{PubSubOwnerRedirect}\n"
	);
}
