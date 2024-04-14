# FAQ
## Specification
#### Q: Why is the thumbnail data section mandatory (always present) even when the byte indicating the presence of thumbnail data is set to false (zero)?
This allows to quickly add, modify or remove a thumbnail from a file without having to alter its structure. Offsetting the image data after prepending thumbnail data virtually means rewriting the entire file.

#### Q: Why is the thumbnail data section before the image data section and not at the end of the file?
For programs that require only the thumbnail instead of the image data (e.g., a Windows shell extension), it is more efficient to have this data located before the image data section. Placing the thumbnail data first in the file ensures that it can be easily and efficiently accessed without the need to skip over large portions of the file.
