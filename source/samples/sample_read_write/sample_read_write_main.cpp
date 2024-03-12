// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#include "../../modules/core/Reader.h"
#include "../../modules/core/Writer.h"
#include "../../modules/core/Depth.h"
#include "../../modules/global.h"
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

class Image
{
public:

	Image(uint64_t width, uint64_t height, uint64_t channels, fsi::Depth depth)
	{
		data = new uint8_t[width*height*channels*fsi::sizeOfDepth(depth)];
		this->width = width;
		this->height = height;
		this->channels = channels;
		this->depth = depth;
	}

	~Image()
	{
		delete data;
	}

	FSI_DISABLE_COPY_MOVE(Image); // Just to keep memory management simple

	uint8_t* data;

	uint64_t width;

	uint64_t height;

	uint64_t channels;

	fsi::Depth depth;
};

void invertColor(uint64_t width, uint64_t height, uint64_t channels, fsi::Depth depth, uint8_t* data)
{
	assert(depth == fsi::Depth::Uint16 && "Data must be Uint16");
	assert(channels == 3 && "Image must have 3 channels");

	uint16_t* dataPtr = reinterpret_cast<uint16_t*>(data);

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			dataPtr[y*width*channels + x*channels + 0]
				= 65535 - dataPtr[y*width*channels + x*channels + 0];
			dataPtr[y*width*channels + x*channels + 1]
				= 65535 - dataPtr[y*width*channels + x*channels + 1];
			dataPtr[y*width*channels + x*channels + 2]
				= 65535 - dataPtr[y*width*channels + x*channels + 2];
		}
	}
}

int main()
{
	using std::cout;

	std::filesystem::path inPath = "../../extras/samples/stone-wall-7/input.fsi";
	std::filesystem::path outPath = "../../extras/samples/stone-wall-7/output.fsi";

	fsi::Result result;

	// Read
	// ----

	cout << "Reading input...\n";

	fsi::Reader reader;

	result = reader.open(inPath);
	if (result != fsi::Result::Code::Success)
	{
		cout << result.message() << "\n";
		return 1;
	}

	fsi::Header headerReader = reader.header();

	Image image(headerReader.width, headerReader.height, headerReader.channels, headerReader.depth);

	result = reader.read(image.data);
	if (result != fsi::Result::Code::Success)
	{
		cout << result.message() << "\n";
		return 1;
	}

	reader.close();

	cout << "Input read successfully\n";

	cout << " ---- Image information ----\n";
	cout << "   Width: " << image.width << "\n";
	cout << "   Height: " << image.height << "\n";
	cout << "   Channels: " << image.channels << "\n";
	cout << "   Depth: " << image.depth << "\n";
	cout << " ---------------------------\n";

	// Invert color
	// ------------

	cout << "Inverting image colors...\n";
	invertColor(image.width, image.height, image.channels, image.depth, image.data);
	cout << "Image colors inverted\n";

	// Write
	// -----

	cout << "Writing output...\n";

	fsi::Writer writer;
	fsi::Header headerWriter;
	headerWriter.width = image.width;
	headerWriter.height = image.height;
	headerWriter.channels = image.channels;
	headerWriter.depth = image.depth;

	result = writer.open(outPath, headerWriter, fsi::FormatVersion::V1);
	if (result != fsi::Result::Code::Success)
	{
		cout << result.message() << "\n";
		return 1;
	}

	result = writer.write(&image.data[0]);
	if (result != fsi::Result::Code::Success)
	{
		cout << result.message() << "\n";
		return 1;
	}

	writer.close();

	cout << "Output written successfully\n";

	return 0;
}