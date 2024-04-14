# Friendly Shade Image Format
Official repository for the FSI file format specification and IO implementation written in C++17.

## Why?

Friendly Shade Image Format (FSI for short) is a modern, future-proof image file format designed in 2022 by Sebastian Zapata from Friendly Shade. We are releasing this with the hope that 3D and image editing software companies will implement it, so we no longer struggle with the rather limited and ancient file formats such as EXR, TIF, PNG, etc. It supports images up to 1,048,575 by 1,048,575 pixels and 1,048,575 channels. It also supports 8, 16, 32 and 64 bit-depths as signed and unsigned integers, as well as 32 and 64-bit floating-point data types. These are the two main goals:

- Having a simpler image file format without layers, tags or other convoluted features for simple texturing and rendering tasks.
- Sharing 2D image data across software without file size, resolution, bit-depth or channel-count limitations.

## Support
For questions about implementation, please contact us at [https://friendlyshade.com/contact](https://friendlyshade.com/contact)

## Implementation
A C++17 implementation for encoding/decoding this format is also available as shared, static or even as a header-only library. It supports callbacks for reporting progress, canceling or even pausing read and write operations (see "sample_read_write" example for more details).

**Please note that this implementation does not account for endianness; it assumes operation on a little-endian computer.**

## API Usage Examples
### Read
Simple read operation excluding error handling
```cpp
#include <friendlyshade/fsi/core/Depth.h>
#include <friendlyshade/fsi/core/Header.h>
#include <friendlyshade/fsi/core/Reader.h>

int main()
{
  using fsi::Reader;
  using fsi::Header;
  
  Reader reader;
  reader.open("path/to/image.fsi");
  Header header = reader.header();
  Image image(headerReader.width, headerReader.height, headerReader.channels, headerReader.depth);
  reader.read(image.data);
  reader.close();

  return 0;
}
```
**Note that calling `Reader::close()` is optional. If it's not explicitly called, it will be invoked automatically during the destruction of the `Reader` object. However, the file will remain locked by the application until then.**

### Write
Simple write operation excluding error handling
```cpp
#include <friendlyshade/fsi/core/Depth.h>
#include <friendlyshade/fsi/core/Header.h>
#include <friendlyshade/fsi/core/Writer.h>

int main()
{
  using fsi::Writer;
  using fsi::Header;
  using fsi::Depth;

  Image image(1024, 1024, 3, Depth::Uint16);
    
  Writer writer;
  Header header;
  header.width = image.width;
  header.height = image.height;
  header.channels = image.channels;
  header.depth = image.depth;
  
  writer.open("path/to/image.fsi", header, fsi::FormatVersion::Latest);
  writer.write(image.data);
  writer.close();
  
  return 0;
}
```
**Note that calling `Writer::close()` is optional. If it's not explicitly called, it will be invoked automatically during the destruction of the `Writer` object. However, the file will remain locked by the application until then.**

## Specification
### FSI v2 (experimental)
All data should be read/written in little-endian byte order.

#### Header Section

| Description                     |    Required Value / Range    |   Size                 |
| :---                            |            :---:             |   :---                 |
| Format signature                |             'f'              |   1 byte (char)        |
| Format signature                |             's'              |   1 byte (char)        |
| Format signature                |             'i'              |   1 byte (char)        |
| Format signature                |             'f'              |   1 byte (char)        |
| Format version                  |              2               |   4 bytes (uint32_t)   |
| Image width                     |   1..2^20-1 (0..1,048,575)   |   4 bytes (uint32_t)   |
| Image height                    |   1..2^20-1 (0..1,048,575)   |   4 bytes (uint32_t)   |
| Image channel count             |   1..2^20-1 (0..1,048,575)   |   4 bytes (uint32_t)   |
| [Image depth](#image-depth)     |            1..10             |   1 byte (uint8_t)     |
| Has thumbnail                   |  0 or 1 (non-zero for true)  |   1 byte (uint8_t)     |

##### Image depth

###### Signed Integers

| Value | Data Type |
| :---  | :--       |
| 1     | int8_t    |
| 2     | int16_t   |
| 3     | int32_t   |
| 4     | int64_t   |

###### Unsigned Integers

| Value | Data Type |
| :---  | :--       |
| 5     | uint8_t   |
| 6     | uint16_t  |
| 7     | uint32_t  |
| 8     | uint64_t  |

###### Floating-point

| Value | Data Type |
| :---  | :--       |
| 9     | float     |
| 10    | double    |

#### Data Section

##### Thumbnail Data
- Thumbnail data is RGBA (4 channels) at 8-bit (unsigned) and dimensions are 256 x 256 pixels (262,144 bytes)
- Thumbnail data is stored in interleaved/packed order = RGBA RGBA RGBA, etc.

##### Image Data
- Image data is stored in interleaved/packed order = RGB RGB RGB, etc.
- Total size of the data in bytes = width * height * channels * [sizeof_data_type_in_bytes](#image-depth).
- It's recommended to read this section in chunks for efficiency, and then reinterpret it as an array of the data type of the [Image depth](#image-depth) of the image.

### FSI v1 (latest official)
All data should be read/written in little-endian byte order.

#### Header Section

| Description                     | Required Value             |   Size                 |
| :---                            |            :---:           |   :---                 |
| Format signature                |             'f'            |   1 byte (char)        |
| Format signature                |             's'            |   1 byte (char)        |
| Format signature                |             'i'            |   1 byte (char)        |
| Format signature                |             'f'            |   1 byte (char)        |
| Format version                  |              1             |   4 bytes (uint32_t)   |
| Image width                     |  1..2^20-1 (0..1,048,575)  |   4 bytes (uint32_t)   |
| Image height                    |  1..2^20-1 (0..1,048,575)  |   4 bytes (uint32_t)   |
| Image channel count             |  1..2^20-1 (0..1,048,575)  |   4 bytes (uint32_t)   |
| Image depth                     |            1..10           |   4 bytes (uint32_t)   |

##### Image depth

###### Signed Integers

| Value | Data Type |
| :---  | :--       |
| 1     | int8_t    |
| 2     | int16_t   |
| 3     | int32_t   |
| 4     | int64_t   |

###### Unsigned Integers

| Value | Data Type |
| :---  | :--       |
| 5     | uint8_t   |
| 6     | uint16_t  |
| 7     | uint32_t  |
| 8     | uint64_t  |

###### Floating-point

| Value | Data Type |
| :---  | :--       |
| 9     | float     |
| 10    | double    |

#### Data Section
- Data is stored in interleaved/packed order = RGB RGB RGB, etc.

- Total size of the data in bytes = width * height * channels * sizeof_data_type_in_bytes.

- It's recommended to read this section in chunks for efficiency, and then reinterpret it as an array of the data type of the Image depth of the image.
