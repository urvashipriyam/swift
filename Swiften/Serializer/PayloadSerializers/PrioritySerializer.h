/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Priority.h>

namespace Swift {
	class SWIFTEN_API PrioritySerializer : public GenericPayloadSerializer<Priority> {
		public:
			PrioritySerializer() : GenericPayloadSerializer<Priority>() {}

			virtual std::string serializePayload(boost::shared_ptr<Priority> priority)  const {
				return "<priority>" + boost::lexical_cast<std::string>(priority->getPriority()) + "</priority>";
			}
	};
}
