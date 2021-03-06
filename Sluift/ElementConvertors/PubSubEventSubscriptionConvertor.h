/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Swiften/Elements/PubSubEventSubscription.h>

namespace Swift {
	class LuaElementConvertors;

	class PubSubEventSubscriptionConvertor : public GenericLuaElementConvertor<PubSubEventSubscription> {
		public:
			PubSubEventSubscriptionConvertor(LuaElementConvertors* convertors);
			virtual ~PubSubEventSubscriptionConvertor();

			virtual boost::shared_ptr<PubSubEventSubscription> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
			virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubEventSubscription>) SWIFTEN_OVERRIDE;
			virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

		private:
			LuaElementConvertors* convertors;
	};
}
