/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <set>
#include <vector>

#include <Swiften/Base/String.h>

namespace Swift {
	class JID;

	class ContactEditWindow {
		public:
			virtual ~ContactEditWindow() {};

			virtual void setEnabled(bool b) = 0;

			virtual void setContact(const JID& jid, const String& name, const std::vector<String>& groups, const std::set<String>& allGroups) = 0;

			virtual void show() = 0;
			virtual void hide() = 0;

			boost::signal<void ()> onRemoveContactRequest;
			boost::signal<void (const String& /* name */, const std::vector<String>& /* groups */)> onChangeContactRequest;
	};
}
