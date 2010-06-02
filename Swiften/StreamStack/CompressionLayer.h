/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_COMPRESSIONLAYER_H
#define SWIFTEN_COMPRESSIONLAYER_H

#include <boost/noncopyable.hpp>
#include "Swiften/Base/boost_signalslib.h"

#include "Swiften/Base/ByteArray.h"
#include "Swiften/StreamStack/StreamLayer.h"
#include "Swiften/Compress/ZLibException.h"
#include "Swiften/Compress/ZLibCompressor.h"
#include "Swiften/Compress/ZLibDecompressor.h"

namespace Swift {
	class ZLibCompressor;
	class ZLibDecompressor;

	class CompressionLayer : public StreamLayer, boost::noncopyable {
		public:
			CompressionLayer() {}

			virtual void writeData(const ByteArray& data) {
				try {
					onWriteData(compressor_.process(data));
				}
				catch (const ZLibException& e) {
					onError();
				}
			}

			virtual void handleDataRead(const ByteArray& data) {
				try {
					onDataRead(decompressor_.process(data));
				}
				catch (const ZLibException& e) {
					onError();
				}
			}

		public:
			boost::signal<void ()> onError;

		private:
			ZLibCompressor compressor_;
			ZLibDecompressor decompressor_;
	};
}

#endif
