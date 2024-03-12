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
#include <vector>
#include <iostream>

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

	FSI_DISABLE_COPY_MOVE(Image);

	uint8_t* data;

	uint64_t width;

	uint64_t height;

	uint64_t channels;

	fsi::Depth depth;
};

int main()
{
	using std::cout;

	std::filesystem::path inPath = "../../samples/stone-wall-7/input.fsi";
	std::filesystem::path outPath = "../../samples/stone-wall-7/output.fsi";

	fsi::Result result;

	// Read
	// ----

	fsi::Reader reader;

	result = reader.open(inPath);
	if (result != fsi::Result::Code::Success)
	{
		std::cout << result.message() << "\n";
		return 1;
	}

	fsi::Header headerReader = reader.header();

	Image image(headerReader.width, headerReader.height, headerReader.channels, headerReader.depth);

	result = reader.read(image.data);
	if (result != fsi::Result::Code::Success)
	{
		std::cout << result.message() << "\n";
		return 1;
	}

	reader.close();

	// Invert color
	// ------------

	// ...

	// Write
	// -----

	fsi::Writer writer;
	fsi::Header headerWriter;
	headerWriter.width = image.width;
	headerWriter.height = image.height;
	headerWriter.channels = image.channels;
	headerWriter.depth = image.depth;
	std::cout << headerWriter.width << "\n";
	result = writer.open(outPath, headerWriter, fsi::FormatVersion::V1);
	if (result != fsi::Result::Code::Success)
	{
		std::cout << result.message() << "\n";
		return 1;
	}

	result = writer.write(&image.data[0]);
	if (result != fsi::Result::Code::Success)
	{
		std::cout << result.message() << "\n";
		return 1;
	}

	writer.close();

	return 0;
}