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
#include <algorithm>
#include <filesystem>

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
		delete[] data;
	}

	uint8_t* data;
	uint64_t width;
	uint64_t height;
	uint64_t channels;
	fsi::Depth depth;

	FSI_DISABLE_COPY_MOVE(Image); // Just to keep memory management simple
};

int64_t positiveMod(int64_t value, int64_t size)
{
	const int64_t result = value % size;
	return result < 0 ? result + size : result;
}

void readRectRepeat(
	fsi::Reader& reader,
	uint8_t* data,
	int64_t x,
	int64_t y,
	uint64_t width,
	uint64_t height
)
{
	const fsi::Header header = reader.header();

	const uint64_t bytesPerPixel =
		static_cast<uint64_t>(header.channels) * fsi::sizeOfDepth(header.depth);

	const uint64_t dstStrideBytes =
		width * bytesPerPixel;

	uint64_t dstY = 0;

	while (dstY < height)
	{
		const uint32_t sourceY = static_cast<uint32_t>(
			positiveMod(y + static_cast<int64_t>(dstY), header.height)
		);

		const uint32_t chunkHeight = static_cast<uint32_t>(
			std::min<uint64_t>(
				height - dstY,
				header.height - sourceY
			)
		);

		uint64_t dstX = 0;

		while (dstX < width)
		{
			const uint32_t sourceX = static_cast<uint32_t>(
				positiveMod(x + static_cast<int64_t>(dstX), header.width)
			);

			const uint32_t chunkWidth = static_cast<uint32_t>(
				std::min<uint64_t>(
					width - dstX,
					header.width - sourceX
				)
			);

			uint8_t* chunkData =
				data +
				dstY * dstStrideBytes +
				dstX * bytesPerPixel;

			reader.readRect(
				chunkData,
				sourceX,
				sourceY,
				chunkWidth,
				chunkHeight,
				dstStrideBytes
			);

			dstX += chunkWidth;
		}

		dstY += chunkHeight;
	}
}

void writeImage(const Image& image, const std::filesystem::path& path)
{
	fsi::Header headerWriter;
	headerWriter.width = image.width;
	headerWriter.height = image.height;
	headerWriter.channels = image.channels;
	headerWriter.depth = image.depth;
	headerWriter.hasThumb = true;

	fsi::Writer writer(fsi::FormatVersion::V2);

	writer.open(path, headerWriter);
	writer.write(image.data);

	writer.close();
}

int main()
{
	using std::cout;

	std::filesystem::path inPath = "../../extras/samples/read_rect/input.fsi";
	std::filesystem::path outPath = "../../extras/samples/read_rect/rect_output.fsi";
	std::filesystem::path repeatOutPath = "../../extras/samples/read_rect/rect_repeat_output.fsi";

	cout << "Reading input...\n";

	fsi::Timer timer; timer.start();

	fsi::Reader reader;

	try
	{
		reader.open(inPath);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << "\n";
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
	catch (const std::exception& e)
	{
		cout << e.what() << "\n";
		return 1;
	}

	cout << "Crop read successfully in " << timer.elapsedMs() << " ms\n";

	cout << " ---- Image information ----\n";
	cout << "   Width: " << image.width << "\n";
	cout << "   Height: " << image.height << "\n";
	cout << "   Channels: " << image.channels << "\n";
	cout << "   Depth: " << image.depth << "\n";
	cout << " ---------------------------\n";

	cout << "Reading repeated corner crop...\n";

	const uint64_t repeatCropWidth = 1200;
	const uint64_t repeatCropHeight = 1200;

	const int64_t repeatCropX =
		static_cast<int64_t>(header.width) - 400;

	const int64_t repeatCropY =
		static_cast<int64_t>(header.height) - 400;

	Image repeatImage(
		repeatCropWidth,
		repeatCropHeight,
		header.channels,
		header.depth
	);

	timer.start();

	try
	{
		readRectRepeat(
			reader,
			repeatImage.data,
			repeatCropX,
			repeatCropY,
			repeatCropWidth,
			repeatCropHeight
		);
	}
	catch (const std::exception& e)
	{
		cout << e.what() << "\n";
		return 1;
	}

	reader.close();

	cout << "Repeated crop read successfully in " << timer.elapsedMs() << " ms\n";

	cout << "Writing output...\n";

	// Create destination path if it doesn't exist
	std::error_code createDirsError;
	std::filesystem::create_directories(outPath.parent_path(), createDirsError);
	if (createDirsError)
	{
		cout << "Could not create output path\n";
		return 1;
	}

	try
	{
		timer.start();
		writeImage(image, outPath);
		cout << "Crop written successfully in " << timer.elapsedMs() << " ms\n";

		timer.start();
		writeImage(repeatImage, repeatOutPath);
		cout << "Repeated crop written successfully in " << timer.elapsedMs() << " ms\n";
	}
	catch (const std::exception& e)
	{
		cout << e.what() << "\n";
		return 1;
	}

	return 0;
}