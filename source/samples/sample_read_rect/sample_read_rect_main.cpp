// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#include "../../modules/core/Depth.hpp"
#include "../../modules/core/ProgressThread.h"
#include "../../modules/core/Exception.h"
#include "../../modules/core/Reader.h"
#include "../../modules/core/Writer.h"
#include "../../modules/core/Timer.h"
#include "../../modules/global.h"
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

struct Image
{
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

	uint8_t* data;
	uint64_t width;
	uint64_t height;
	uint64_t channels;
	fsi::Depth depth;

	FSI_DISABLE_COPY_MOVE(Image); // Just to keep memory management simple
};

int main()
{
	using std::cout;

	std::filesystem::path inPath = "../../extras/samples/read_rect/input.fsi";
	std::filesystem::path outPath = "../../extras/samples/read_rect/rect_output.fsi";

	cout << "Reading input...\n";

	fsi::Timer timer; timer.start();

	fsi::Reader reader;

	try
	{
		reader.open(inPath);
	}
	catch (fsi::Exception& e)
	{
		cout << e << "\n";
		return 1;
	}

	const fsi::Header header = reader.header();

	const uint64_t cropX = 3333;
	const uint64_t cropY = 2999;
	const uint64_t cropWidth = 355;
	const uint64_t cropHeight = 801;

	Image image(cropWidth, cropHeight, header.channels, header.depth);

	try
	{
		reader.readRect(
			image.data,
			cropX,
			cropY,
			cropWidth,
			cropHeight
		);
	}
	catch (fsi::Exception& e)
	{
		cout << e << "\n";
		return 1;
	}

	reader.close();

	cout << "Crop read successfully in " << timer.elapsedMs() << " ms\n";

	cout << "Input read successfully\n";

	cout << " ---- Image information ----\n";
	cout << "   Width: " << image.width << "\n";
	cout << "   Height: " << image.height << "\n";
	cout << "   Channels: " << image.channels << "\n";
	cout << "   Depth: " << image.depth << "\n";
	cout << " ---------------------------\n";

	cout << "Writing output...\n";

	// Create destination path if it doesn't exist
	std::error_code createDirsError;
	std::filesystem::create_directories(outPath.parent_path(), createDirsError);
	if (createDirsError)
	{
		cout << "Could not create output path\n";
		return 1;
	}

	fsi::Header headerWriter;
	headerWriter.width = image.width;
	headerWriter.height = image.height;
	headerWriter.channels = image.channels;
	headerWriter.depth = image.depth;
	headerWriter.hasThumb = true;

	timer.start();

	fsi::Writer writer(fsi::FormatVersion::V2);

	try
	{
		writer.open(outPath, headerWriter);
	}
	catch (fsi::Exception& e)
	{
		cout << e << "\n";
		return 1;
	}

	try
	{
		writer.write(image.data);
	}
	catch (fsi::Exception& e)
	{
		cout << e << "\n";
		return 1;
	}

	writer.close();

	cout << "Crop written successfully in " << timer.elapsedMs() << " ms\n";

	return 0;
}