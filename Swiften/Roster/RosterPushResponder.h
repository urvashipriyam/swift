/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/API.h>
#include <Swiften/Queries/SetResponder.h>
#include <Swiften/Elements/RosterPayload.h>

namespace Swift {
	class SWIFTEN_API RosterPushResponder : public SetResponder<RosterPayload> {
		public:
			RosterPushResponder(IQRouter* router) : SetResponder<RosterPayload>(router) {}

		public:
			boost::signal<void (boost::shared_ptr<RosterPayload>)> onRosterReceived;

		private:
			virtual bool handleSetRequest(const JID& from, const JID&, const std::string& id, boost::shared_ptr<RosterPayload> payload) {
				if (getIQRouter()->isAccountJID(from)) {
					onRosterReceived(payload);
					sendResponse(from, id, boost::shared_ptr<RosterPayload>());
				}
				else {
					sendError(from, id, ErrorPayload::NotAuthorized, ErrorPayload::Cancel);
				}
				return true;
			}
	};
}
