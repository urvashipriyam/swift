/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubConfigure.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubConfigureConvertor : public GenericLuaElementConvertor<PubSubConfigure> {
		public:
			PubSubConfigureConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubConfigureConvertor();

			virtual boost::shared_ptr<PubSubConfigure> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubConfigure>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
