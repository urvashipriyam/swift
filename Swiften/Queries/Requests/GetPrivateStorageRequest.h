/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_signalslib.h"
#include <boost/shared_ptr.hpp>

#include "Swiften/Queries/Request.h"
#include "Swiften/Elements/PrivateStorage.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	template<typename PAYLOAD_TYPE>
	class GetPrivateStorageRequest : public Request {
		public:
			GetPrivateStorageRequest(IQRouter* router) : Request(IQ::Get, JID(), boost::shared_ptr<PrivateStorage>(new PrivateStorage(boost::shared_ptr<Payload>(new PAYLOAD_TYPE()))), router) {
			}

			virtual void handleResponse(boost::shared_ptr<Payload> payload, boost::optional<ErrorPayload> error) {
				boost::shared_ptr<PrivateStorage> storage = boost::dynamic_pointer_cast<PrivateStorage>(payload);
				if (storage) {
					onResponse(boost::dynamic_pointer_cast<PAYLOAD_TYPE>(storage->getPayload()), error);
				}
				else {
					onResponse(boost::shared_ptr<PAYLOAD_TYPE>(), error);
				}
			}

		public:
			boost::signal<void (boost::shared_ptr<PAYLOAD_TYPE>, const boost::optional<ErrorPayload>&)> onResponse;
	};
}
