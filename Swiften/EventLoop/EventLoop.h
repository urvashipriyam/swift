/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>

#include <boost/function.hpp>
#include <boost/thread.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/Event.h>

namespace Swift {
	class EventOwner;

	/**
	 *	The \ref EventLoop class provides the abstract interface for implementing event loops to use with Swiften.
	 *  
	 *  Events are added to the event queue using the \ref postEvent method and can be removed from the queue using
	 *  the \ref removeEventsFromOwner method.
	 */
	class SWIFTEN_API EventLoop {
		public:
			EventLoop();
			virtual ~EventLoop();

			/**
			 * The \ref postEvent method allows events to be added to the event queue of the \ref EventLoop.
			 * An optional \ref EventOwner can be passed, allowing later removal of events that have not yet been
			 * executed using the \ref removeEventsFromOwner method.
			 */
			void postEvent(boost::function<void ()> event, boost::shared_ptr<EventOwner> owner = boost::shared_ptr<EventOwner>());
			
			/**
			 * The \ref removeEventsFromOwner method removes all events from the specified \ref owner from the
			 * event queue.
			 */
			void removeEventsFromOwner(boost::shared_ptr<EventOwner> owner);

		protected:
			/**
			 * The \ref handleNextEvents method is called by an implementation of the abstract \ref EventLoop class
			 * at any point after the virtual \ref eventPosted method has been called.
			 * This method does not block, except for short-time synchronization.
			 * It can process multiple events before it reutrns.
			 */
			void handleNextEvents();

			/**
			 * The \ref eventPosted virtual method serves as notification for when events are still available in the queue.
			 * It is called after the first event is posted to an empty queue or after an event has been handled in
			 * \ref handleNextEvents and there are still remaining events in the queue.
			 */
			virtual void eventPosted() = 0;

		private:
			unsigned int nextEventID_;
			std::list<Event> events_;
			boost::recursive_mutex eventsMutex_;
			boost::recursive_mutex removeEventsMutex_;
	};
}
