# pARMesan Documentation

## WHEEL files

The WHEEL file format is pARMesan's equivalent of ELF files. Eventually, we will probably transition over to using ELF files; this depends on progress on the pARMesan compiler. The WHEEL format is essentially a compressed memory image made of several sections called "wedges". Each wedge is an image of a chunk of memory. The current WHEEL format allows for up to 256 wedges each up to 65536 bytes long. Practically speaking, one will never end up using more than 5-6 wedges, and will likely rarely use a wedge occupying more than a couple kilobytes or so.

Note that the WHEEL file format does not specify what is to be done with the rest of the memory addressable by the processor. That is up to the implementation of the simulator. The WHEEL file may also contain bytes over ranges of memory that are mapped to MMIO on the CPU; this simply means those bytes will never be accessed by the CPU, since those addresses will instead be routed to the MMIO simulation system.

## File Format

### WHEEL Header

The WHEEL header contains a few parts, laid out in the following order:

- Magic Identifier: A 4-byte field containing the bytes `0x77`, `0x68`, `0x65`, and `0x65` in that order. This corresponds to the string `"whee"`, but note the lack of null-terminator.
- Version: A 1-byte field containing the WHEEL version number of this file. The loader should check this field value against the configured WHEEL version of the loader to ensure compatibility.
- Number of Segments: A 1-byte field containing the number of wedges this WHEEL file contains.
- Reserved: A 2-byte field, currently reserved for future use.

The total size of the WHEEL header is 8 bytes. 

Following the WHEEL header is a series of (wedge header, wedge data) pairs. Each wedge header is encoded as follows.

### Wedge Header

The wedge header contains a few parts, laid out in the following order:

- Start Address: A 2-byte field containing the memory address to start loading this wedge into.
- Length: A 2-byte field containing the number of bytes in this wedge (not including the bytes of this header).
- Checksum: A 4-byte field that can be used to encode a checksum for the wedge. The checksum algorithm in use depends on the WHEEL version in use. For WHEEL version 1, there is no defined checksum algorithm, and the checksum field is summarily ignored.

The total size of the wedge header is 8 bytes.

Following each wedge header is a sequence of bytes with length equal to the `length` field of the wedge header. This sequence is to be loaded into memory starting at the address specified in the `start_address` field of the wedge header.

---

[Table of Contents](index.md)