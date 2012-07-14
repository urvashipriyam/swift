/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>

namespace Swift {
	class SWIFTEN_API FullPayloadSerializerCollection : public PayloadSerializerCollection {
		public:
			FullPayloadSerializerCollection();
			~FullPayloadSerializerCollection();

		private:
			std::vector<PayloadSerializer*> serializers_;
	};
}
