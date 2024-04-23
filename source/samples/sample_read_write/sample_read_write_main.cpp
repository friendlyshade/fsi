// � 2023 Friendly Shade, Inc.
// � 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#include "../../modules/core/Depth.hpp"
#include "../../modules/core/ProgressThread.h"
#include "../../modules/core/Reader.h"
#include "../../modules/core/Writer.h"
#include "../../modules/core/Writer_V1.h"
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

	/*int* b = new int(5);
	int* a = new int(*b);
	// *a = *b;
	cout << "a: " << *a << "\n";
	return 0;
	*/


	/*
	const fsi::Header* header = new fsi::Header_V2;
	const fsi::Header& D_ptr = *header;// get a base pointer to derived type
	const fsi::Header_V2 *derived_ptr1 = dynamic_cast<const fsi::Header_V2*>(&D_ptr);// works fine

	if (derived_ptr1)
		cout << "Yes, it was converted!\n";
	else
		cout << "No, could not be converted!\n";

	return 0;
	*/

	// std::filesystem::path inPath = "../../extras/samples/stone-wall-7/input.fsi";
	// std::filesystem::path inPath = "../../extras/samples/stone-wall-7/input-18333px-rgb-16-bit.fsi";
	// std::filesystem::path inPath = "../../extras/samples/2-channel-image/924x2000px_2C_16bit.fsi";
	std::filesystem::path inPath = "../../extras/samples/stone-wall-7/input-18333px-gray-16-bit.fsi";
	std::filesystem::path outV1Path = "../../extras/samples/stone-wall-7/output-v1.fsi";
	std::filesystem::path outV2Path = "../../extras/samples/stone-wall-7/output-v2.fsi";
	// std::filesystem::path outPath = "../../extras/samples/2-channel-image/output.fsi";

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

	fsi::Header_V1 headerReader = reader.header();

	Image image(headerReader.width, headerReader.height, headerReader.channels, headerReader.depth);

	result = reader.read(image.data, progressCallback);
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
	//invertColor(image.width, image.height, image.channels, image.depth, image.data);
	cout << "Image colors inverted\n";

	// Write
	// -----

	cout << "Writing output...\n";

	// v1
	{
		fsi::Header_V1* headerWriter;
		headerWriter->width = image.width;
		headerWriter->height = image.height;
		headerWriter->channels = image.channels;
		headerWriter->depth = image.depth;

		fsi::Writer_V1 writer(headerWriter);

		result = writer.open(outV1Path);
		if (result != fsi::Result::Code::Success)
		{
			cout << result.message() << "\n";
			return 1;
		}

		fsi::Timer timer; timer.start();
		result = writer.write(image.data, progressCallback);
		if (result != fsi::Result::Code::Success)
		{
			cout << result.message() << "\n";
			return 1;
		}

		writer.close();

		cout << "Output (v1) written successfully in " << timer.elapsedMs() << " ms\n";
	}

	// v2
	{
		std::unique_ptr<fsi::Header_V2> headerWriter;
		headerWriter->width = image.width;
		headerWriter->height = image.height;
		headerWriter->channels = image.channels;
		headerWriter->depth = image.depth;
		headerWriter->depth = image.depth;

		std::unique_ptr<fsi::Writer> writer = fsi::Writer::createWriter(headerWriter);

		result = writer->open(outV1Path);
		if (result != fsi::Result::Code::Success)
		{
			cout << result.message() << "\n";
			return 1;
		}

		fsi::Timer timer; timer.start();
		result = writer->write(image.data, progressCallback);
		if (result != fsi::Result::Code::Success)
		{
			cout << result.message() << "\n";
			return 1;
		}

		writer->close();

		cout << "Output (v2) written successfully in " << timer.elapsedMs() << " ms\n";
	}

	return 0;
}