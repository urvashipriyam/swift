/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Roster/RosterItem.h"
#include "Swiften/Elements/StatusShow.h"
#include "Swiften/Elements/Presence.h"

#include <map>
#include <boost/bind.hpp>
#include "Swiften/Base/boost_signalslib.h"
#include <boost/shared_ptr.hpp>

namespace Swift {

class GroupRosterItem;
class ContactRosterItem : public RosterItem {
	public:
		ContactRosterItem(const JID& jid, const String& name, GroupRosterItem* parent);
		virtual ~ContactRosterItem();

		StatusShow::Type getStatusShow() const;
		StatusShow::Type getSimplifiedStatusShow() const;
		String getStatusText() const;
		void setAvatarPath(const String& path);
		const String& getAvatarPath() const;
		const JID& getJID() const;
		void applyPresence(const String& resource, boost::shared_ptr<Presence> presence);
		void clearPresence();
		void calculateShownPresence();
	private:
		JID jid_;
		String avatarPath_;
		bool hidden_;
		std::map<String, boost::shared_ptr<Presence> > presences_;
		boost::shared_ptr<Presence> offlinePresence_;
		boost::shared_ptr<Presence> shownPresence_;
};

}

