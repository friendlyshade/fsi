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

void RGBtoRG(uint64_t width, uint64_t height, fsi::Depth depth, uint8_t* srcData, uint8_t* dstData)
{
	assert(depth == fsi::Depth::Uint16 && "Data must be Uint16");

	uint16_t* dataPtr = reinterpret_cast<uint16_t*>(srcData);

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			dataPtr[(y*width + x)*3 + 0] = dataPtr[(y*width + x)*2 + 0];
			dataPtr[(y*width + x)*3 + 1] = dataPtr[(y*width + x)*2 + 1];
		}
	}
}

void updateProgressBar(
	int tick,
	int total,
	int width)
{
	// Erase entire line and return cursor to the start of the line
	printf("\33[2K\r");

	// Important: the progress has to be set to 100 at some point before trying to print
	// anything else. Othwerwise it will be all merged with whatever it's printed during
	// the "progress". So, make sure to call progressBar(100, 100) before printing
	// anything else to the console.

	float ratio = 100.0f * tick / total;
	float count = (float)width * (float)tick / (float)total;
	std::string bar(width, ' ');
	std::fill(bar.begin(), bar.begin() + (int64_t)count, '|');
	printf("[%s] [ %4.2f %% ] %c",
		bar.c_str(),
		ratio,
		tick == total ? '\n' : '\r');
	std::fflush(stdout);
}

fsi::ProgressThread::StateRequest progressCallback(void* opaquePointer, float progress)
{
	updateProgressBar((int)(progress * 100.0f), 100, 50);

	return fsi::ProgressThread::StateRequest::NoAction;
}

int main()
{
	using std::cout;

	std::filesystem::path inPath = "../../extras/samples/read_write/input/input.fsi";
	std::filesystem::path outV1Path = "../../extras/samples/read_write/output/output-v1.fsi";
	std::filesystem::path outV2Path = "../../extras/samples/read_write/output/output-v2.fsi";

	// Read
	// ----

	cout << "Reading input...\n";

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

	fsi::Header headerReader = reader.header();

	Image image(headerReader.width, headerReader.height, headerReader.channels, headerReader.depth);

	try
	{
		reader.read(image.data, nullptr, progressCallback);
	}
	catch (fsi::Exception& e)
	{
		cout << e << "\n";
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

	// Image rgImage(image.width, image.height, 2, image.depth);
	// RGBtoRG(image.width, image.height, image.depth, image.data, rgImage.data);

	// Write
	// -----

	cout << "Writing output...\n";

	// v1
	{
		// Create destination path if it doesn't exist
		std::error_code createDirsError;
		std::filesystem::create_directories(outV1Path.parent_path(), createDirsError);
		if (createDirsError)
		{
			cout << "Could not create out v1 path recursively\n";
			return 1;
		}

		fsi::Header headerWriter;
		headerWriter.width = image.width;
		headerWriter.height = image.height;
		headerWriter.channels = image.channels;
		headerWriter.depth = image.depth;

		fsi::Writer writer(fsi::FormatVersion::V1);

		try
		{
			writer.open(outV1Path, headerWriter);
		}
		catch (fsi::Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		fsi::Timer timer; timer.start();

		try
		{
			writer.write(image.data, progressCallback);
		}
		catch (fsi::Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		writer.close();

		cout << "Output (v1) written successfully in " << timer.elapsedMs() << " ms\n";
	}

	// v2
	{
		// Create destination path if it doesn't exist
		std::error_code createDirsError;
		std::filesystem::create_directories(outV2Path.parent_path(), createDirsError);
		if (createDirsError)
		{
			cout << "Could not create out v2 path recursively\n";
			return 1;
		}

		fsi::Header headerWriter;
		headerWriter.width = image.width;
		headerWriter.height = image.height;
		headerWriter.channels = image.channels;
		headerWriter.depth = image.depth;
		headerWriter.hasThumb = false;

		fsi::Writer writer(fsi::FormatVersion::V2);

		try
		{
			writer.open(outV2Path, headerWriter);
		}
		catch (fsi::Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		fsi::Timer timer; timer.start();

		try
		{
			writer.write(image.data, progressCallback);
		}
		catch (fsi::Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		writer.close();

		cout << "Output (v2) written successfully in " << timer.elapsedMs() << " ms\n";
	}

	return 0;
}