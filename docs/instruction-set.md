# Bond Virtual Machine Instruction Set

This document describes every instruction understood by the Bond Virtual Machine: what it does,
how it is encoded, what its operands mean, and how it changes the operand stack.

The authoritative list of opcodes is the `BOND_OPCODE_LIST` X-macro in
[`include/bond/types/opcodes.h`](../include/bond/types/opcodes.h). The semantics described here are
those implemented by `VM::ExecuteScriptFunction` in [`source/vm.cpp`](../source/vm.cpp). The
container format that holds the bytecode is described in [`cbo-file-format.md`](cbo-file-format.md).

## Contents

1. [Execution model](#1-execution-model)
2. [Notation and encoding](#2-notation-and-encoding)
3. [Instruction reference](#3-instruction-reference)
   - [Constants](#31-constants)
   - [Addresses](#32-addresses)
   - [Loads through an address](#33-loads-through-an-address)
   - [Stores through an address](#34-stores-through-an-address)
   - [Pushing frame values](#35-pushing-frame-values)
   - [Popping into the frame](#36-popping-into-the-frame)
   - [Block memory operations](#37-block-memory-operations)
   - [Stack manipulation](#38-stack-manipulation)
   - [Type conversions](#39-type-conversions)
   - [Pointer arithmetic](#310-pointer-arithmetic)
   - [In-place increment](#311-in-place-increment)
   - [Arithmetic](#312-arithmetic)
   - [Shifts and bitwise logic](#313-shifts-and-bitwise-logic)
   - [Negation and logical not](#314-negation-and-logical-not)
   - [Comparisons](#315-comparisons)
   - [Conditional branches](#316-conditional-branches)
   - [Unconditional branches](#317-unconditional-branches)
   - [Switches](#318-switches)
   - [Function invocation](#319-function-invocation)
   - [Function return](#320-function-return)
4. [Appendix: opcode table](#4-appendix-opcode-table)

---

## 1. Execution model

The Bond VM is a stack machine. Each function invocation executes with three pieces of state:

- **`pc`**: the program counter, a byte offset into the function's code. Execution begins at 0.
- **`fp`**: the frame pointer. Parameters and locals are addressed relative to it.
- **`sp`**: the operand stack pointer. It points one past the top slot of the operand stack.

### Slots

Every entry on the operand stack occupies one **slot** of `BOND_SLOT_SIZE` (8) bytes, regardless
of its type. Values narrower than 32 bits are always widened to 32 bits on the stack. A 32-bit value
occupies the low-addressed 4 bytes of its slot. The upper 4 bytes are unspecified. The stack grows
toward higher addresses.

The exception is a struct value pushed by [`loadmemw`](#loadmemw), which occupies as many
consecutive slots as needed to hold it.

Addresses are pointer-sized: 4 bytes when compiling for 32-bit pointers, 8 bytes for 64-bit
pointers (`bondc -p32` / `-p64`, defaulting to the host). The compiler moves addresses with the
`*32` or `*64` instructions to match.

### Stack frames

The memory for a function invocation is laid out as follows, from lower to higher addresses:

```
              ┌───────────────────────────┐
              │  caller's operand stack   │
              ├───────────────────────────┤
              │  ...                      │
   fp - 24    │  parameter 2      (_p2)   │   Parameters: pushed by the caller,
   fp - 16    │  parameter 1      (_p1)   │   at negative offsets from fp
   fp -  8    │  parameter 0      (_p0)   │
   fp ──────► ├───────────────────────────┤
   fp +  0    │  local slot 0     (_l0)   │   Locals: at non-negative offsets
   fp +  8    │  local slot 1     (_l1)   │   from fp, localSize bytes in total
              │  ...                      │
              ├───────────────────────────┤ ◄── fp + localSize, rounded up to a slot
              │  operand stack            │
              │  ▼ grows toward higher    │
              │    addresses              │
              └───────────────────────────┘
```

- **Parameters** are at negative offsets. Parameter 0 is closest to `fp`. Each parameter's offset
  is the negated running total of the parameter sizes, each total rounded up to the parameter's
  alignment or 8, whichever is larger. A list of `int` parameters therefore lands at `fp - 8`,
  `fp - 16`, `fp - 24`, and so on.
- **Member functions** have an implicit `this` pointer at `fp - 8`, and their declared parameters
  start at `fp - 16`.
- **Locals** are allocated at non-negative offsets, each rounded up to a slot boundary.
- The **operand stack** of the invocation begins at `fp + localSize`, rounded up to a slot.

The `_p0`–`_p3` and `_l0`–`_l3` instruction variants are one-byte shortcuts for the first four
parameter slots and the first four local slots. See [Function invocation](#319-function-invocation)
for how the caller sets up the parameters.

### Runtime checking

The VM does not verify types, alignment, memory bounds, stack underflow, or arithmetic exceptions
at run time. Bytecode that misuses an instruction (for example, dividing by zero or loading through
an invalid address) has undefined behaviour on the host, typically a crash. The only check is for
stack overflow. When the VM sets up an invocation's frame, it checks that the locals and the
function's maximum operand stack depth (the `stackSize` recorded in the CBO) fit in the remaining
stack space.

---

## 2. Notation and encoding

### Instruction format

An instruction is a one-byte opcode followed by zero or more bytes of **operands** encoded in the
bytecode stream. This document uses *operand* for values in the instruction stream, and *stack
value* for values on the operand stack.

### Operand kinds

| Operand kind | Bytes | Meaning |
|---|---:|---|
| *(none)* | 0 | The instruction has no operands. |
| `char` | 1 | Signed 8-bit immediate. |
| `uchar` | 1 | Unsigned 8-bit immediate. |
| `char, char` | 2 | Two signed 8-bit immediates. Used only by [`inci`/`incl`](#inci-incl). |
| `short` | 2 | Signed 16-bit immediate. |
| `ushort` | 2 | Unsigned 16-bit immediate. |
| `int index` | 2 | Unsigned 16-bit index into the function's **value32 table**. The entry is read as a signed 32-bit integer. |
| `value32 index` | 2 | Unsigned 16-bit index into the value32 table. The entry is used as raw 32 bits. |
| `value64 index` | 2 | Unsigned 16-bit index into the **value64 table**. The entry is used as raw 64 bits. |
| `off16` | 2 | Signed 16-bit branch offset, relative to the address immediately following the operand. |
| `off32 index` | 2 | Unsigned 16-bit index into the value32 table. The entry is a signed 32-bit branch offset, relative to the address immediately following the operand. |
| `string index` | 2 | Unsigned 16-bit index into the **string table**. |
| `name index` | 2 | Unsigned 16-bit index. In a CBO file it indexes the **qualified name table**. When the CBO is loaded, the `CboLoader` rewrites it as the index of the referenced data entry or function in the `CodeSegment`. |
| `lookupswitch table`, `tableswitch table` | variable | Padding to a 4-byte boundary, followed by an inline jump table. See [Switches](#318-switches). |

The value32, value64, string and qualified name tables form the constant table of the CBO file
that contains the function.

Multi-byte operands are stored in little-endian byte order in CBO files. Bond only supports
little-endian targets (`include/bond/conf.h` rejects others at compile time), so the VM reads
operands from the loaded bytecode without conversion.

### Wide forms

Many instructions come in a narrow and a wide (`w`-suffixed) form. The narrow form encodes its
value as a `short` immediate. The wide form encodes an `int index` (or an `off32 index` for
branches) that refers to a full 32-bit value in the value32 table. Both forms are 3 bytes long. The
compiler emits the wide form only when the value doesn't fit in 16 bits. The wide opcode is always
the narrow opcode plus one. `loadmemw`, `memcopyw`, `memzerow` and `returnmemw` exist only in the
wide form.

### Stack notation

Stack effects are written in the form

```
..., value1, value2 → ..., result
```

where the top of the stack is on the right. `...` stands for the part of the stack that the
instruction doesn't touch. The stack value types are:

| Type | Meaning |
|---|---|
| `int`, `uint` | 32-bit signed or unsigned integer. `bool`, `char`, `uchar`, `short` and `ushort` values are widened to these on the stack. |
| `long`, `ulong` | 64-bit signed or unsigned integer. |
| `float`, `double` | 32-bit or 64-bit IEEE 754 floating point number. |
| `address` | A pointer-sized memory address. |
| `value32`, `value64` | Any 32-bit or 64-bit value whose interpretation doesn't matter to the instruction. |
| `slot` | Any single slot, regardless of type. |

### Entry format

Each instruction entry in the reference gives:

- **Format**: the mnemonic and the operands, written as `name:kind`.
- **Operands**: what each operand means.
- **Stack**: the stack transition.

Families of instructions that differ only in a built-in constant or a type are documented in one
entry, with a table that describes each opcode.

---

## 3. Instruction reference

### 3.1 Constants

<a id="nop"></a>
#### `nop`

Does nothing.

| | |
|---|---|
| **Format** | `nop` |
| **Stack** | `... → ...` |

---

<a id="constc-constuc"></a>
#### `constc`, `constuc`

Push an 8-bit immediate constant.

| | |
|---|---|
| **Format** | `constc value:char` · `constuc value:uchar` |
| **Operands** | `value`: the constant. |
| **Stack** | `constc`: `... → ..., int` · `constuc`: `... → ..., uint` |

`constc` sign-extends `value` to 32 bits, and `constuc` zero-extends it.

---

<a id="consts-constus"></a>
#### `consts`, `constus`

Push a 16-bit immediate constant.

| | |
|---|---|
| **Format** | `consts value:short` · `constus value:ushort` |
| **Operands** | `value`: the constant. |
| **Stack** | `consts`: `... → ..., int` · `constus`: `... → ..., uint` |

`consts` sign-extends `value` to 32 bits, and `constus` zero-extends it.

---

<a id="const32-const64"></a>
#### `const32`, `const64`

Push a constant from the constant table.

| | |
|---|---|
| **Format** | `const32 index:value32 index` · `const64 index:value64 index` |
| **Operands** | `index`: the index of the constant in the value32 or value64 table. |
| **Stack** | `const32`: `... → ..., value32` · `const64`: `... → ..., value64` |

The bits of the table entry are pushed unchanged. The compiler uses these instructions for
`int`, `uint` and `float` constants (`const32`) and `long`, `ulong` and `double` constants
(`const64`) that have no shorter encoding.

---

<a id="consti"></a>
#### `consti_n2` … `consti_8`, `constl_n2` … `constl_8`

Push a small built-in integer constant.

| | |
|---|---|
| **Format** | `consti_<k>`, `constl_<k>` (no operands) |
| **Stack** | `consti_<k>`: `... → ..., int` · `constl_<k>`: `... → ..., long` |

| `int` opcode | `long` opcode | Pushes |
|---|---|---:|
| `consti_n2` | `constl_n2` | −2 |
| `consti_n1` | `constl_n1` | −1 |
| `consti_0` | `constl_0` | 0 |
| `consti_1` | `constl_1` | 1 |
| `consti_2` | `constl_2` | 2 |
| `consti_3` | `constl_3` | 3 |
| `consti_4` | `constl_4` | 4 |
| `consti_8` | `constl_8` | 8 |

The same opcodes are used for `uint` and `ulong` constants, whose bits are identical.

---

<a id="constf"></a>
#### `constf_n2` … `constf_2`, `constd_n2` … `constd_2`

Push a small built-in floating point constant.

| | |
|---|---|
| **Format** | `constf_<k>`, `constd_<k>` (no operands) |
| **Stack** | `constf_<k>`: `... → ..., float` · `constd_<k>`: `... → ..., double` |

| `float` opcode | `double` opcode | Pushes |
|---|---|---:|
| `constf_n2` | `constd_n2` | −2.0 |
| `constf_n1` | `constd_n1` | −1.0 |
| `constf_nh` | `constd_nh` | −0.5 |
| `constf_0` | `constd_0` | 0.0 |
| `constf_h` | `constd_h` | 0.5 |
| `constf_1` | `constd_1` | 1.0 |
| `constf_2` | `constd_2` | 2.0 |

---

### 3.2 Addresses

<a id="loadfp"></a>
#### `loadfp`, `loadfpw`

Push the address of a parameter or local.

| | |
|---|---|
| **Format** | `loadfp offset:short` · `loadfpw index:int index` |
| **Operands** | `offset`: signed byte offset from `fp`. `index`: value32 table index of the offset. |
| **Stack** | `... → ..., address` |

Pushes `fp + offset`. A negative offset addresses a parameter and a non-negative one addresses a
local. The compiler uses it to take the address of a variable, to access struct parameters and
locals, and to pass the address of a return buffer to a function that returns a struct.

---

<a id="loadea"></a>
#### `loadea`

Push the address of a global data entry.

| | |
|---|---|
| **Format** | `loadea name:name index` |
| **Operands** | `name`: the global variable or constant. In the CBO file it is an index into the qualified name table. The loader replaces it with the index of the data entry in the `CodeSegment`. |
| **Stack** | `... → ..., address` |

Loading fails with an error if the name can't be resolved to a data entry.

---

<a id="loadstr"></a>
#### `loadstr`

Push the address of a string constant.

| | |
|---|---|
| **Format** | `loadstr index:string index` |
| **Operands** | `index`: index of the string in the string table. |
| **Stack** | `... → ..., address` |

Pushes the address of the first character of the string. The loader null-terminates every string
in the table, so the result is a valid C string. String constants must not be modified.

---

### 3.3 Loads through an address

<a id="load"></a>
#### `loadc`, `loaduc`, `loads`, `loadus`, `load32`, `load64`

Replace an address with the value stored at that address.

| | |
|---|---|
| **Format** | `loadc`, `loaduc`, `loads`, `loadus`, `load32`, `load64` (no operands) |
| **Stack** | `..., address → ..., value` |

| Opcode | Reads | Pushes |
|---|---|---|
| `loadc` | signed 8-bit value | `int`, sign-extended |
| `loaduc` | unsigned 8-bit value | `uint`, zero-extended |
| `loads` | signed 16-bit value | `int`, sign-extended |
| `loadus` | unsigned 16-bit value | `uint`, zero-extended |
| `load32` | 32-bit value | `value32` |
| `load64` | 64-bit value | `value64` |

`bool` values are loaded with `loaduc`. Addresses are loaded with `load32` or `load64` depending on
the pointer size. The address must be suitably aligned for the type.

---

<a id="loadmemw"></a>
#### `loadmemw`

Replace an address with a copy of the block of memory at that address.

| | |
|---|---|
| **Format** | `loadmemw size:int index` |
| **Operands** | `size`: value32 table index of the number of bytes to copy. |
| **Stack** | `..., address → ..., value…` |

Copies exactly `size` bytes from `address` onto the operand stack, starting at the slot that held
the address. The result occupies `ceil(size / 8)` slots. Any remaining bytes of the last slot are
unspecified. The compiler uses `loadmemw` to push a struct by value, for example as a function
argument, and passes the size of the struct.

> **Note:** The stack delta recorded for `loadmemw` in `opcodes.h` is 0, which is exact for structs
> of at most 8 bytes. The real change is `ceil(size / 8) − 1` slots, and the code generator accounts
> for the additional slots itself.

---

### 3.4 Stores through an address

<a id="store"></a>
#### `storec`, `stores`, `store32`, `store64`

Store a value at an address.

| | |
|---|---|
| **Format** | `storec`, `stores`, `store32`, `store64` (no operands) |
| **Stack** | `..., address, value → ...` |

| Opcode | Stack value | Writes |
|---|---|---|
| `storec` | `int` | the low 8 bits |
| `stores` | `int` | the low 16 bits |
| `store32` | `value32` | 32 bits |
| `store64` | `value64` | 64 bits |

There are no unsigned variants because truncation produces the same bits for signed and unsigned
values.

---

### 3.5 Pushing frame values

These instructions read a parameter or local directly from the frame and push it. They combine
[`loadfp`](#loadfp) and a [load](#load) into a single instruction.

<a id="push-narrow"></a>
#### `pushc`, `pushcw`, `pushuc`, `pushucw`, `pushs`, `pushsw`, `pushus`, `pushusw`

Push a narrow integer from the frame, widened to 32 bits.

| | |
|---|---|
| **Format** | `push<t> offset:short` · `push<t>w index:int index` |
| **Operands** | `offset`: signed byte offset from `fp`. `index`: value32 table index of the offset. |
| **Stack** | `... → ..., int` (`pushc`, `pushs`) · `... → ..., uint` (`pushuc`, `pushus`) |

| Opcodes | Reads at `fp + offset` | Pushes |
|---|---|---|
| `pushc`, `pushcw` | signed 8-bit value | `int`, sign-extended |
| `pushuc`, `pushucw` | unsigned 8-bit value | `uint`, zero-extended |
| `pushs`, `pushsw` | signed 16-bit value | `int`, sign-extended |
| `pushus`, `pushusw` | unsigned 16-bit value | `uint`, zero-extended |

---

<a id="push32"></a>
#### `push32`, `push32w`, `push32_p0` … `push32_p3`, `push32_l0` … `push32_l3`

Push a 32-bit value from the frame.

| | |
|---|---|
| **Format** | `push32 offset:short` · `push32w index:int index` · `push32_p<n>` · `push32_l<n>` |
| **Operands** | `offset`: signed byte offset from `fp`. `index`: value32 table index of the offset. |
| **Stack** | `... → ..., value32` |

| Opcode | Reads the 32-bit value at |
|---|---|
| `push32`, `push32w` | `fp + offset` |
| `push32_p0` | `fp − 8` (parameter slot 0) |
| `push32_p1` | `fp − 16` (parameter slot 1) |
| `push32_p2` | `fp − 24` (parameter slot 2) |
| `push32_p3` | `fp − 32` (parameter slot 3) |
| `push32_l0` | `fp + 0` (local slot 0) |
| `push32_l1` | `fp + 8` (local slot 1) |
| `push32_l2` | `fp + 16` (local slot 2) |
| `push32_l3` | `fp + 24` (local slot 3) |

Used for `int`, `uint` and `float` variables, and for addresses when compiling for 32-bit
pointers.

---

<a id="push64"></a>
#### `push64`, `push64w`, `push64_p0` … `push64_p3`, `push64_l0` … `push64_l3`

Push a 64-bit value from the frame.

| | |
|---|---|
| **Format** | `push64 offset:short` · `push64w index:int index` · `push64_p<n>` · `push64_l<n>` |
| **Operands** | `offset`: signed byte offset from `fp`. `index`: value32 table index of the offset. |
| **Stack** | `... → ..., value64` |

| Opcode | Reads the 64-bit value at |
|---|---|
| `push64`, `push64w` | `fp + offset` |
| `push64_p0` | `fp − 8` (parameter slot 0) |
| `push64_p1` | `fp − 16` (parameter slot 1) |
| `push64_p2` | `fp − 24` (parameter slot 2) |
| `push64_p3` | `fp − 32` (parameter slot 3) |
| `push64_l0` | `fp + 0` (local slot 0) |
| `push64_l1` | `fp + 8` (local slot 1) |
| `push64_l2` | `fp + 16` (local slot 2) |
| `push64_l3` | `fp + 24` (local slot 3) |

Used for `long`, `ulong` and `double` variables, and for addresses when compiling for 64-bit
pointers.

---

### 3.6 Popping into the frame

<a id="pop"></a>
#### `pop`

Discard the top slot of the stack.

| | |
|---|---|
| **Format** | `pop` |
| **Stack** | `..., slot → ...` |

Used to discard the value of an expression statement.

---

<a id="pop-narrow"></a>
#### `popc`, `popcw`, `pops`, `popsw`

Pop an integer and store it, truncated, in the frame.

| | |
|---|---|
| **Format** | `popc offset:short` · `popcw index:int index` · `pops offset:short` · `popsw index:int index` |
| **Operands** | `offset`: signed byte offset from `fp`. `index`: value32 table index of the offset. |
| **Stack** | `..., int → ...` |

`popc` and `popcw` write the low 8 bits of the value to `fp + offset`. `pops` and `popsw` write the
low 16 bits. They are used for both signed and unsigned narrow types.

---

<a id="pop32"></a>
#### `pop32`, `pop32w`, `pop32_p0` … `pop32_p3`, `pop32_l0` … `pop32_l3`

Pop a 32-bit value and store it in the frame.

| | |
|---|---|
| **Format** | `pop32 offset:short` · `pop32w index:int index` · `pop32_p<n>` · `pop32_l<n>` |
| **Operands** | `offset`: signed byte offset from `fp`. `index`: value32 table index of the offset. |
| **Stack** | `..., value32 → ...` |

| Opcode | Writes the 32-bit value to |
|---|---|
| `pop32`, `pop32w` | `fp + offset` |
| `pop32_p0` | `fp − 8` (parameter slot 0) |
| `pop32_p1` | `fp − 16` (parameter slot 1) |
| `pop32_p2` | `fp − 24` (parameter slot 2) |
| `pop32_p3` | `fp − 32` (parameter slot 3) |
| `pop32_l0` | `fp + 0` (local slot 0) |
| `pop32_l1` | `fp + 8` (local slot 1) |
| `pop32_l2` | `fp + 16` (local slot 2) |
| `pop32_l3` | `fp + 24` (local slot 3) |

---

<a id="pop64"></a>
#### `pop64`, `pop64w`, `pop64_p0` … `pop64_p3`, `pop64_l0` … `pop64_l3`

Pop a 64-bit value and store it in the frame.

| | |
|---|---|
| **Format** | `pop64 offset:short` · `pop64w index:int index` · `pop64_p<n>` · `pop64_l<n>` |
| **Operands** | `offset`: signed byte offset from `fp`. `index`: value32 table index of the offset. |
| **Stack** | `..., value64 → ...` |

| Opcode | Writes the 64-bit value to |
|---|---|
| `pop64`, `pop64w` | `fp + offset` |
| `pop64_p0` | `fp − 8` (parameter slot 0) |
| `pop64_p1` | `fp − 16` (parameter slot 1) |
| `pop64_p2` | `fp − 24` (parameter slot 2) |
| `pop64_p3` | `fp − 32` (parameter slot 3) |
| `pop64_l0` | `fp + 0` (local slot 0) |
| `pop64_l1` | `fp + 8` (local slot 1) |
| `pop64_l2` | `fp + 16` (local slot 2) |
| `pop64_l3` | `fp + 24` (local slot 3) |

---

### 3.7 Block memory operations

<a id="memcopyw"></a>
#### `memcopyw`

Copy a block of memory.

| | |
|---|---|
| **Format** | `memcopyw size:int index` |
| **Operands** | `size`: value32 table index of the number of bytes to copy. |
| **Stack** | `..., destAddress, sourceAddress → ...` |

Copies `size` bytes from `sourceAddress` to `destAddress`. The two blocks must not overlap. The
compiler uses it for struct assignment, struct initialization, and to copy a struct returned by a
function into its destination.

---

<a id="memzerow"></a>
#### `memzerow`

Zero a block of memory.

| | |
|---|---|
| **Format** | `memzerow size:int index` |
| **Operands** | `size`: value32 table index of the number of bytes to zero. |
| **Stack** | `..., address → ...` |

Sets `size` bytes at `address` to 0. The compiler uses it for empty initializers (`= {}`) and to
zero the remainder of partially initialized arrays and structs.

---

### 3.8 Stack manipulation

These instructions move whole 8-byte slots, so they work on a value of any type that occupies a
single slot.

<a id="dup"></a>
#### `dup`

Duplicate the top slot.

| | |
|---|---|
| **Format** | `dup` |
| **Stack** | `..., a → ..., a, a` |

---

<a id="dupins"></a>
#### `dupins`

Duplicate the top slot and insert the copy below the second slot.

| | |
|---|---|
| **Format** | `dupins` |
| **Stack** | `..., a, b → ..., b, a, b` |

Used when an assignment through an address must also leave the assigned value as the result of the
expression. For example, in `x = *p = v`, the copy of `v` stays below the address and value consumed
by the store.

---

<a id="swap"></a>
#### `swap`

Exchange the top two slots.

| | |
|---|---|
| **Format** | `swap` |
| **Stack** | `..., a, b → ..., b, a` |

---

### 3.9 Type conversions

<a id="conversions"></a>
#### `itoc` … `dtof`

Convert the value on top of the stack to another type, in place.

| | |
|---|---|
| **Format** | the mnemonic alone (no operands) |
| **Stack** | `..., value → ..., result` |

| Opcode | From | To | Conversion |
|---|---|---|---|
| `itoc` | `int` | `int` | Truncate to 8 bits, then sign-extend. |
| `uitouc` | `uint` | `uint` | Truncate to 8 bits, then zero-extend. |
| `itos` | `int` | `int` | Truncate to 16 bits, then sign-extend. |
| `uitous` | `uint` | `uint` | Truncate to 16 bits, then zero-extend. |
| `itol` | `int` | `long` | Sign-extend. |
| `uitoul` | `uint` | `ulong` | Zero-extend. |
| `itof` | `int` | `float` | Round to nearest representable value. |
| `uitof` | `uint` | `float` | Round to nearest representable value. |
| `itod` | `int` | `double` | Exact. |
| `uitod` | `uint` | `double` | Exact. |
| `ltoi` | `long` | `int` | Keep the low 32 bits. |
| `ltof` | `long` | `float` | Round to nearest representable value. |
| `ultof` | `ulong` | `float` | Round to nearest representable value. |
| `ltod` | `long` | `double` | Round to nearest representable value. |
| `ultod` | `ulong` | `double` | Round to nearest representable value. |
| `ftoi` | `float` | `int` | Truncate toward zero. |
| `ftoui` | `float` | `uint` | Truncate toward zero. |
| `ftol` | `float` | `long` | Truncate toward zero. |
| `ftoul` | `float` | `ulong` | Truncate toward zero. |
| `ftod` | `float` | `double` | Exact. |
| `dtoi` | `double` | `int` | Truncate toward zero. |
| `dtoui` | `double` | `uint` | Truncate toward zero. |
| `dtol` | `double` | `long` | Truncate toward zero. |
| `dtoul` | `double` | `ulong` | Truncate toward zero. |
| `dtof` | `double` | `float` | Round to nearest representable value. |

The conversions follow C++ `static_cast` semantics on the host. Converting a floating point value
that is out of range for the destination integer type (including NaN and infinities) is undefined.

Some conversions need no instruction:

- Between signed and unsigned types of the same width, because the bits are unchanged.
- From `long` or `ulong` to `uint`, which uses `ltoi`.
- From `long` to a narrow type, which uses `ltoi` followed by `itoc`, `uitouc`, `itos` or `uitous`.

---

### 3.10 Pointer arithmetic

<a id="ptroff"></a>
#### `ptroffi`, `ptroffl`

Offset an address by a scaled integer.

| | |
|---|---|
| **Format** | `ptroffi elementSize:short` · `ptroffl elementSize:short` |
| **Operands** | `elementSize`: the signed scale factor, normally the size of the pointed-to type. It is negated to subtract an offset from a pointer. |
| **Stack** | `ptroffi`: `..., address, int → ..., address` · `ptroffl`: `..., address, long → ..., address` |

Computes `address + offset × elementSize`. `ptroffi` sign-extends its `int` offset. The compiler
widens `uint` offsets to `long` and uses `ptroffl` instead. When `elementSize` doesn't fit in a
`short`, the compiler emits `muli`/`mull` and `addi`/`addl` instead.

---

<a id="ptrdiff"></a>
#### `ptrdiff`

Compute the number of elements between two addresses.

| | |
|---|---|
| **Format** | `ptrdiff elementSize:short` |
| **Operands** | `elementSize`: the size of the pointed-to type. |
| **Stack** | `..., address1, address2 → ..., int` (32-bit pointers) or `..., address1, address2 → ..., long` (64-bit pointers) |

Computes `(address1 − address2) / elementSize` as a pointer-sized signed integer, truncating toward
zero. The result is exact when both addresses point into the same array. When `elementSize` doesn't
fit in a `short`, the compiler emits `subi`/`subl` followed by `divi`/`divl` instead.

---

### 3.11 In-place increment

<a id="inci-incl"></a>
#### `inci`, `incl`

Add a small constant to a slot in the frame, without touching the operand stack.

| | |
|---|---|
| **Format** | `inci slot:char, delta:char` · `incl slot:char, delta:char` |
| **Operands** | `slot`: the signed slot index relative to `fp`. The value is at `fp + 8 × slot`, so non-negative slots are locals and negative slots are parameters (`−1` is parameter slot 0). `delta`: the signed amount to add. |
| **Stack** | `... → ...` |

`inci` adds the sign-extended `delta` to the 32-bit integer in the slot, and `incl` adds it to the
64-bit integer. Both wrap modulo 2³² or 2⁶⁴.

The compiler uses these instructions for `++`, `--`, and `+=`/`-=` by a constant, applied to a
32-bit or 64-bit integer or pointer variable that starts on a slot boundary of the frame. The
amount added (scaled by the element size for pointers) must fit in a `char`. When the value of the
expression is needed, the compiler pushes the variable before (postfix) or after (prefix) the
increment:

```
// for (int i = 0; i < 10; ++i) ...    (i is local slot 1)
    15: inci        1, 1
```

---

### 3.12 Arithmetic

<a id="add-sub-mul"></a>
#### `addi`, `addl`, `addf`, `addd`, `subi`, `subl`, `subf`, `subd`, `muli`, `mull`, `mulf`, `muld`

Add, subtract or multiply the top two stack values.

| | |
|---|---|
| **Format** | the mnemonic alone (no operands) |
| **Stack** | `..., value1, value2 → ..., result` |

| Operation | `int` | `long` | `float` | `double` |
|---|---|---|---|---|
| `value1 + value2` | `addi` | `addl` | `addf` | `addd` |
| `value1 − value2` | `subi` | `subl` | `subf` | `subd` |
| `value1 × value2` | `muli` | `mull` | `mulf` | `muld` |

The integer forms wrap modulo 2³² or 2⁶⁴ on overflow. Because the resulting bits are the same, they
are also used for `uint` and `ulong`. The floating point forms follow IEEE 754.

---

<a id="div-rem"></a>
#### `divi`, `divui`, `divl`, `divul`, `divf`, `divd`, `remi`, `remui`, `reml`, `remul`

Divide the top two stack values, or compute the remainder.

| | |
|---|---|
| **Format** | the mnemonic alone (no operands) |
| **Stack** | `..., value1, value2 → ..., result` |

| Operation | `int` | `uint` | `long` | `ulong` | `float` | `double` |
|---|---|---|---|---|---|---|
| `value1 / value2` | `divi` | `divui` | `divl` | `divul` | `divf` | `divd` |
| `value1 % value2` | `remi` | `remui` | `reml` | `remul` | — | — |

Integer division truncates toward zero. The remainder has the sign of `value1`. Integer division
or remainder by zero is undefined, as is dividing the most negative signed value by −1. Floating
point division follows IEEE 754.

---

### 3.13 Shifts and bitwise logic

<a id="shifts"></a>
#### `lshi`, `lshl`, `rshi`, `rshui`, `rshl`, `rshul`

Shift `value1` by `value2` bits.

| | |
|---|---|
| **Format** | the mnemonic alone (no operands) |
| **Stack** | `..., value1, value2 → ..., result` |

| Opcode | Operand types | Operation |
|---|---|---|
| `lshi` | `int`, `int` | Shift left. Also used for `uint`. |
| `lshl` | `long`, `long` | Shift left. Also used for `ulong`. |
| `rshi` | `int`, `int` | Arithmetic shift right. The sign bit is replicated. |
| `rshui` | `uint`, `uint` | Logical shift right. Zeros are shifted in. |
| `rshl` | `long`, `long` | Arithmetic shift right. |
| `rshul` | `ulong`, `ulong` | Logical shift right. |

The shift count is the same width as the value being shifted. A count that is negative, or greater
than or equal to the bit width, is undefined.

---

<a id="bitwise"></a>
#### `andi`, `andl`, `ori`, `orl`, `xori`, `xorl`

Bitwise logic on the top two stack values.

| | |
|---|---|
| **Format** | the mnemonic alone (no operands) |
| **Stack** | `..., value1, value2 → ..., result` |

| Operation | 32-bit | 64-bit |
|---|---|---|
| `value1 & value2` | `andi` | `andl` |
| `value1 \| value2` | `ori` | `orl` |
| `value1 ^ value2` | `xori` | `xorl` |

The same opcodes serve signed and unsigned types. There is no complement instruction. The compiler
implements `~x` as `consti_n1` followed by `xori` (or `constl_n1` and `xorl`).

---

### 3.14 Negation and logical not

<a id="neg"></a>
#### `negi`, `negl`, `negf`, `negd`

Negate the value on top of the stack.

| | |
|---|---|
| **Format** | the mnemonic alone (no operands) |
| **Stack** | `..., value → ..., result` |

| Opcode | Type |
|---|---|
| `negi` | `int` |
| `negl` | `long` |
| `negf` | `float` |
| `negd` | `double` |

Integer negation wraps, so negating the most negative value yields the same value. Floating point
negation flips the sign bit, including for zeros and NaN.

---

<a id="not"></a>
#### `not`

Logical not.

| | |
|---|---|
| **Format** | `not` |
| **Stack** | `..., int → ..., int` |

Replaces the value with 1 if it is 0, and with 0 otherwise. It is a logical not (`!`), not a
bitwise complement. The compiler often folds `!` into the choice of branch instruction instead of
emitting `not`.

---

### 3.15 Comparisons

<a id="cmp"></a>
#### `cmpeq*`, `cmpneq*`, `cmplt*`, `cmple*`, `cmpgt*`, `cmpge*`

Compare the top two stack values.

| | |
|---|---|
| **Format** | the mnemonic alone (no operands) |
| **Stack** | `..., value1, value2 → ..., int` |

Pushes 1 if `value1 <op> value2` holds, and 0 otherwise.

| Comparison | `int` | `uint` | `long` | `ulong` | `float` | `double` |
|---|---|---|---|---|---|---|
| `value1 == value2` | `cmpeqi` | `cmpeqi` | `cmpeql` | `cmpeql` | `cmpeqf` | `cmpeqd` |
| `value1 != value2` | `cmpneqi` | `cmpneqi` | `cmpneql` | `cmpneql` | `cmpneqf` | `cmpneqd` |
| `value1 < value2` | `cmplti` | `cmpltui` | `cmpltl` | `cmpltul` | `cmpltf` | `cmpltd` |
| `value1 <= value2` | `cmplei` | `cmpleui` | `cmplel` | `cmpleul` | `cmplef` | `cmpled` |
| `value1 > value2` | `cmpgti` | `cmpgtui` | `cmpgtl` | `cmpgtul` | `cmpgtf` | `cmpgtd` |
| `value1 >= value2` | `cmpgei` | `cmpgeui` | `cmpgel` | `cmpgeul` | `cmpgef` | `cmpged` |

Equality doesn't depend on signedness, so there are no unsigned equality opcodes. Floating point
comparisons follow IEEE 754. If either operand is NaN, every comparison yields 0 except
`cmpneqf`/`cmpneqd`, which yield 1.

---

### 3.16 Conditional branches

All branch offsets are relative to the address immediately following the instruction's operand. In
`bondd` output, the offset is followed by the absolute target in parentheses, e.g. `brz 3 (9)`.

<a id="ifz-ifnz"></a>
#### `ifz`, `ifzw`, `ifnz`, `ifnzw`

Pop a condition and branch on it.

| | |
|---|---|
| **Format** | `ifz offset:off16` · `ifzw offset:off32 index` · `ifnz offset:off16` · `ifnzw offset:off32 index` |
| **Operands** | `offset`: the branch offset. |
| **Stack** | `..., int → ...` |

Pops the condition. `ifz` and `ifzw` branch if it is zero, and `ifnz` and `ifnzw` branch if it is
non-zero. Otherwise execution continues with the next instruction. These are the branches used by
`if`, `while` and `for` statements.

---

<a id="brz-brnz"></a>
#### `brz`, `brzw`, `brnz`, `brnzw`

Branch on a condition, keeping it on the stack if the branch is taken.

| | |
|---|---|
| **Format** | `brz offset:off16` · `brzw offset:off32 index` · `brnz offset:off16` · `brnzw offset:off32 index` |
| **Operands** | `offset`: the branch offset. |
| **Stack** | branch taken: `..., int → ..., int` · not taken: `..., int → ...` |

`brz` and `brzw` branch if the condition is zero. `brnz` and `brnzw` branch if it is non-zero. When
the branch is taken, the condition is left on the stack. When execution falls through, it is
popped.

These instructions implement the short-circuit operators. For `a && b`, if `a` is zero the result
is already on the stack and the branch skips the evaluation of `b`. Otherwise `a` is discarded and
`b` becomes the result:

```
// return (a > 0) && (b > 0);
     0: push32_p0
     1: consti_0
     2: cmpgti
     3: brz         3 (9)      // if a > 0 is false, leave 0 on the stack and jump
     6: push32_p1
     7: consti_0
     8: cmpgti
     9: return32
```

---

<a id="nbrz-nbrnz"></a>
#### `nbrz`, `nbrzw`, `nbrnz`, `nbrnzw`

Negate a condition, then branch on it, keeping it on the stack if the branch is taken.

| | |
|---|---|
| **Format** | `nbrz offset:off16` · `nbrzw offset:off32 index` · `nbrnz offset:off16` · `nbrnzw offset:off32 index` |
| **Operands** | `offset`: the branch offset. |
| **Stack** | branch taken: `..., int → ..., int` · not taken: `..., int → ...` |

First replaces the condition `c` with its logical negation (1 if `c` is 0, and 0 otherwise). Then
behaves exactly like [`brz`/`brnz`](#brz-brnz) on the negated value. So `nbrz` branches when the
original condition is non-zero, leaving 0 on the stack, and `nbrnz` branches when it is zero,
leaving 1. The compiler uses these for short-circuit operators whose operand is negated with `!`.

> When a branch targets another branch, the code generator threads the jump straight to the final
> destination and may rewrite the opcode (for example a `brz` into an `nbrnz` or an `ifz`) so that
> the stack effect is preserved.

---

### 3.17 Unconditional branches

<a id="goto"></a>
#### `goto`, `gotow`

Branch unconditionally.

| | |
|---|---|
| **Format** | `goto offset:off16` · `gotow offset:off32 index` |
| **Operands** | `offset`: the branch offset. |
| **Stack** | `... → ...` |

---

### 3.18 Switches

Both switch instructions pop an `int` key and branch to the matching case. Their operand is an
inline jump table. It starts at the next 4-byte boundary in the function's code (counting from the
start of the function's code), so 0 to 3 padding bytes follow the opcode. All values in the table
are 32-bit integers, and **all offsets are relative to the end of the jump table**.

The compiler chooses `tableswitch` when the case values are dense, and `lookupswitch` otherwise.

<a id="lookupswitch"></a>
#### `lookupswitch`

Branch to the offset matching a key, by searching a sorted table.

| | |
|---|---|
| **Format** | `lookupswitch <padding> defaultOffset numMatches (match offset)…` |
| **Operands** | see the table layout below |
| **Stack** | `..., int → ...` |

| Field | Type | Meaning |
|---|---|---|
| padding | 0–3 bytes | Aligns the table to 4 bytes. |
| `defaultOffset` | `int32` | Offset taken when no match is found. |
| `numMatches` | `uint32` | Number of match/offset pairs. |
| `match[i]`, `offset[i]` | `int32`, `int32` | `numMatches` pairs, sorted by ascending `match`. |

The VM searches the pairs with a binary search. The table occupies `(2 + 2 × numMatches) × 4`
bytes after the padding.

```
     0: push32_p0
     1: lookupswitch
         default: 6 (34)
               1: 0 (28)
             100: 3 (31)
    28: constc      10
    ...
```

---

<a id="tableswitch"></a>
#### `tableswitch`

Branch to the offset matching a key, by indexing a table.

| | |
|---|---|
| **Format** | `tableswitch <padding> defaultOffset minMatch maxMatch offset…` |
| **Operands** | see the table layout below |
| **Stack** | `..., int → ...` |

| Field | Type | Meaning |
|---|---|---|
| padding | 0–3 bytes | Aligns the table to 4 bytes. |
| `defaultOffset` | `int32` | Offset taken when the key is outside `[minMatch, maxMatch]`. |
| `minMatch` | `int32` | Smallest key in the table. |
| `maxMatch` | `int32` | Largest key in the table. Must be ≥ `minMatch`. |
| `offset[i]` | `int32` | `maxMatch − minMatch + 1` offsets. `offset[key − minMatch]` is the target for `key`. |

Values in the range that have no case label get `defaultOffset`. The table occupies
`(4 + maxMatch − minMatch) × 4` bytes after the padding.

```
     0: push32_p0
     1: tableswitch
         default: 9 (37)
               1: 0 (28)
               2: 3 (31)
               3: 6 (34)
    28: constc      10
    ...
```

---

### 3.19 Function invocation

<a id="invoke"></a>
#### `invoke`

Call a function.

| | |
|---|---|
| **Format** | `invoke function:name index` |
| **Operands** | `function`: the function to call. In the CBO file it is an index into the qualified name table. The loader replaces it with the index of the function in the `CodeSegment`. |
| **Stack** | scalar return: `..., arguments → ..., value` · `void` return: `..., arguments → ...` · struct return: `..., arguments, returnAddress → ...` |

**Calling sequence.** Before `invoke`, the caller pushes, in order:

1. The arguments, **last argument first**, so that argument 0 ends up on top. Each argument
   occupies `ceil(size / 8)` slots. Struct arguments are pushed with [`loadmemw`](#loadmemw).
2. For a member function, the `this` pointer.
3. For a function that returns a struct, the address of a caller-allocated buffer for the return
   value, usually obtained with [`loadfp`](#loadfp).

The callee's frame pointer is placed directly above the arguments, so they become its parameters
in place: argument 0 at `fp − 8`, and so on (`this` at `fp − 8` and argument 0 at `fp − 16` for a
member function). See [Stack frames](#stack-frames).

**Return value.** On return, the arguments, the `this` pointer and the return buffer address are
removed from the stack. A function that returns a scalar leaves its result in a single slot, where
the deepest argument was. A `void` function or a function that returns a struct leaves nothing; a
struct result is in the return buffer.

The callee may be a Bond function or a native function bound to the VM. The calling sequence is
the same.

```
// int CallAdd3() { return Add3(1, 2, 3); }
     0: consti_3
     1: consti_2
     2: consti_1
     3: invoke      Add3       // pops 3 slots, pushes the int result
     6: return32

// int CallBig() { Big b = MakeBig(5); return b.x[0]; }   (Big is 24 bytes)
     0: loadfp      0          // destination: local b
     3: constc      5          // argument 0
     5: loadfp      24         // return buffer: a temporary local
     8: invoke      MakeBig    // pops the argument and the buffer address
    11: loadfp      24
    14: memcopyw    24         // b = temporary
    17: push32_l0
    18: return32
```

> **Note:** The stack delta recorded for `invoke` in `opcodes.h` is 0. The code generator accounts
> for the arguments and return value separately.

---

<a id="invokeunpack"></a>
#### `invokeunpack`

Call a function whose parameters require more than slot alignment.

| | |
|---|---|
| **Format** | `invokeunpack function:name index` |
| **Operands** | `function`: the index of the function in the `CodeSegment`. |
| **Stack** | same as [`invoke`](#invoke) |

The compiler never emits this opcode, and it never appears in a CBO file. When the `CboLoader`
resolves an `invoke`, it substitutes `invokeunpack` if the callee's frame pointer alignment exceeds
`BOND_SLOT_SIZE`. In that case the packed arguments on the caller's stack aren't where the callee's
frame expects them. Before executing the callee, `invokeunpack` moves each argument from its packed
position to its frame pointer offset, as recorded in the callee's parameter signatures. Apart from
that, it behaves exactly like `invoke`.

---

### 3.20 Function return

All return instructions end the current function and resume the caller after its `invoke`. Values
remaining on the callee's operand stack are discarded.

<a id="return"></a>
#### `return`

Return from a `void` function.

| | |
|---|---|
| **Format** | `return` |
| **Stack** | `... → (discarded)` |

---

<a id="return32-return64"></a>
#### `return32`, `return64`

Return a scalar value.

| | |
|---|---|
| **Format** | `return32` · `return64` |
| **Stack** | `..., value32 → (discarded)` · `..., value64 → (discarded)` |

Copies the 32-bit or 64-bit value on top of the stack to the caller's return slot. `return32` is
used for `bool`, `char`, `short`, `int` and `float` results (and their unsigned counterparts), and
for addresses with 32-bit pointers. `return64` is used for `long`, `ulong` and `double` results, and
for addresses with 64-bit pointers.

---

<a id="returnmemw"></a>
#### `returnmemw`

Return a struct value.

| | |
|---|---|
| **Format** | `returnmemw size:int index` |
| **Operands** | `size`: value32 table index of the size of the struct in bytes. |
| **Stack** | `..., address → (discarded)` |

Copies `size` bytes from `address` to the return buffer supplied by the caller (see
[`invoke`](#invoke)).

```
// Big MakeBig(int v) { Big b = {}; b.x[0] = v; return b; }
     0: loadfp      0
     3: memzerow    24
     6: push32_p0
     7: pop32_l0
     8: loadfp      0
    11: returnmemw  24
```

---

## 4. Appendix: opcode table

Opcode values are assigned in the order of `BOND_OPCODE_LIST`, starting from 0. The length includes
the opcode byte. The stack delta is the nominal value recorded in `opcodes.h`. For the branches that
keep their condition when taken, and for `loadmemw` and `invoke`, the actual effect is described in
the instruction's entry.

| Dec | Hex | Mnemonic | Operand | Length (bytes) | Stack delta (slots) |
|---:|---:|---|---|---:|---:|
| 0 | 0x00 | `nop` | — | 1 | 0 |
| 1 | 0x01 | `constc` | char | 2 | +1 |
| 2 | 0x02 | `constuc` | uchar | 2 | +1 |
| 3 | 0x03 | `consts` | short | 3 | +1 |
| 4 | 0x04 | `constus` | ushort | 3 | +1 |
| 5 | 0x05 | `const32` | value32 index | 3 | +1 |
| 6 | 0x06 | `const64` | value64 index | 3 | +1 |
| 7 | 0x07 | `consti_n2` | — | 1 | +1 |
| 8 | 0x08 | `consti_n1` | — | 1 | +1 |
| 9 | 0x09 | `consti_0` | — | 1 | +1 |
| 10 | 0x0A | `consti_1` | — | 1 | +1 |
| 11 | 0x0B | `consti_2` | — | 1 | +1 |
| 12 | 0x0C | `consti_3` | — | 1 | +1 |
| 13 | 0x0D | `consti_4` | — | 1 | +1 |
| 14 | 0x0E | `consti_8` | — | 1 | +1 |
| 15 | 0x0F | `constl_n2` | — | 1 | +1 |
| 16 | 0x10 | `constl_n1` | — | 1 | +1 |
| 17 | 0x11 | `constl_0` | — | 1 | +1 |
| 18 | 0x12 | `constl_1` | — | 1 | +1 |
| 19 | 0x13 | `constl_2` | — | 1 | +1 |
| 20 | 0x14 | `constl_3` | — | 1 | +1 |
| 21 | 0x15 | `constl_4` | — | 1 | +1 |
| 22 | 0x16 | `constl_8` | — | 1 | +1 |
| 23 | 0x17 | `constf_n2` | — | 1 | +1 |
| 24 | 0x18 | `constf_n1` | — | 1 | +1 |
| 25 | 0x19 | `constf_nh` | — | 1 | +1 |
| 26 | 0x1A | `constf_0` | — | 1 | +1 |
| 27 | 0x1B | `constf_h` | — | 1 | +1 |
| 28 | 0x1C | `constf_1` | — | 1 | +1 |
| 29 | 0x1D | `constf_2` | — | 1 | +1 |
| 30 | 0x1E | `constd_n2` | — | 1 | +1 |
| 31 | 0x1F | `constd_n1` | — | 1 | +1 |
| 32 | 0x20 | `constd_nh` | — | 1 | +1 |
| 33 | 0x21 | `constd_0` | — | 1 | +1 |
| 34 | 0x22 | `constd_h` | — | 1 | +1 |
| 35 | 0x23 | `constd_1` | — | 1 | +1 |
| 36 | 0x24 | `constd_2` | — | 1 | +1 |
| 37 | 0x25 | `loadfp` | short | 3 | +1 |
| 38 | 0x26 | `loadfpw` | int index | 3 | +1 |
| 39 | 0x27 | `loadea` | name index | 3 | +1 |
| 40 | 0x28 | `loadstr` | string index | 3 | +1 |
| 41 | 0x29 | `loadc` | — | 1 | 0 |
| 42 | 0x2A | `loaduc` | — | 1 | 0 |
| 43 | 0x2B | `loads` | — | 1 | 0 |
| 44 | 0x2C | `loadus` | — | 1 | 0 |
| 45 | 0x2D | `load32` | — | 1 | 0 |
| 46 | 0x2E | `load64` | — | 1 | 0 |
| 47 | 0x2F | `loadmemw` | int index | 3 | 0 |
| 48 | 0x30 | `storec` | — | 1 | −2 |
| 49 | 0x31 | `stores` | — | 1 | −2 |
| 50 | 0x32 | `store32` | — | 1 | −2 |
| 51 | 0x33 | `store64` | — | 1 | −2 |
| 52 | 0x34 | `pushc` | short | 3 | +1 |
| 53 | 0x35 | `pushcw` | int index | 3 | +1 |
| 54 | 0x36 | `pushuc` | short | 3 | +1 |
| 55 | 0x37 | `pushucw` | int index | 3 | +1 |
| 56 | 0x38 | `pushs` | short | 3 | +1 |
| 57 | 0x39 | `pushsw` | int index | 3 | +1 |
| 58 | 0x3A | `pushus` | short | 3 | +1 |
| 59 | 0x3B | `pushusw` | int index | 3 | +1 |
| 60 | 0x3C | `push32` | short | 3 | +1 |
| 61 | 0x3D | `push32w` | int index | 3 | +1 |
| 62 | 0x3E | `push32_p0` | — | 1 | +1 |
| 63 | 0x3F | `push32_p1` | — | 1 | +1 |
| 64 | 0x40 | `push32_p2` | — | 1 | +1 |
| 65 | 0x41 | `push32_p3` | — | 1 | +1 |
| 66 | 0x42 | `push32_l0` | — | 1 | +1 |
| 67 | 0x43 | `push32_l1` | — | 1 | +1 |
| 68 | 0x44 | `push32_l2` | — | 1 | +1 |
| 69 | 0x45 | `push32_l3` | — | 1 | +1 |
| 70 | 0x46 | `push64` | short | 3 | +1 |
| 71 | 0x47 | `push64w` | int index | 3 | +1 |
| 72 | 0x48 | `push64_p0` | — | 1 | +1 |
| 73 | 0x49 | `push64_p1` | — | 1 | +1 |
| 74 | 0x4A | `push64_p2` | — | 1 | +1 |
| 75 | 0x4B | `push64_p3` | — | 1 | +1 |
| 76 | 0x4C | `push64_l0` | — | 1 | +1 |
| 77 | 0x4D | `push64_l1` | — | 1 | +1 |
| 78 | 0x4E | `push64_l2` | — | 1 | +1 |
| 79 | 0x4F | `push64_l3` | — | 1 | +1 |
| 80 | 0x50 | `pop` | — | 1 | −1 |
| 81 | 0x51 | `popc` | short | 3 | −1 |
| 82 | 0x52 | `popcw` | int index | 3 | −1 |
| 83 | 0x53 | `pops` | short | 3 | −1 |
| 84 | 0x54 | `popsw` | int index | 3 | −1 |
| 85 | 0x55 | `pop32` | short | 3 | −1 |
| 86 | 0x56 | `pop32w` | int index | 3 | −1 |
| 87 | 0x57 | `pop32_p0` | — | 1 | −1 |
| 88 | 0x58 | `pop32_p1` | — | 1 | −1 |
| 89 | 0x59 | `pop32_p2` | — | 1 | −1 |
| 90 | 0x5A | `pop32_p3` | — | 1 | −1 |
| 91 | 0x5B | `pop32_l0` | — | 1 | −1 |
| 92 | 0x5C | `pop32_l1` | — | 1 | −1 |
| 93 | 0x5D | `pop32_l2` | — | 1 | −1 |
| 94 | 0x5E | `pop32_l3` | — | 1 | −1 |
| 95 | 0x5F | `pop64` | short | 3 | −1 |
| 96 | 0x60 | `pop64w` | int index | 3 | −1 |
| 97 | 0x61 | `pop64_p0` | — | 1 | −1 |
| 98 | 0x62 | `pop64_p1` | — | 1 | −1 |
| 99 | 0x63 | `pop64_p2` | — | 1 | −1 |
| 100 | 0x64 | `pop64_p3` | — | 1 | −1 |
| 101 | 0x65 | `pop64_l0` | — | 1 | −1 |
| 102 | 0x66 | `pop64_l1` | — | 1 | −1 |
| 103 | 0x67 | `pop64_l2` | — | 1 | −1 |
| 104 | 0x68 | `pop64_l3` | — | 1 | −1 |
| 105 | 0x69 | `memcopyw` | int index | 3 | −2 |
| 106 | 0x6A | `memzerow` | int index | 3 | −1 |
| 107 | 0x6B | `dup` | — | 1 | +1 |
| 108 | 0x6C | `dupins` | — | 1 | +1 |
| 109 | 0x6D | `swap` | — | 1 | 0 |
| 110 | 0x6E | `itoc` | — | 1 | 0 |
| 111 | 0x6F | `uitouc` | — | 1 | 0 |
| 112 | 0x70 | `itos` | — | 1 | 0 |
| 113 | 0x71 | `uitous` | — | 1 | 0 |
| 114 | 0x72 | `itol` | — | 1 | 0 |
| 115 | 0x73 | `uitoul` | — | 1 | 0 |
| 116 | 0x74 | `itof` | — | 1 | 0 |
| 117 | 0x75 | `uitof` | — | 1 | 0 |
| 118 | 0x76 | `itod` | — | 1 | 0 |
| 119 | 0x77 | `uitod` | — | 1 | 0 |
| 120 | 0x78 | `ltoi` | — | 1 | 0 |
| 121 | 0x79 | `ltof` | — | 1 | 0 |
| 122 | 0x7A | `ultof` | — | 1 | 0 |
| 123 | 0x7B | `ltod` | — | 1 | 0 |
| 124 | 0x7C | `ultod` | — | 1 | 0 |
| 125 | 0x7D | `ftoi` | — | 1 | 0 |
| 126 | 0x7E | `ftoui` | — | 1 | 0 |
| 127 | 0x7F | `ftol` | — | 1 | 0 |
| 128 | 0x80 | `ftoul` | — | 1 | 0 |
| 129 | 0x81 | `ftod` | — | 1 | 0 |
| 130 | 0x82 | `dtoi` | — | 1 | 0 |
| 131 | 0x83 | `dtoui` | — | 1 | 0 |
| 132 | 0x84 | `dtol` | — | 1 | 0 |
| 133 | 0x85 | `dtoul` | — | 1 | 0 |
| 134 | 0x86 | `dtof` | — | 1 | 0 |
| 135 | 0x87 | `ptroffi` | short | 3 | −1 |
| 136 | 0x88 | `ptroffl` | short | 3 | −1 |
| 137 | 0x89 | `ptrdiff` | short | 3 | −1 |
| 138 | 0x8A | `inci` | char, char | 3 | 0 |
| 139 | 0x8B | `incl` | char, char | 3 | 0 |
| 140 | 0x8C | `addi` | — | 1 | −1 |
| 141 | 0x8D | `addl` | — | 1 | −1 |
| 142 | 0x8E | `addf` | — | 1 | −1 |
| 143 | 0x8F | `addd` | — | 1 | −1 |
| 144 | 0x90 | `subi` | — | 1 | −1 |
| 145 | 0x91 | `subl` | — | 1 | −1 |
| 146 | 0x92 | `subf` | — | 1 | −1 |
| 147 | 0x93 | `subd` | — | 1 | −1 |
| 148 | 0x94 | `muli` | — | 1 | −1 |
| 149 | 0x95 | `mull` | — | 1 | −1 |
| 150 | 0x96 | `mulf` | — | 1 | −1 |
| 151 | 0x97 | `muld` | — | 1 | −1 |
| 152 | 0x98 | `divi` | — | 1 | −1 |
| 153 | 0x99 | `divui` | — | 1 | −1 |
| 154 | 0x9A | `divl` | — | 1 | −1 |
| 155 | 0x9B | `divul` | — | 1 | −1 |
| 156 | 0x9C | `divf` | — | 1 | −1 |
| 157 | 0x9D | `divd` | — | 1 | −1 |
| 158 | 0x9E | `remi` | — | 1 | −1 |
| 159 | 0x9F | `remui` | — | 1 | −1 |
| 160 | 0xA0 | `reml` | — | 1 | −1 |
| 161 | 0xA1 | `remul` | — | 1 | −1 |
| 162 | 0xA2 | `lshi` | — | 1 | −1 |
| 163 | 0xA3 | `lshl` | — | 1 | −1 |
| 164 | 0xA4 | `rshi` | — | 1 | −1 |
| 165 | 0xA5 | `rshui` | — | 1 | −1 |
| 166 | 0xA6 | `rshl` | — | 1 | −1 |
| 167 | 0xA7 | `rshul` | — | 1 | −1 |
| 168 | 0xA8 | `andi` | — | 1 | −1 |
| 169 | 0xA9 | `andl` | — | 1 | −1 |
| 170 | 0xAA | `ori` | — | 1 | −1 |
| 171 | 0xAB | `orl` | — | 1 | −1 |
| 172 | 0xAC | `xori` | — | 1 | −1 |
| 173 | 0xAD | `xorl` | — | 1 | −1 |
| 174 | 0xAE | `negi` | — | 1 | 0 |
| 175 | 0xAF | `negl` | — | 1 | 0 |
| 176 | 0xB0 | `negf` | — | 1 | 0 |
| 177 | 0xB1 | `negd` | — | 1 | 0 |
| 178 | 0xB2 | `not` | — | 1 | 0 |
| 179 | 0xB3 | `cmpeqi` | — | 1 | −1 |
| 180 | 0xB4 | `cmpeql` | — | 1 | −1 |
| 181 | 0xB5 | `cmpeqf` | — | 1 | −1 |
| 182 | 0xB6 | `cmpeqd` | — | 1 | −1 |
| 183 | 0xB7 | `cmpneqi` | — | 1 | −1 |
| 184 | 0xB8 | `cmpneql` | — | 1 | −1 |
| 185 | 0xB9 | `cmpneqf` | — | 1 | −1 |
| 186 | 0xBA | `cmpneqd` | — | 1 | −1 |
| 187 | 0xBB | `cmplti` | — | 1 | −1 |
| 188 | 0xBC | `cmpltui` | — | 1 | −1 |
| 189 | 0xBD | `cmpltl` | — | 1 | −1 |
| 190 | 0xBE | `cmpltul` | — | 1 | −1 |
| 191 | 0xBF | `cmpltf` | — | 1 | −1 |
| 192 | 0xC0 | `cmpltd` | — | 1 | −1 |
| 193 | 0xC1 | `cmplei` | — | 1 | −1 |
| 194 | 0xC2 | `cmpleui` | — | 1 | −1 |
| 195 | 0xC3 | `cmplel` | — | 1 | −1 |
| 196 | 0xC4 | `cmpleul` | — | 1 | −1 |
| 197 | 0xC5 | `cmplef` | — | 1 | −1 |
| 198 | 0xC6 | `cmpled` | — | 1 | −1 |
| 199 | 0xC7 | `cmpgti` | — | 1 | −1 |
| 200 | 0xC8 | `cmpgtui` | — | 1 | −1 |
| 201 | 0xC9 | `cmpgtl` | — | 1 | −1 |
| 202 | 0xCA | `cmpgtul` | — | 1 | −1 |
| 203 | 0xCB | `cmpgtf` | — | 1 | −1 |
| 204 | 0xCC | `cmpgtd` | — | 1 | −1 |
| 205 | 0xCD | `cmpgei` | — | 1 | −1 |
| 206 | 0xCE | `cmpgeui` | — | 1 | −1 |
| 207 | 0xCF | `cmpgel` | — | 1 | −1 |
| 208 | 0xD0 | `cmpgeul` | — | 1 | −1 |
| 209 | 0xD1 | `cmpgef` | — | 1 | −1 |
| 210 | 0xD2 | `cmpged` | — | 1 | −1 |
| 211 | 0xD3 | `brz` | off16 | 3 | −1 |
| 212 | 0xD4 | `brzw` | off32 index | 3 | −1 |
| 213 | 0xD5 | `brnz` | off16 | 3 | −1 |
| 214 | 0xD6 | `brnzw` | off32 index | 3 | −1 |
| 215 | 0xD7 | `nbrz` | off16 | 3 | −1 |
| 216 | 0xD8 | `nbrzw` | off32 index | 3 | −1 |
| 217 | 0xD9 | `nbrnz` | off16 | 3 | −1 |
| 218 | 0xDA | `nbrnzw` | off32 index | 3 | −1 |
| 219 | 0xDB | `ifz` | off16 | 3 | −1 |
| 220 | 0xDC | `ifzw` | off32 index | 3 | −1 |
| 221 | 0xDD | `ifnz` | off16 | 3 | −1 |
| 222 | 0xDE | `ifnzw` | off32 index | 3 | −1 |
| 223 | 0xDF | `goto` | off16 | 3 | 0 |
| 224 | 0xE0 | `gotow` | off32 index | 3 | 0 |
| 225 | 0xE1 | `lookupswitch` | lookupswitch table | variable | −1 |
| 226 | 0xE2 | `tableswitch` | tableswitch table | variable | −1 |
| 227 | 0xE3 | `invoke` | name index | 3 | 0 |
| 228 | 0xE4 | `invokeunpack` | name index | 3 | 0 |
| 229 | 0xE5 | `return` | — | 1 | 0 |
| 230 | 0xE6 | `return32` | — | 1 | −1 |
| 231 | 0xE7 | `return64` | — | 1 | −1 |
| 232 | 0xE8 | `returnmemw` | int index | 3 | −1 |
