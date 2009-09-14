#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/UnitTest/PayloadsSerializer.h"
#include "Swiften/Elements/PrivateStorage.h"
#include "Swiften/Elements/Storage.h"

using namespace Swift;

class PrivateStorageSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PrivateStorageSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		PrivateStorageSerializerTest() {}

		void testSerialize() {
			PayloadsSerializer serializer;

			boost::shared_ptr<PrivateStorage> privateStorage(new PrivateStorage());
			boost::shared_ptr<Storage> storage(new Storage());
			Storage::Conference conference;
			conference.name = "Swift";
			conference.jid = JID("swift@rooms.swift.im");
			conference.nick = "Alice";
			storage->addConference(conference);
			privateStorage->setPayload(storage);

			CPPUNIT_ASSERT_EQUAL(String(
				"<query xmlns=\"jabber:iq:private\">"	
					"<storage xmlns=\"storage:bookmarks\">"
						"<conference "
								"autojoin=\"1\" "
								"jid=\"swift@rooms.swift.im\" "
								"name=\"Swift\">"
							"<nick>Alice</nick>"
						"</conference>"
					"</storage>"
				"</query>"), serializer.serialize(privateStorage));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PrivateStorageSerializerTest);
