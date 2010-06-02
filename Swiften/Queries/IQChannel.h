/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_IQChannel_H
#define SWIFTEN_IQChannel_H

#include "Swiften/Base/boost_signalslib.h"
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/IQ.h"

namespace Swift {
	class IQChannel {
		public:
			virtual ~IQChannel();

			virtual void sendIQ(boost::shared_ptr<IQ>) = 0;
			virtual String getNewIQID() = 0;
			
			virtual bool isAvailable() = 0;

			boost::signal<void (boost::shared_ptr<IQ>)> onIQReceived;
	};
}

#endif
