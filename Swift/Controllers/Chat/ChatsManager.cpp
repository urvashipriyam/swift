/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatsManager.h"

#include <boost/bind.hpp>

#include "Swiften/Client/Client.h"

#include "Swift/Controllers/Chat/ChatController.h"
#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIInterfaces/ChatListWindowFactory.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Elements/ChatState.h"
#include "Swiften/MUC/MUCBookmarkManager.h"

namespace Swift {

typedef std::pair<JID, ChatController*> JIDChatControllerPair;
typedef std::pair<JID, MUCController*> JIDMUCControllerPair;

ChatsManager::ChatsManager(JID jid, StanzaChannel* stanzaChannel, IQRouter* iqRouter, EventController* eventController, ChatWindowFactory* chatWindowFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, boost::shared_ptr<DiscoInfo> serverDiscoInfo, PresenceSender* presenceSender, UIEventStream* uiEventStream, ChatListWindowFactory* chatListWindowFactory, bool useDelayForLatency, TimerFactory* timerFactory) : jid_(jid), useDelayForLatency_(useDelayForLatency) {
	timerFactory_ = timerFactory;
	eventController_ = eventController;
	stanzaChannel_ = stanzaChannel;
	iqRouter_ = iqRouter;
	chatWindowFactory_ = chatWindowFactory;
	nickResolver_ = nickResolver;
	presenceOracle_ = presenceOracle;
	avatarManager_ = NULL;
	serverDiscoInfo_ = serverDiscoInfo;
	presenceSender_ = presenceSender;
	uiEventStream_ = uiEventStream;
	mucBookmarkManager_ = new MUCBookmarkManager(iqRouter);
	mucBookmarkManager_->onBookmarkAdded.connect(boost::bind(&ChatsManager::handleMUCBookmarkAdded, this, _1));
	mucBookmarkManager_->onBookmarkRemoved.connect(boost::bind(&ChatsManager::handleMUCBookmarkRemoved, this, _1));
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatsManager::handlePresenceChange, this, _1, _2));
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&ChatsManager::handleUIEvent, this, _1));
	chatListWindow_ = chatListWindowFactory->createWindow(uiEventStream_);
}

ChatsManager::~ChatsManager() {
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		delete controllerPair.second;
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		delete controllerPair.second;
	}
	delete mucBookmarkManager_;
}

void ChatsManager::handleMUCBookmarkAdded(const MUCBookmark& bookmark) {
	std::map<JID, MUCController*>::iterator it = mucControllers_.find(bookmark.getRoom());
	if (it == mucControllers_.end() && bookmark.getAutojoin()) {
		//FIXME: need vcard stuff here to get a nick
		handleJoinMUCRequest(bookmark.getRoom(), bookmark.getNick());
	}
	chatListWindow_->addMUCBookmark(bookmark);
}

void ChatsManager::handleMUCBookmarkRemoved(const MUCBookmark& bookmark) {
	chatListWindow_->removeMUCBookmark(bookmark);
}

void ChatsManager::handleUserLeftMUC(MUCController* mucController) {
	std::map<JID, MUCController*>::iterator it;
	for (it = mucControllers_.begin(); it != mucControllers_.end(); it++) {
		if ((*it).second == mucController) {
			mucControllers_.erase(it);
			delete mucController;
			return;
		}
	}
}

void ChatsManager::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<RequestChatUIEvent> chatEvent = boost::dynamic_pointer_cast<RequestChatUIEvent>(event);
	if (chatEvent) {
		handleChatRequest(chatEvent->getContact());
		return;
	}
	boost::shared_ptr<JoinMUCUIEvent> joinMUCEvent = boost::dynamic_pointer_cast<JoinMUCUIEvent>(event);
	if (joinMUCEvent) {
		handleJoinMUCRequest(joinMUCEvent->getJID(), joinMUCEvent->getNick());
		return;
	}
	boost::shared_ptr<RemoveMUCBookmarkUIEvent> removeMUCBookmarkEvent = boost::dynamic_pointer_cast<RemoveMUCBookmarkUIEvent>(event);
	if (removeMUCBookmarkEvent) {
		mucBookmarkManager_->removeBookmark(removeMUCBookmarkEvent->getBookmark());
		return;
	}
	boost::shared_ptr<AddMUCBookmarkUIEvent> addMUCBookmarkEvent = boost::dynamic_pointer_cast<AddMUCBookmarkUIEvent>(event);
	if (addMUCBookmarkEvent) {
		mucBookmarkManager_->addBookmark(addMUCBookmarkEvent->getBookmark());
		return;
	}
	boost::shared_ptr<EditMUCBookmarkUIEvent> editMUCBookmarkEvent = boost::dynamic_pointer_cast<EditMUCBookmarkUIEvent>(event);
	if (editMUCBookmarkEvent) {
		mucBookmarkManager_->replaceBookmark(editMUCBookmarkEvent->getOldBookmark(), editMUCBookmarkEvent->getNewBookmark());
		return;
	}
}

/**
 * If a resource goes offline, release bound chatdialog to that resource.
 */
