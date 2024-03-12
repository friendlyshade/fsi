#pragma once
#include <string>

namespace fsi
{

const uint32_t DEFAULT_FORMAT_VERSION = 2;

const uint8_t expectedFormatSignature[] = { 'f', 's', 'i', 'f' };

const std::string expectedFileExtension = ".fsi";

const uint64_t defaultBufferSize = 1024*1024; // in bytes

const uint64_t PROGRESS_CALLBACK_CALL_INTERVAL = 250; // in ms

}