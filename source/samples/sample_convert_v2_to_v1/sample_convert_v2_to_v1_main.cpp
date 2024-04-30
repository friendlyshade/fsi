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
#include <utility>
#include <assert.h>

class Image
{
public:

	Image()
	{
		data = nullptr;
		this->width = 0;
		this->height = 0;
		this->channels = 0;
		this->depth = fsi::Depth::Invalid;
	}

	Image(uint64_t width, uint64_t height, uint64_t channels, fsi::Depth depth)
	{
		data = new uint8_t[width * height * channels * fsi::sizeOfDepth(depth)];
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
	using fsi::Depth;
	using fsi::Header;
	using fsi::FormatVersion;
	using fsi::Exception;
	using fsi::Reader;
	using fsi::Writer;
	using fsi::Timer;

	std::vector<std::pair<std::filesystem::path, std::filesystem::path>> pathPairs =
	{
		{
			"../../extras/samples/convert_v2_to_v1/input/924x2000px_1C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/924x2000px_1C_16bit.fsi"
		},
		{
			"../../extras/samples/convert_v2_to_v1/input/924x2000px_2C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/924x2000px_2C_16bit.fsi"
		},
		{
			"../../extras/samples/convert_v2_to_v1/input/924x2000px_3C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/924x2000px_3C_16bit.fsi",
		},
		{
			"../../extras/samples/convert_v2_to_v1/input/924x2000px_4C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/924x2000px_4C_16bit.fsi",
		},
		{
			"../../extras/samples/convert_v2_to_v1/input/2000x924px_1C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/2000x924px_1C_16bit.fsi",
		},
		{
			"../../extras/samples/convert_v2_to_v1/input/2000x924px_2C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/2000x924px_2C_16bit.fsi",
		},
		{
			"../../extras/samples/convert_v2_to_v1/input/2000x924px_3C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/2000x924px_3C_16bit.fsi",
		},
		{
			"../../extras/samples/convert_v2_to_v1/input/2000x924px_4C_16bit.fsi",
			"../../extras/samples/convert_v2_to_v1/output/2000x924px_4C_16bit.fsi",
		},
	};

	// --- Read ---

	for (auto& pathPair : pathPairs)
	{
		cout << "Reading input \"" << pathPair.first << "\"...\n";

		Reader reader;
		try
		{
			reader.open(pathPair.first);
		}
		catch (Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		Header headerReader = reader.header();

		Image image(headerReader.width, headerReader.height, headerReader.channels, headerReader.depth);
		std::vector<uint8_t> thumb;
		if (headerReader.hasThumb)
			thumb = std::vector<uint8_t>(headerReader.thumbWidth * headerReader.thumbHeight * 4);

		try
		{
			reader.read(image.data, headerReader.hasThumb ? thumb.data() : nullptr , progressCallback);
		}
		catch (Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		reader.close();

		cout << "Input read successfully (w: " << image.width << ", h: " << image.height << ", c: "
			<< image.channels << ", d: " << image.depth << ")\n";

		// --- Write ---

		cout << "Writing output...\n";

		// Create destination path if it doesn't exist
		std::error_code createDirsError;
		std::filesystem::create_directories(pathPair.second.parent_path(), createDirsError);
		if (createDirsError)
		{
			cout << "Could not create output path recursively\n";
			return 1;
		}

		Header headerWriter;
		headerWriter.width = image.width;
		headerWriter.height = image.height;
		headerWriter.channels = image.channels;
		headerWriter.depth = image.depth;

		Writer writer(FormatVersion::V1);

		try
		{
			writer.open(pathPair.second, headerWriter);
		}
		catch (Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		Timer timer; timer.start();
		try
		{
			writer.write(image.data, progressCallback);
		}
		catch (Exception& e)
		{
			cout << e << "\n";
			return 1;
		}

		writer.close();

		cout << "Output (v1) written successfully in " << timer.elapsedMs() << " ms\n";
	}

	return 0;
}