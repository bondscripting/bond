# gdb pretty-printers for the Bond VM. See docs/debugging.md.
#
# Load them with:
#     source /path/to/bond/debugger/bond_gdb.py
# for example from ~/.gdbinit. They are the gdb counterpart of vs/bond.natvis and show the same
# views: the Bond call stack, stack frames with typed arguments, raw locals and operand stacks,
# functions, code segments and qualified names.
#
# These printers read the private members of the VM classes, so they must be kept in sync with
# include/bond/vm/vm.h, include/bond/vm/codesegment.h, include/bond/stl/autostack.h and
# include/bond/types/qualifiedname.h, as well as with vs/bond.natvis and bond_lldb.py.
# SignatureType values are compared numerically:
# 0 void, 1 bool, 2 char, 3 uchar, 4 short, 5 ushort, 6 int, 7 uint, 8 long, 9 ulong, 10 float,
# 11 double, 12 pointer, 13 aggregate.

import gdb
import gdb.printing

SIG_AGGREGATE = 13
SIG_POINTER = 12

SIG_NAMES = (
	"void", "bool", "char", "uchar", "short", "ushort", "int", "uint", "long", "ulong", "float",
	"double", "pointer", "aggregate")

# The C types of the Bond types, indexed by SignatureType. Pointers and aggregates are handled
# separately.
SIG_C_TYPES = (
	None, "bool", "signed char", "unsigned char", "short", "unsigned short", "int", "unsigned int",
	"long long", "unsigned long long", "float", "double")

# Protects against reading garbage when a pointer is uninitialized.
MAX_NAME_ELEMENTS = 64
MAX_STACK_FRAMES = 100000

VALUE64_SIZE = 8


def _lookup_type(name):
	try:
		return gdb.lookup_type(name)
	except gdb.error:
		return None


def _is_null(pointer):
	return int(pointer) == 0


def _array(pointer, element_type, count):
	"""Returns a gdb.Value of type element_type[count] at the given address."""
	return pointer.cast(element_type.array(count - 1).pointer()).dereference()


def _byte_type():
	return gdb.lookup_type("unsigned char")


def qualified_name_string(name):
	"""Formats a Bond::QualifiedName as a dotted name."""
	elements = name["mElements"]
	if _is_null(elements):
		return "<static initializer>"
	parts = []
	for i in range(MAX_NAME_ELEMENTS):
		element = elements[i]
		if _is_null(element):
			break
		parts.append(element.string("utf-8", "replace"))
	if not parts:
		return "<static initializer>"
	return ".".join(parts)


def signature_type_string(sig):
	"""Formats a ReturnSignature or ParamSignature type, e.g. "int" or "aggregate<16>"."""
	sig_type = int(sig["mType"])
	name = SIG_NAMES[sig_type] if sig_type < len(SIG_NAMES) else "?%d" % sig_type
	if sig_type == SIG_AGGREGATE:
		name += "<%d>" % int(sig["mSize"])
	return name


def function_string(function):
	name = qualified_name_string(function["mName"])
	if int(function["mCodeSize"]) == 0:
		name += " [native]"
	return name


def stack_frame_string(frame):
	function = frame["mFunction"]
	if _is_null(function):
		return "{ no function }"
	function = function.dereference()
	name = qualified_name_string(function["mName"])
	if int(function["mCodeSize"]) == 0:
		return name + " [native]"
	if not _is_null(frame["mOperandStackPointer"]):
		return "%s @ pc %d" % (name, int(frame["mPc"]))
	return name


def arg_value(frame_pointer, sig):
	"""Returns the value of an argument, typed according to its ParamSignature."""
	sig_type = int(sig["mType"])
	address = frame_pointer + int(sig["mFramePointerOffset"])
	if sig_type == SIG_AGGREGATE:
		size = int(sig["mSize"])
		if size == 0:
			return address.cast(_byte_type().pointer())
		return _array(address, _byte_type(), size)
	if sig_type == SIG_POINTER:
		return address.cast(gdb.lookup_type("void").pointer().pointer()).dereference()
	if 0 < sig_type < len(SIG_C_TYPES):
		return address.cast(gdb.lookup_type(SIG_C_TYPES[sig_type]).pointer()).dereference()
	return address


def stack_frame_elements(stack):
	"""Yields the Elements of an AutoStack from the top down."""
	element = stack["mTop"]
	count = 0
	while not _is_null(element) and count < MAX_STACK_FRAMES:
		element = element.dereference()
		yield element
		element = element["mNext"]
		count += 1


class QualifiedNamePrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return qualified_name_string(self.val)


class ReturnSignaturePrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return signature_type_string(self.val)


class ParamSignaturePrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return "%s at fp offset %d" % (
			signature_type_string(self.val), int(self.val["mFramePointerOffset"]))


class ParamListSignaturePrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return "{ count=%d }" % int(self.val["mParamCount"])

	def children(self):
		signatures = self.val["mParamSignatures"]
		for i in range(int(self.val["mParamCount"])):
			yield "[%d]" % i, signatures[i]

	def display_hint(self):
		return "array"


class FunctionPrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return function_string(self.val)

	def children(self):
		f = self.val
		code_size = int(f["mCodeSize"])
		yield "name", f["mName"]
		yield "return", f["mReturnSignature"]
		yield "params", f["mParamListSignature"]
		if code_size == 0:
			yield "native function", f["mNativeFunction"]
		else:
			yield "code", _array(f["mCode"], _byte_type(), code_size)
		yield "code size", f["mCodeSize"]
		yield "arg size", f["mArgSize"]
		yield "packed arg size", f["mPackedArgSize"]
		yield "local size", f["mLocalSize"]
		yield "stack size", f["mStackSize"]
		yield "frame pointer alignment", f["mFramePointerAlignment"]
		yield "constant table", f["mConstantTable"]


class StackFramePrinter:
	"""The activation record of a function. mPc and mOperandStackPointer are only valid while the
	frame is suspended in a call, i.e. for every Bond frame but the top one. The pc of the top frame
	is the pc local variable of the innermost C++ VM::ExecuteScriptFunction frame."""

	def __init__(self, val):
		self.val = val

	def to_string(self):
		return stack_frame_string(self.val)

	def children(self):
		frame = self.val
		function_pointer = frame["mFunction"]
		fp = frame["mFramePointer"]
		osp = frame["mOperandStackPointer"]
		suspended = not _is_null(osp)

		if not _is_null(function_pointer):
			function = function_pointer.dereference()
			yield "function", function

			if suspended:
				pc = frame["mPc"]
				yield "pc", pc
				opcode = function["mCode"][int(pc)]
				opcode_type = _lookup_type("Bond::OpCode")
				yield "instruction", opcode.cast(opcode_type) if opcode_type is not None else opcode

			params = function["mParamListSignature"]
			signatures = params["mParamSignatures"]
			for i in range(int(params["mParamCount"])):
				sig = signatures[i]
				yield "args[%d] %s" % (i, signature_type_string(sig)), arg_value(fp, sig)

			local_size = int(function["mLocalSize"])
			if local_size != 0:
				yield "locals (bytes)", _array(fp, _byte_type(), local_size)
			if local_size >= 4:
				yield "locals (ints)", _array(fp, gdb.lookup_type("int"), local_size // 4)

		if suspended:
			num_slots = int(osp - frame["mStackPointer"]) // VALUE64_SIZE
			if num_slots > 0:
				yield "operand stack", _array(
					frame["mStackPointer"], gdb.lookup_type("Bond::Value64"), num_slots)

		void_pointer = gdb.lookup_type("void").pointer()
		yield "frame pointer", fp.cast(void_pointer)
		if not _is_null(frame["mReturnPointer"]):
			yield "return pointer", frame["mReturnPointer"].cast(void_pointer)
		yield "collector", frame["mCollector"]


class InvocationStackFramePrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return stack_frame_string(self.val["mValue"])

	def children(self):
		yield "frame", self.val["mValue"]
		yield "next arg", self.val["mNextArg"]


class CallStackPrinter:
	"""Bond::AutoStack<Bond::StackFrame>: the call stack, from the top down, without the VM's empty
	bottom frame."""

	def __init__(self, val):
		self.val = val

	def to_string(self):
		top = self.val["mTop"]
		if _is_null(top) or _is_null(top.dereference()["mValue"]["mFunction"]):
			return "{ empty }"
		return "{ top=%s }" % stack_frame_string(top.dereference()["mValue"])

	def children(self):
		i = 0
		for element in stack_frame_elements(self.val):
			frame = element["mValue"]
			if not _is_null(frame["mFunction"]):
				yield "[%d]" % i, frame
				i += 1

	def display_hint(self):
		return "array"


class AutoStackPrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		top = self.val["mTop"]
		if _is_null(top):
			return "{ empty }"
		return "{ top=%s }" % str(top.dereference()["mValue"])

	def children(self):
		for i, element in enumerate(stack_frame_elements(self.val)):
			yield "[%d]" % i, element["mValue"]

	def display_hint(self):
		return "array"


class AutoStackElementPrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return str(self.val["mValue"])


class VMPrinter:
	"""The stack base is read from the empty bottom frame, which VM::VM points at mStack, so that the
	internals of std::unique_ptr do not matter."""

	def __init__(self, val):
		self.val = val

	def to_string(self):
		return CallStackPrinter(self.val["mStackFrames"]).to_string()

	def children(self):
		vm = self.val
		stack_frames = vm["mStackFrames"]
		stack_base = vm["mDummyFrame"]["mValue"]["mStackPointer"]
		yield "call stack", stack_frames
		yield "code segment", vm["mCodeSegment"]
		yield "stack base", stack_base.cast(gdb.lookup_type("void").pointer())
		yield "stack size", vm["mStackSize"]
		top = stack_frames["mTop"]
		if not _is_null(top):
			yield "stack in use", int(top.dereference()["mValue"]["mStackPointer"] - stack_base)
		yield "allocator", vm["mAllocator"]


class CodeSegmentPrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return "{ functions=%d static initializers=%d data=%d }" % (
			int(self.val["mFunctionCount"]),
			int(self.val["mStaticInitializerCount"]),
			int(self.val["mDataCount"]))

	def children(self):
		segment = self.val
		for name, table, count in (
				("functions", "mFunctionTable", "mFunctionCount"),
				("static initializers", "mStaticInitializerTable", "mStaticInitializerCount"),
				("data", "mDataTable", "mDataCount")):
			n = int(segment[count])
			if n > 0:
				pointer = segment[table]
				yield name, _array(pointer, pointer.type.target(), n)
			else:
				yield name, segment[table]


class DataEntryPrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		return qualified_name_string(self.val["mName"])

	def children(self):
		yield "name", self.val["mName"]
		yield "data", self.val["mData"]


class Value32Printer:
	"""Constants hold values of any type, so show the common views."""

	def __init__(self, val):
		self.val = val

	def to_string(self):
		v = self.val
		return "{ int=%d float=%s hex=0x%x }" % (int(v["mInt"]), float(v["mFloat"]), int(v["mUInt"]))


class Value64Printer:
	"""Operand stack slots hold values of any type, so show the common views."""

	def __init__(self, val):
		self.val = val

	def to_string(self):
		v = self.val
		if v.address is not None:
			as_int = int(v.address.cast(gdb.lookup_type("int").pointer()).dereference())
		else:
			# Not in memory: take the low 32 bits, as they are on little-endian targets.
			as_int = int(v["mULong"]) & 0xFFFFFFFF
			as_int = as_int - 0x100000000 if as_int >= 0x80000000 else as_int
		return "{ int=%d long=%d double=%s hex=0x%x }" % (
			as_int, int(v["mLong"]), float(v["mDouble"]), int(v["mULong"]))


def build_pretty_printer():
	pp = gdb.printing.RegexpCollectionPrettyPrinter("bond")
	pp.add_printer("QualifiedName", "^Bond::QualifiedName$", QualifiedNamePrinter)
	pp.add_printer("ReturnSignature", "^Bond::ReturnSignature$", ReturnSignaturePrinter)
	pp.add_printer("ParamSignature", "^Bond::ParamSignature$", ParamSignaturePrinter)
	pp.add_printer("ParamListSignature", "^Bond::ParamListSignature$", ParamListSignaturePrinter)
	pp.add_printer("Function", "^Bond::Function$", FunctionPrinter)
	pp.add_printer("StackFrame", "^Bond::StackFrame$", StackFramePrinter)
	pp.add_printer("InvocationStackFrame", "^Bond::InvocationStackFrame$", InvocationStackFramePrinter)
	# The call stack must come before the generic AutoStack, since the first match wins.
	pp.add_printer("CallStack", "^Bond::AutoStack<Bond::StackFrame>$", CallStackPrinter)
	pp.add_printer("AutoStackElement", "^Bond::AutoStack<.*>::Element$", AutoStackElementPrinter)
	pp.add_printer("AutoStack", "^Bond::AutoStack<.*>$", AutoStackPrinter)
	pp.add_printer("VM", "^Bond::VM$", VMPrinter)
	pp.add_printer("CodeSegment", "^Bond::CodeSegment$", CodeSegmentPrinter)
	pp.add_printer("DataEntry", "^Bond::DataEntry$", DataEntryPrinter)
	pp.add_printer("Value32", "^Bond::Value32$", Value32Printer)
	pp.add_printer("Value64", "^Bond::Value64$", Value64Printer)
	return pp


def register_bond_printers(objfile=None):
	"""Registers the printers with the given objfile, or globally if it is None."""
	gdb.printing.register_pretty_printer(objfile, build_pretty_printer(), replace=True)


register_bond_printers()
