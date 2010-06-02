/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_signalslib.h"
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Network/DomainNameResolveError.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {
	class DomainNameAddressQuery {
		public:
			virtual ~DomainNameAddressQuery();

			virtual void run() = 0;

			boost::signal<void (const std::vector<HostAddress>&, boost::optional<DomainNameResolveError>)> onResult;
	};
}