void ChatsManager::handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> /*lastPresence*/) {
	if (isMUC(newPresence->getFrom().toBare())) return;
	if (newPresence->getType() != Presence::Unavailable) return;
	JID fullJID(newPresence->getFrom());
	std::map<JID, ChatController*>::iterator it = chatControllers_.find(fullJID);
	if (it == chatControllers_.end()) return;
	JID bareJID(fullJID.toBare());
	//It doesn't make sense to have two unbound dialogs.
	if (chatControllers_.find(bareJID) != chatControllers_.end()) return;
	rebindControllerJID(fullJID, bareJID);
}

void ChatsManager::setAvatarManager(AvatarManager* avatarManager) {
	avatarManager_ = avatarManager;
}

void ChatsManager::setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info) {
	serverDiscoInfo_ = info;
	foreach (JIDChatControllerPair pair, chatControllers_) {
		pair.second->setAvailableServerFeatures(info);
	}
	foreach (JIDMUCControllerPair pair, mucControllers_) {
		pair.second->setAvailableServerFeatures(info);
	}
}

/**
 * This is to be called on connect/disconnect.
 */ 
void ChatsManager::setEnabled(bool enabled) {
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		controllerPair.second->setEnabled(enabled);
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		controllerPair.second->setEnabled(enabled);
		if (enabled) {
			controllerPair.second->rejoin();
		}
	}

}

void ChatsManager::handleChatRequest(const String &contact) {
	ChatController* controller = getChatControllerOrFindAnother(JID(contact));
	controller->activateChatWindow();
}

ChatController* ChatsManager::getChatControllerOrFindAnother(const JID &contact) {
	ChatController* controller = getChatControllerIfExists(contact);
	if (!controller && !isMUC(contact.toBare())) {
		foreach (JIDChatControllerPair pair, chatControllers_) {
			if (pair.first.toBare() == contact.toBare()) {
				controller = pair.second;
				break;
			}
		}
	}
	return controller ? controller : createNewChatController(contact);
}

ChatController* ChatsManager::createNewChatController(const JID& contact) {
	ChatController* controller = new ChatController(jid_, stanzaChannel_, iqRouter_, chatWindowFactory_, contact, nickResolver_, presenceOracle_, avatarManager_, isMUC(contact.toBare()), useDelayForLatency_, uiEventStream_, eventController_);
	chatControllers_[contact] = controller;
	controller->setAvailableServerFeatures(serverDiscoInfo_);
	return controller;
}

ChatController* ChatsManager::getChatControllerOrCreate(const JID &contact) {
	ChatController* controller = getChatControllerIfExists(contact);
	return controller ? controller : createNewChatController(contact);
}

ChatController* ChatsManager::getChatControllerIfExists(const JID &contact) {
	if (chatControllers_.find(contact) == chatControllers_.end()) {
		//Need to look for an unbound window to bind first
		JID bare(contact.toBare());
		if (chatControllers_.find(bare) != chatControllers_.end()) {
			rebindControllerJID(bare, contact);
		} else {
			return NULL;
		}
	}
	return chatControllers_[contact];
}

void ChatsManager::rebindControllerJID(const JID& from, const JID& to) {
	chatControllers_[to] = chatControllers_[from];
	chatControllers_.erase(from);
	chatControllers_[to]->setToJID(to);
}

void ChatsManager::handleJoinMUCRequest(const JID &muc, const boost::optional<String>& nickMaybe) {
	std::map<JID, MUCController*>::iterator it = mucControllers_.find(muc);
	if (it != mucControllers_.end()) {
		it->second->rejoin();
	} else {
		String nick = nickMaybe ? nickMaybe.get() : jid_.getNode();
		MUCController* controller = new MUCController(jid_, muc, nick, stanzaChannel_, presenceSender_, iqRouter_, chatWindowFactory_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory_, eventController_);
		mucControllers_[muc] = controller;
		controller->setAvailableServerFeatures(serverDiscoInfo_);
		controller->onUserLeft.connect(boost::bind(&ChatsManager::handleUserLeftMUC, this, controller));
	}
	mucControllers_[muc]->activateChatWindow();
}

void ChatsManager::handleIncomingMessage(boost::shared_ptr<Message> message) {
	JID jid = message->getFrom();
	boost::shared_ptr<MessageEvent> event(new MessageEvent(message));
	if (!event->isReadable() && !message->getPayload<ChatState>() && message->getSubject().isEmpty()) {
		return;
	}

	// Try to deliver it to a MUC
	if (message->getType() == Message::Groupchat || message->getType() == Message::Error) {
		std::map<JID, MUCController*>::iterator i = mucControllers_.find(jid.toBare());
		if (i != mucControllers_.end()) {
			i->second->handleIncomingMessage(event);
			return;
		}
		else if (message->getType() == Message::Groupchat) {
			//FIXME: Error handling - groupchat messages from an unknown muc.
			return;
		}
	}
	
	//if not a mucroom
	getChatControllerOrCreate(jid)->handleIncomingMessage(event);
}

bool ChatsManager::isMUC(const JID& jid) const {
	return mucControllers_.find(jid.toBare()) != mucControllers_.end();
}



}
