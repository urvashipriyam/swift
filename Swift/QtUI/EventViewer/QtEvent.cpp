/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/EventViewer/QtEvent.h>

#include <QColor>
#include <QDateTime>

#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>
#include <Swift/Controllers/XMPPEvents/IncomingFileTransferEvent.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtEvent::QtEvent(boost::shared_ptr<StanzaEvent> event, bool active) : event_(event) {
	active_ = active;
}

QVariant QtEvent::data(int role) {
 	switch (role) {
		case Qt::ToolTipRole: return QVariant(text()).toString() + "\n" + B2QDATE(event_->getTime()).toString();
		case Qt::DisplayRole: return QVariant(text());
		case Qt::TextColorRole: return QColor(active_ ? Qt::black : Qt::darkGray);
		case Qt::BackgroundColorRole: return QColor(active_ ? Qt::white : Qt::lightGray);
		case SenderRole: return QVariant(sender());
			/*case StatusTextRole: return statusText_;
		case AvatarRole: return avatar_;
		case PresenceIconRole: return getPresenceIcon();*/
	 	default: return QVariant();
	}
}

QString QtEvent::sender() {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event_);
	if (messageEvent) {
		return P2QSTRING(messageEvent->getStanza()->getFrom().toString());
	}
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionRequestEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(event_);
	if (subscriptionRequestEvent) {
		return P2QSTRING(subscriptionRequestEvent->getJID().toBare().toString());
	}
	boost::shared_ptr<ErrorEvent> errorEvent = boost::dynamic_pointer_cast<ErrorEvent>(event_);
	if (errorEvent) {
		return P2QSTRING(errorEvent->getJID().toBare().toString());
	}
	boost::shared_ptr<MUCInviteEvent> mucInviteEvent = boost::dynamic_pointer_cast<MUCInviteEvent>(event_);
	if (mucInviteEvent) {
		return P2QSTRING(mucInviteEvent->getInviter().toString());
	}
	boost::shared_ptr<IncomingFileTransferEvent> incomingFTEvent = boost::dynamic_pointer_cast<IncomingFileTransferEvent>(event_);
	if (incomingFTEvent) {
		return P2QSTRING(incomingFTEvent->getSender().toString());
	}
	return "";
}

QString QtEvent::text() {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event_);
	if (messageEvent) {
		return P2QSTRING(messageEvent->getStanza()->getBody());
	}
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionRequestEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(event_);
	if (subscriptionRequestEvent) {
		std::string reason = subscriptionRequestEvent->getReason();
		QString message;
		if (reason.empty()) {
			message = QString(QObject::tr("%1 would like to add you to their contact list.")).arg(subscriptionRequestEvent->getJID().toBare().toString().c_str());
		}
		else {
			message = QString(QObject::tr("%1 would like to add you to their contact list, saying '%2'")).arg(subscriptionRequestEvent->getJID().toBare().toString().c_str()).arg(reason.c_str());
		}
		return message;
	}
	boost::shared_ptr<ErrorEvent> errorEvent = boost::dynamic_pointer_cast<ErrorEvent>(event_);
	if (errorEvent) {
		return P2QSTRING(errorEvent->getText());
	}
	boost::shared_ptr<MUCInviteEvent> mucInviteEvent = boost::dynamic_pointer_cast<MUCInviteEvent>(event_);
	if (mucInviteEvent) {
		QString message = QString(QObject::tr("%1 has invited you to enter the %2 room.")).arg(P2QSTRING(mucInviteEvent->getInviter().toBare().toString())).arg(P2QSTRING(mucInviteEvent->getRoomJID().toString()));
		return message;
	}
	boost::shared_ptr<IncomingFileTransferEvent> incomingFTEvent = boost::dynamic_pointer_cast<IncomingFileTransferEvent>(event_);
	if (incomingFTEvent) {
		QString message = QString(QObject::tr("%1 would like to send a file to you.")).arg(P2QSTRING(incomingFTEvent->getSender().toBare().toString()));
		return message;
	}
	return "";
}

}
