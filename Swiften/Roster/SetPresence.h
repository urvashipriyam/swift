/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SetPresence_H
#define SWIFTEN_SetPresence_H

#include "Swiften/Elements/Presence.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Roster/RosterItemOperation.h"
#include "Swiften/Roster/ContactRosterItem.h"

namespace Swift {

class RosterItem;

class SetPresence : public RosterItemOperation {
	public:
		SetPresence(boost::shared_ptr<Presence> presence, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, presence->getFrom().toBare()), presence_(presence), compareType_(compareType) {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact && contact->getJID().equals(presence_->getFrom(), compareType_)) {
				if (presence_->getType() != Presence::Available) {
					contact->setStatusShow(StatusShow::None);
					contact->setStatusText(presence_->getStatus());
				} else {
					contact->setStatusShow(presence_->getShow());
					contact->setStatusText(presence_->getStatus());
				}
			}
		}
	
	private:
		boost::shared_ptr<Presence> presence_;
		JID::CompareType compareType_;
};

}
#endif

