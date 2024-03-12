# FSI IO
Official repo for the FSI file format specification and basic IO implementation written in C++17

## FSI v1 File Format Specification
### Header
| Description                     | Required Value          |   Size                 |
| :---                            |          :---:          |   :---                 |
| Format signature                |           'f'           |   1 byte (char)        |
| Format signature                |           's'           |   1 byte (char)        |
| Format signature                |           'i'           |   1 byte (char)        |
| Format signature                |           'f'           |   1 byte (char)        |
| Format version                  |            1            |   4 bytes (uint32_t)   |
