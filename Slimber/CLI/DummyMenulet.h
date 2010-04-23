/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Slimber/Menulet.h"

class DummyMenulet : public Menulet {
	public:
		DummyMenulet() {
		}

		void clear() {
		}

		void addItem(const Swift::String&, const Swift::String&) {
		}

		void addAboutItem() {
		}

		void addRestartItem() {
		}

		void addExitItem() {
		}

		void addSeparator() {
		}

		void setIcon(const Swift::String&) {
		}
};
