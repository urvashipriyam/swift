/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_signalslib.h"
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Session/Session.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Network/Connection.h"

namespace Swift {
	class ProtocolHeader;
	class Element;
	class Stanza;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class StreamStack;
	class UserRegistry;
	class XMPPLayer;
	class ConnectionLayer;
	class Connection;
	class ByteArray;

	class ServerFromClientSession : public Session {
		public:
			ServerFromClientSession(
					const String& id,
					boost::shared_ptr<Connection> connection, 
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					UserRegistry* userRegistry);

			boost::signal<void ()> onSessionStarted;

		private:
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader& header);

			void setInitialized();
			bool isInitialized() const { 
				return initialized; 
			}

		private:
			String id_;
			UserRegistry* userRegistry_;
			bool authenticated_;
			bool initialized;
			String user_;
	};
}
