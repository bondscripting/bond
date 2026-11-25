# Compiled Bond Object (CBO) File Format

A CBO file holds the compiled form of one or more Bond translation units: the functions and global
data they define, and the constants they reference. `bondc` writes CBO files, `CboLoader` loads
them into the VM, and `bondd` disassembles them.

The bytecode inside function blobs is described in [`instruction-set.md`](instruction-set.md). The
authoritative implementations of the format are `GeneratorCore::WriteCbo` in
[`source/codegenerator.cpp`](../source/codegenerator.cpp), the validator in
[`source/cbovalidator.cpp`](../source/cbovalidator.cpp) and the loader in
[`source/cboloader.cpp`](../source/cboloader.cpp).

## 1. Conventions

- All multi-byte values, including bytecode operands, are little endian.
- Fields are packed with no padding. The only alignment padding in the format is inside the
  `lookupswitch` and `tableswitch` operands in bytecode.
- `u8`, `u16`, `u32`, `u64` are unsigned integers and `i16`, `i32` are two's complement signed
  integers of the given number of bits.
- Every index into a constant table is a `u16` and is local to the file that contains it.

## 2. File layout

```
Header
ConstantTable
Blob            (the root blob, a List blob in files written by bondc)
```

The file must end exactly where the root blob ends.

## 3. Header

| Offset | Type | Field | Description |
|---:|---|---|---|
| 0 | `u32` | `magicNumber` | `0x57EDFA57` (bytes `57 FA ED 57`). |
| 4 | `u16` | `majorVersion` | Currently 1. |
| 6 | `u16` | `minorVersion` | Currently 0. |
| 8 | `u16` | `flags` | Bit 0: set if the file was compiled for 64-bit pointers, clear for 32-bit pointers. The other bits are reserved and written as 0. |

A file can be loaded if its major version is in the range supported by the VM (`version.h`),
whatever its minor version. The loader also rejects a file whose pointer size differs from the
host's.

## 4. Constant table

| Type | Field | Description |
|---|---|---|
| `u32` | `size` | Size of the constant table in bytes, including this field. |
| `u16` | `value32Count` | |
| `u16` | `value64Count` | |
| `u16` | `stringCount` | |
| `u16` | `qualifiedNameCount` | |
| `u32[value32Count]` | `value32Table` | 32-bit constants: integers, floats and the sizes of block memory operations. |
| `u64[value64Count]` | `value64Table` | 64-bit constants: longs and doubles. |
| `String[stringCount]` | `stringTable` | |
| `QualifiedName[qualifiedNameCount]` | `qualifiedNameTable` | |

The value tables hold raw bits. Their interpretation depends on the instruction or blob that
references them.

**String**: a `u16 length` followed by `length` bytes. There is no NUL terminator. The bytes are
UTF-8 by convention, but the format doesn't enforce it.

**QualifiedName**: a `u16 elementCount` followed by `elementCount` `u16` string indices, from the
outermost scope inwards. For example, `Std::Print` is `[index of "Std", index of "Print"]`. Two
forms are special:

- A qualified name with zero elements names the **static initializer** of a file.
- The getter and setter that `bondc` generates for a native struct member add a final `@get` or
  `@set` element to the member's name.

## 5. SizeAndType

A `u32` that packs a type's size in bytes and its signature type:

```
type = value & 0xF
size = value >> 4
```

| Value | Signature type | | Value | Signature type |
|---:|---|---|---:|---|
| 0 | `void` | | 7 | `uint` |
| 1 | `bool` | | 8 | `long` |
| 2 | `char` | | 9 | `ulong` |
| 3 | `uchar` | | 10 | `float` |
| 4 | `short` | | 11 | `double` |
| 5 | `ushort` | | 12 | pointer |
| 6 | `int` | | 13 | aggregate (struct passed by value) |

The values follow the order of `BOND_SIGNATURE_TYPE_LIST` in
[`include/bond/types/types.h`](../include/bond/types/types.h). Values 14 and 15 are invalid.

The size must match the type: 0 for `void`, 1 for `bool`, `char` and `uchar`, 2 for `short` and
`ushort`, 4 for `int`, `uint` and `float`, 8 for `long`, `ulong` and `double`, and the pointer size
given by the header flags for pointers. Only an aggregate can have any size.

