# Friendly Shade Image Format
Official repository for the FSI file format specification and IO implementation written in C++17.

## Why?

Friendly Shade Image Format (FSI for short) is a modern, future-proof image file format designed in 2022 by Sebastian Zapata from Friendly Shade. We are releasing this with the hope that 3d and image editing software companies will implement it, so we no longer struggle with the rather limited and ancient file formats such as EXR, TIF, PNG, etc. It supports images up to 1,048,575 by 1,048,575 pixels and 1,048,575 channels. It also supports 8, 16, 32 and 64 bit-depths as signed and unsigned integers, as well as 32 and 64-bit floating-point data types. These are the two main goals:

- Having a simpler file format without layers, tags or other convoluted features for simple texturing and rendering tasks.
- Sharing 2d image data accross software without file size, resolution, bit-depth and channel count limitations.

## Support
For questions about implementation, please contact us at [https://friendlyshade.com/contact](https://friendlyshade.com/contact)

## FSI v2
All data should be read/written in little-endian byte order.

### Header Section

| Description                     |    Required Value / Range    |   Size                 |
| :---                            |            :---:             |   :---                 |
| Format signature                |             'f'              |   1 byte (char)        |
| Format signature                |             's'              |   1 byte (char)        |
| Format signature                |             'i'              |   1 byte (char)        |
| Format signature                |             'f'              |   1 byte (char)        |
| Format version                  |              2               |   4 bytes (uint32_t)   |
| Image width                     |   1..2^20-1 (0..1,048,575)   |   8 bytes (uint64_t)   |
| Image height                    |   1..2^20-1 (0..1,048,575)   |   8 bytes (uint64_t)   |
| Image channel count             |   1..2^20-1 (0..1,048,575)   |   8 bytes (uint64_t)   |
| [Image depth](#image-depth)     |            1..10             |   1 byte (uint8_t)     |

#### Image depth

##### Signed

| Value | Data Type |
| :---  | :--       |
| 1     | int8_t    |
| 2     | int16_t   |
| 3     | int32_t   |
| 4     | int64_t   |

##### Unsigned Integers

| Value | Data Type |
| :---  | :--       |
| 5     | uint8_t   |
| 6     | uint16_t  |
| 7     | uint32_t  |
| 8     | uint64_t  |

##### Floating-point

| Value | Data Type |
| :---  | :--       |
| 9     | float     |
| 10    | double    |

### Data Section
- Data is stored in interleaved/packed order = RGB RGB RGB, etc.

- Total size of the data in bytes = width * height * channels * [sizeof_data_type_in_bytes](#image-depth).

- It's recommended to read this section in chunks for efficiency, and then reinterpret it as an array of the data type of the [Image depth](#image-depth) of the image.

## FSI v1

### Header Section

| Description                     | Required Value          |   Size                 |
| :---                            |          :---:          |   :---                 |
| Format signature                |           'f'           |   1 byte (char)        |
| Format signature                |           's'           |   1 byte (char)        |
| Format signature                |           'i'           |   1 byte (char)        |
| Format signature                |           'f'           |   1 byte (char)        |
| Format version                  |            1            |   4 bytes (uint32_t)   |
| Image width                     |     (not specified)     |   4 bytes (uint32_t)   |
| Image height                    |     (not specified)     |   4 bytes (uint32_t)   |
| Image channel count             |     (not specified)     |   4 bytes (uint32_t)   |
| Image depth                     |     (not specified)     |   4 bytes (uint32_t)   |

#### Image depth

##### Signed

| Value | Data Type |
| :---  | :--       |
| 1     | int8_t    |
| 2     | int16_t   |
| 3     | int32_t   |
| 4     | int64_t   |

##### Unsigned Integers

| Value | Data Type |
| :---  | :--       |
| 5     | uint8_t   |
| 6     | uint16_t  |
| 7     | uint32_t  |
| 8     | uint64_t  |

##### Floating-point

| Value | Data Type |
| :---  | :--       |
| 9     | float     |
| 10    | double    |

### Data Section
- Data is stored in interleaved/packed order = RGB RGB RGB, etc.

- Total size of the data in bytes = width * height * channels * sizeof_data_type_in_bytes.

- It's recommended to read this section in chunks for efficiency, and then reinterpret it as an array of the data type of the Image depth of the image.
