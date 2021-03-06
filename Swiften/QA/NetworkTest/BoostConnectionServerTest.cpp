/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

#include <boost/shared_ptr.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/sleep.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/BoostIOServiceThread.h>

using namespace Swift;

class BoostConnectionServerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(BoostConnectionServerTest);
		CPPUNIT_TEST(testConstructor_TwoServersOnSamePort);
		CPPUNIT_TEST(testStart_Conflict);
		CPPUNIT_TEST(testStop);
		CPPUNIT_TEST(testIPv4Server);
		CPPUNIT_TEST(testIPv6Server);
		CPPUNIT_TEST(testIPv4IPv6DualStackServer);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			eventLoop_ = new DummyEventLoop();
			boostIOServiceThread_ = new BoostIOServiceThread();
			stopped_ = false;
			stoppedError_.reset();
			receivedNewConnection_ = false;
			connectFinished_ = false;
		}

		void tearDown() {
			delete boostIOServiceThread_;
			while (eventLoop_->hasEvents()) {
				eventLoop_->processEvents();
			}
			delete eventLoop_;
		}

		void testConstructor_TwoServersOnSamePort() {
			BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
		}

		void testStart_Conflict() {
			BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling->start();

			BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling2->onStopped.connect(
					boost::bind(&BoostConnectionServerTest::handleStopped_, this, _1));

			testling->stop();
		}

		void testStop() {
			BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling->start();

			testling->stop();

			BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
			testling2->start();

			testling2->stop();
		}

		void testIPv4Server() {
			BoostConnectionServer::ref testling = BoostConnectionServer::create(HostAddress("127.0.0.1"), 9999, boostIOServiceThread_->getIOService(), eventLoop_);
			testling->onNewConnection.connect(boost::bind(&BoostConnectionServerTest::handleNewConnection, this, _1));
			testling->start();

			BoostConnection::ref clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
			clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
			clientTestling->connect(HostAddressPort(HostAddress("127.0.0.1"), 9999));

			while (!connectFinished_) {
				Swift::sleep(10);
				eventLoop_->processEvents();
			}

			CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

			testling->stop();
		}

		void testIPv6Server() {
			BoostConnectionServer::ref testling = BoostConnectionServer::create(HostAddress("::1"), 9999, boostIOServiceThread_->getIOService(), eventLoop_);
			testling->onNewConnection.connect(boost::bind(&BoostConnectionServerTest::handleNewConnection, this, _1));
			testling->start();

			BoostConnection::ref clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
			clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
			clientTestling->connect(HostAddressPort(HostAddress("::1"), 9999));

			while (!connectFinished_) {
				Swift::sleep(10);
				eventLoop_->processEvents();
			}

			CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

			testling->stop();
		}

		void testIPv4IPv6DualStackServer() {
			BoostConnectionServer::ref testling = BoostConnectionServer::create(HostAddress("::"), 9999, boostIOServiceThread_->getIOService(), eventLoop_);
			testling->onNewConnection.connect(boost::bind(&BoostConnectionServerTest::handleNewConnection, this, _1));
			testling->start();

			// Test IPv4.
			BoostConnection::ref clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
			clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
			clientTestling->connect(HostAddressPort(HostAddress("127.0.0.1"), 9999));

			while (!connectFinished_) {
				Swift::sleep(10);
				eventLoop_->processEvents();
			}

			CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

			receivedNewConnection_ = false;
			connectFinished_ = false;

			// Test IPv6.
			clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
			clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
			clientTestling->connect(HostAddressPort(HostAddress("::1"), 9999));

			while (!connectFinished_) {
				Swift::sleep(10);
				eventLoop_->processEvents();
			}

			CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

			testling->stop();
		}

		void handleStopped_(boost::optional<BoostConnectionServer::Error> e) {
			stopped_ = true;
			stoppedError_ = e;
		}

		void handleNewConnection(boost::shared_ptr<Connection> /*connection*/) {
			receivedNewConnection_ = true;
		}

		void handleConnectFinished(bool /*error*/) {
			connectFinished_ = true;
		}

	private:
		BoostIOServiceThread* boostIOServiceThread_;
		DummyEventLoop* eventLoop_;
		bool stopped_;
		bool receivedNewConnection_;
		bool connectFinished_;
		boost::optional<BoostConnectionServer::Error> stoppedError_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoostConnectionServerTest);