## 6. Blobs

Everything after the constant table is a tree of blobs. Every blob starts with the same header:

| Type | Field | Description |
|---|---|---|
| `u32` | `blobSize` | Size of the blob in bytes, including this header. |
| `u16` | `idIndex` | String index of the blob's four-character kind: `"List"`, `"Func"` or `"Data"`. |

The kind is identified by the content of the string, not by the index. A parser must skip a blob of
unknown kind by advancing to `blobStart + blobSize`. The contents of every known blob must end
exactly at `blobStart + blobSize`.

### List blob (`"List"`)

| Type | Field | Description |
|---|---|---|
| `u32` | `blobCount` | |
| `Blob[blobCount]` | `blobs` | |

`bondc` writes a single root List blob that contains all of the file's Func blobs followed by all
of its Data blobs.

### Func blob (`"Func"`)

| Type | Field | Description |
|---|---|---|
| `u16` | `nameIndex` | Qualified name index of the function. |
| `u32` | `returnType` | SizeAndType of the return value. |
| `u16` | `paramCount` | |
| `ParamSignature[paramCount]` | `params` | |
| `u32` | `frameSize` | Bytes occupied by the parameters below the frame pointer, each aligned. |
| `u32` | `packedFrameSize` | Bytes occupied by the parameters when pushed in consecutive slots. |
| `u32` | `localSize` | Bytes of local variables above the frame pointer. |
| `u32` | `stackSize` | Maximum depth of the operand stack in bytes. |
| `u32` | `framePointerAlignment` | Required alignment of the frame pointer. |
| `u32` | `codeSize` | |
| `u8[codeSize]` | `code` | Bytecode. |
| `u16` | `extCount` | Number of entries in the extended instruction table. 0 for a native function. |
| `ExtEntry[extCount]` | `extTable` | The function's extended instruction table. |
| `Blob` | `metadata` | Optional. See [Metadata](#metadata). |

**ParamSignature**: an `i32 framePointerOffset` followed by a `u32` SizeAndType. Offsets are
negative and strictly decreasing. A member function has an implicit `this` pointer parameter at
offset -8 that is listed first. See
[Stack frames](instruction-set.md#stack-frames) for how the offsets are assigned.

`frameSize`, `packedFrameSize` and `localSize` are multiples of 8 (the slot size), and
`packedFrameSize <= frameSize`. `framePointerAlignment` is a power of two of at least 8.

**ExtEntry**: a `u16 opCode` followed by an `i32 operand`, 6 bytes. The
[`ext`](instruction-set.md#ext) instruction executes an entry: the opcode with the 32-bit operand
in place of its 16-bit one. The opcode must be a branch with a 16-bit offset or `loadfp`. For a
branch, the operand is an offset relative to the end of the `ext` instruction. Values of 256 and
above are reserved for a secondary opcode space and are rejected for now. `bondc` writes each
distinct entry once per function.

A function with `codeSize == 0` is **native**: the host must bind an implementation to its name
before the code is run. A function whose name is the empty qualified name is the file's **static
initializer**, which the host runs once after loading, before any other code
(`CodeSegment::CallStaticInitializers`). `bondc` omits the static initializer when it has no code.

**Bytecode** is a sequence of instructions, each a one-byte opcode followed by operands whose
layout depends on the opcode. Opcode values are listed in the
[opcode table](instruction-set.md#4-appendix-opcode-table) and operand kinds in
[Notation and encoding](instruction-set.md#2-notation-and-encoding). Two details matter to a parser:

- Branch targets are positions within `code`, not file offsets. A branch offset is relative to the
  end of its operand, and a switch offset to the end of its jump table.
- The padding in `lookupswitch` and `tableswitch` aligns to a multiple of 4 bytes from the start of
  `code`, not from the start of the file. The loader places each function's code at a 4-byte aligned
  address. See [Switches](instruction-set.md#318-switches).

### Data blob (`"Data"`)

| Type | Field | Description |
|---|---|---|
| `u16` | `nameIndex` | Qualified name index of the global. |
| `u32` | `type` | SizeAndType of the global. |
| `u32` | `payload` | Depends on the type. See below. |
| `Blob` | `metadata` | Optional. See [Metadata](#metadata). |

| Type | Payload |
|---|---|
| `bool`, `char`, `uchar`, `short`, `ushort` | Initial value, truncated to the type's size. 0 if none. |
| `int`, `uint`, `float` | Initial value as raw 32 bits. 0 if none. |
| `long`, `ulong`, `double` | Index into `value64Table` of the initial value, or `0xFFFFFFFF` to initialize to zero. |
| `void`, pointer, aggregate | Required alignment of the data in bytes, a power of two. The data is zero initialized. |

Every global is aligned to at least 8 bytes (the slot size), whatever its type.

### Metadata

A Func or Data blob may end with a single nested blob of metadata. It is present when bytes remain
before the end of the enclosing blob. No metadata blob kinds are defined yet, so a parser should
skip it as an unknown blob. A line number table for Func blobs is planned.

## 7. Loading

The `CboLoader` loads several CBO files into one code segment:

- Function and data names are resolved across all loaded files. A name defined in more than one file
  is an error, as is a reference to a name that no file defines.
- The loader rewrites each `name index` operand in the bytecode to the index of the referenced
  function or data entry in the code segment. Bytecode in a file always holds qualified name
  indices.

## 8. Example

This script, compiled with `bondc -p64`:

```
namespace Ex
{
	const int G = 7;
	int Add(int a, int b) { return a + b; }
}
```

produces this 152-byte file:

```
Offset  Bytes                     Meaning
------  ------------------------  ------------------------------------------------------------
Header
0x00    57 FA ED 57               magicNumber
0x04    01 00                     majorVersion = 1
0x06    00 00                     minorVersion = 0
0x08    01 00                     flags: 64-bit pointers
Constant table
0x0A    38 00 00 00               size = 56 (table ends at 0x42)
0x0E    00 00                     value32Count = 0
0x10    00 00                     value64Count = 0
0x12    06 00                     stringCount = 6
0x14    03 00                     qualifiedNameCount = 3
0x16    01 00 47                  string 0: "G"
0x19    02 00 45 78               string 1: "Ex"
0x1D    03 00 41 64 64            string 2: "Add"
0x22    04 00 4C 69 73 74         string 3: "List"
0x28    04 00 46 75 6E 63         string 4: "Func"
0x2E    04 00 44 61 74 61         string 5: "Data"
0x34    00 00                     qualified name 0: (empty; static initializer)
0x36    02 00 01 00 00 00         qualified name 1: Ex.G
0x3C    02 00 01 00 02 00         qualified name 2: Ex.Add
Root List blob
0x42    56 00 00 00               blobSize = 86 (ends at 0x98, end of file)
0x46    03 00                     idIndex = 3 ("List")
0x48    02 00 00 00               blobCount = 2
Func blob
0x4C    3C 00 00 00               blobSize = 60 (ends at 0x88)
0x50    04 00                     idIndex = 4 ("Func")
0x52    02 00                     nameIndex = 2 (Ex.Add)
0x54    46 00 00 00               returnType: size 4, type 6 (int)
0x58    02 00                     paramCount = 2
0x5A    F8 FF FF FF 46 00 00 00   param a: offset -8, int
0x62    F0 FF FF FF 46 00 00 00   param b: offset -16, int
0x6A    10 00 00 00               frameSize = 16
0x6E    10 00 00 00               packedFrameSize = 16
0x72    00 00 00 00               localSize = 0
0x76    10 00 00 00               stackSize = 16
0x7A    08 00 00 00               framePointerAlignment = 8
0x7E    04 00 00 00               codeSize = 4
0x82    3C 3D 86 E2               push32_p0, push32_p1, addi, return32
0x86    00 00                     extCount = 0
Data blob
0x88    10 00 00 00               blobSize = 16 (ends at 0x98)
0x8C    05 00                     idIndex = 5 ("Data")
0x8E    01 00                     nameIndex = 1 (Ex.G)
0x90    46 00 00 00               type: size 4, type 6 (int)
0x94    07 00 00 00               payload: initial value 7
```
