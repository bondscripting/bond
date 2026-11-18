# lldb data formatters for the Bond VM. See docs/debugging.md.
#
# Load them with:
#     command script import /path/to/bond/debugger/bond_lldb.py
# for example from ~/.lldbinit. They are the lldb counterpart of vs/bond.natvis and show the same
# views: the Bond call stack, stack frames with typed arguments, raw locals and operand stacks,
# functions, code segments and qualified names. They are registered in the type category "bond".
#
# These formatters read the private members of the VM classes, so they must be kept in sync with
# include/bond/vm/vm.h, include/bond/vm/codesegment.h, include/bond/stl/autostack.h and
# include/bond/types/qualifiedname.h, as well as with vs/bond.natvis and bond_gdb.py.
# SignatureType values are compared numerically:
# 0 void, 1 bool, 2 char, 3 uchar, 4 short, 5 ushort, 6 int, 7 uint, 8 long, 9 ulong, 10 float,
# 11 double, 12 pointer, 13 aggregate.

import lldb

SIG_AGGREGATE = 13
SIG_POINTER = 12

SIG_NAMES = (
	"void", "bool", "char", "uchar", "short", "ushort", "int", "uint", "long", "ulong", "float",
	"double", "pointer", "aggregate")

# The basic types of the Bond types, indexed by SignatureType. Pointers and aggregates are handled
# separately.
SIG_BASIC_TYPES = (
	None,
	lldb.eBasicTypeBool,
	lldb.eBasicTypeSignedChar,
	lldb.eBasicTypeUnsignedChar,
	lldb.eBasicTypeShort,
	lldb.eBasicTypeUnsignedShort,
	lldb.eBasicTypeInt,
	lldb.eBasicTypeUnsignedInt,
	lldb.eBasicTypeLongLong,
	lldb.eBasicTypeUnsignedLongLong,
	lldb.eBasicTypeFloat,
	lldb.eBasicTypeDouble)

# Protects against reading garbage when a pointer is uninitialized.
MAX_NAME_ELEMENTS = 64
MAX_NAME_LENGTH = 1024
MAX_STACK_FRAMES = 100000

VALUE64_SIZE = 8


# Helpers.

def _raw(valobj):
	"""Returns the value without synthetic children, so that its real members can be read."""
	return valobj.GetNonSyntheticValue()


def _member(valobj, name):
	return valobj.GetChildMemberWithName(name)


def _unsigned(valobj, name):
	return _member(valobj, name).GetValueAsUnsigned(0)


def _signed(valobj, name):
	return _member(valobj, name).GetValueAsSigned(0)


def _referent(valobj):
	"""Returns the object that a reference member refers to, or the value itself otherwise."""
	if valobj.GetType().IsReferenceType():
		return valobj.Dereference()
	return valobj


def _basic_type(valobj, basic_type):
	return valobj.GetTarget().GetBasicType(basic_type)


def _void_pointer_type(valobj):
	return _basic_type(valobj, lldb.eBasicTypeVoid).GetPointerType()


def _at(parent, name, address, sbtype):
	"""Creates a child named name that reads a value of type sbtype at address."""
	return parent.CreateValueFromAddress(name, address, sbtype)


def _array_at(parent, name, address, element_type, count):
	return _at(parent, name, address, element_type.GetArrayType(count))


def _renamed(parent, name, valobj, sbtype=None):
	"""Creates a child named name with the value of valobj, optionally reinterpreted as sbtype."""
	if sbtype is None:
		sbtype = valobj.GetType()
	address = valobj.GetLoadAddress()
	if address != lldb.LLDB_INVALID_ADDRESS:
		return _at(parent, name, address, sbtype)
	return parent.CreateValueFromData(name, valobj.GetData(), sbtype)


def _integer(parent, name, value, sbtype):
	"""Creates a child named name holding a computed integer of type sbtype."""
	target = parent.GetTarget()
	size = sbtype.GetByteSize()
	little = target.GetByteOrder() == lldb.eByteOrderLittle
	raw = (value & ((1 << (8 * size)) - 1)).to_bytes(size, "little" if little else "big")
	data = lldb.SBData()
	error = lldb.SBError()
	data.SetData(error, raw, target.GetByteOrder(), target.GetAddressByteSize())
	return parent.CreateValueFromData(name, data, sbtype)


def _read_pointer(valobj, address):
	error = lldb.SBError()
	pointer = valobj.GetProcess().ReadPointerFromMemory(address, error)
	return pointer if error.Success() else 0


def _read_string(valobj, address):
	error = lldb.SBError()
	text = valobj.GetProcess().ReadCStringFromMemory(address, MAX_NAME_LENGTH, error)
	return text if error.Success() else "?"


def _read_byte(valobj, address):
	error = lldb.SBError()
	data = valobj.GetProcess().ReadUnsignedFromMemory(address, 1, error)
	return data if error.Success() else 0


# Formatting shared by summaries and children.

def qualified_name_string(name):
	"""Formats a Bond::QualifiedName as a dotted name."""
	elements = _unsigned(name, "mElements")
	if elements == 0:
		return "<static initializer>"
	pointer_size = name.GetTarget().GetAddressByteSize()
	parts = []
	for i in range(MAX_NAME_ELEMENTS):
		element = _read_pointer(name, elements + i * pointer_size)
		if element == 0:
			break
		parts.append(_read_string(name, element))
	if not parts:
		return "<static initializer>"
	return ".".join(parts)


def signature_type_string(sig):
	"""Formats a ReturnSignature or ParamSignature type, e.g. "int" or "aggregate<16>"."""
	sig_type = _unsigned(sig, "mType")
	name = SIG_NAMES[sig_type] if sig_type < len(SIG_NAMES) else "?%d" % sig_type
	if sig_type == SIG_AGGREGATE:
		name += "<%d>" % _unsigned(sig, "mSize")
	return name


def function_string(function):
	name = qualified_name_string(_member(function, "mName"))
	if _unsigned(function, "mCodeSize") == 0:
		name += " [native]"
	return name


def stack_frame_string(frame):
	function_pointer = _member(frame, "mFunction")
	if function_pointer.GetValueAsUnsigned(0) == 0:
		return "{ no function }"
	function = function_pointer.Dereference()
	name = qualified_name_string(_member(function, "mName"))
	if _unsigned(function, "mCodeSize") == 0:
		return name + " [native]"
	if _unsigned(frame, "mOperandStackPointer") != 0:
		return "%s @ pc %d" % (name, _unsigned(frame, "mPc"))
	return name


def call_stack_frames(stack):
	"""Returns the StackFrames of an AutoStack<StackFrame> from the top down, including the empty
	bottom frame."""
	frames = []
	element = _member(stack, "mTop")
	while element.GetValueAsUnsigned(0) != 0 and len(frames) < MAX_STACK_FRAMES:
		element = element.Dereference()
		frames.append(_member(element, "mValue"))
		element = _member(element, "mNext")
	return frames


def call_stack_string(stack):
	frames = call_stack_frames(stack)
	if not frames or _unsigned(frames[0], "mFunction") == 0:
		return "{ empty }"
	return "{ top=%s }" % stack_frame_string(frames[0])


# Summaries.

def qualified_name_summary(valobj, internal_dict):
	return qualified_name_string(_raw(valobj))


def return_signature_summary(valobj, internal_dict):
	return signature_type_string(_raw(valobj))


def param_signature_summary(valobj, internal_dict):
	sig = _raw(valobj)
	return "%s at fp offset %d" % (signature_type_string(sig), _signed(sig, "mFramePointerOffset"))


def param_list_signature_summary(valobj, internal_dict):
	return "{ count=%d }" % _unsigned(_raw(valobj), "mParamCount")


def function_summary(valobj, internal_dict):
	return function_string(_raw(valobj))


def stack_frame_summary(valobj, internal_dict):
	return stack_frame_string(_raw(valobj))


def invocation_stack_frame_summary(valobj, internal_dict):
	return stack_frame_string(_member(_raw(valobj), "mValue"))


def call_stack_summary(valobj, internal_dict):
	return call_stack_string(_raw(valobj))


def auto_stack_summary(valobj, internal_dict):
	top = _member(_raw(valobj), "mTop")
	if top.GetValueAsUnsigned(0) == 0:
		return "{ empty }"
	value = _member(top.Dereference(), "mValue")
	return "{ top=%s }" % (value.GetSummary() or value.GetValue() or value.GetTypeName())


def auto_stack_element_summary(valobj, internal_dict):
	value = _member(_raw(valobj), "mValue")
	return value.GetSummary() or value.GetValue() or ""


def vm_summary(valobj, internal_dict):
	return call_stack_string(_member(_raw(valobj), "mStackFrames"))


def code_segment_summary(valobj, internal_dict):
	segment = _raw(valobj)
	return "{ functions=%d static initializers=%d data=%d }" % (
		_unsigned(segment, "mFunctionCount"),
		_unsigned(segment, "mStaticInitializerCount"),
		_unsigned(segment, "mDataCount"))


def data_entry_summary(valobj, internal_dict):
	return qualified_name_string(_member(_raw(valobj), "mName"))


def value32_summary(valobj, internal_dict):
	v = _raw(valobj)
	return "{ int=%d float=%s hex=0x%x }" % (
		_signed(v, "mInt"), _member(v, "mFloat").GetValue(), _unsigned(v, "mUInt"))


def value64_summary(valobj, internal_dict):
	v = _raw(valobj)
	as_int = _signed(v, "mLong")
	address = v.GetLoadAddress()
	if address != lldb.LLDB_INVALID_ADDRESS:
		int_value = _at(v, "int", address, _basic_type(v, lldb.eBasicTypeInt))
		as_int = int_value.GetValueAsSigned(0)
	return "{ int=%d long=%d double=%s hex=0x%x }" % (
		as_int, _signed(v, "mLong"), _member(v, "mDouble").GetValue(), _unsigned(v, "mULong"))


# Synthetic children.

class ChildListProvider:
	"""Base class for synthetic children that are computed in update() as a list of SBValues."""

	def __init__(self, valobj, internal_dict):
		self.valobj = valobj
		self.children = []

	def compute_children(self, valobj):
		return []

	def update(self):
		try:
			self.children = self.compute_children(_raw(self.valobj))
		except Exception:
			self.children = []
		return False

	def num_children(self):
		return len(self.children)

	def get_child_index(self, name):
		for i, child in enumerate(self.children):
			if child.GetName() == name:
				return i
		return -1

	def get_child_at_index(self, index):
		if 0 <= index < len(self.children):
			return self.children[index]
		return None

	def has_children(self):
		return True


class ParamListSignatureProvider(ChildListProvider):
	def compute_children(self, params):
		count = _unsigned(params, "mParamCount")
		if count == 0:
			return []
		signatures = _member(params, "mParamSignatures")
		element_type = signatures.GetType().GetPointeeType()
		base = signatures.GetValueAsUnsigned(0)
		size = element_type.GetByteSize()
		return [_at(params, "[%d]" % i, base + i * size, element_type) for i in range(count)]


class FunctionProvider(ChildListProvider):
	def compute_children(self, f):
		code_size = _unsigned(f, "mCodeSize")
		children = [
			_renamed(f, "name", _member(f, "mName")),
			_renamed(f, "return", _member(f, "mReturnSignature")),
			_renamed(f, "params", _member(f, "mParamListSignature")),
		]
		if code_size == 0:
			children.append(_renamed(f, "native function", _member(f, "mNativeFunction")))
		else:
			code = _unsigned(f, "mCode")
			byte_type = _basic_type(f, lldb.eBasicTypeUnsignedChar)
			children.append(_array_at(f, "code", code, byte_type, code_size))
		for name, member in (
				("code size", "mCodeSize"),
				("arg size", "mArgSize"),
				("packed arg size", "mPackedArgSize"),
				("local size", "mLocalSize"),
				("stack size", "mStackSize"),
				("frame pointer alignment", "mFramePointerAlignment"),
				("constant table", "mConstantTable")):
			children.append(_renamed(f, name, _member(f, member)))
		return children


class StackFrameProvider(ChildListProvider):
	"""The activation record of a function. mPc and mOperandStackPointer are only valid while the
	frame is suspended in a call, i.e. for every Bond frame but the top one. The pc of the top frame
	is the pc local variable of the innermost C++ VM::ExecuteScriptFunction frame."""

	def compute_children(self, frame):
		children = []
		function_pointer = _member(frame, "mFunction")
		fp = _unsigned(frame, "mFramePointer")
		osp = _unsigned(frame, "mOperandStackPointer")
		suspended = osp != 0

		if function_pointer.GetValueAsUnsigned(0) != 0:
			function = function_pointer.Dereference()
			children.append(_renamed(frame, "function", function))

			if suspended:
				pc_value = _member(frame, "mPc")
				children.append(_renamed(frame, "pc", pc_value))
				pc = pc_value.GetValueAsUnsigned(0)
				opcode = _read_byte(frame, _unsigned(function, "mCode") + pc)
				opcode_type = frame.GetTarget().FindFirstType("Bond::OpCode")
				if not opcode_type.IsValid():
					opcode_type = _basic_type(frame, lldb.eBasicTypeUnsignedChar)
				children.append(_integer(frame, "instruction", opcode, opcode_type))

			params = _member(function, "mParamListSignature")
			signatures = _member(params, "mParamSignatures")
			sig_type = signatures.GetType().GetPointeeType()
			sig_base = signatures.GetValueAsUnsigned(0)
			for i in range(_unsigned(params, "mParamCount")):
				sig = _at(frame, "sig", sig_base + i * sig_type.GetByteSize(), sig_type)
				children.append(self.arg_value(frame, fp, i, sig))

			local_size = _unsigned(function, "mLocalSize")
			if local_size != 0:
				byte_type = _basic_type(frame, lldb.eBasicTypeUnsignedChar)
				children.append(_array_at(frame, "locals (bytes)", fp, byte_type, local_size))
			if local_size >= 4:
				int_type = _basic_type(frame, lldb.eBasicTypeInt)
				children.append(_array_at(frame, "locals (ints)", fp, int_type, local_size // 4))

		if suspended:
			sp = _unsigned(frame, "mStackPointer")
			num_slots = (osp - sp) // VALUE64_SIZE
			value64_type = frame.GetTarget().FindFirstType("Bond::Value64")
			if num_slots > 0 and value64_type.IsValid():
				children.append(_array_at(frame, "operand stack", sp, value64_type, num_slots))

		void_pointer = _void_pointer_type(frame)
		children.append(_renamed(frame, "frame pointer", _member(frame, "mFramePointer"), void_pointer))
		if _unsigned(frame, "mReturnPointer") != 0:
			children.append(_renamed(frame, "return pointer", _member(frame, "mReturnPointer"), void_pointer))
		children.append(_renamed(frame, "collector", _member(frame, "mCollector")))
		return children

	@staticmethod
	def arg_value(frame, fp, index, sig):
		"""Returns the value of an argument, typed according to its ParamSignature."""
		sig_type = _unsigned(sig, "mType")
		name = "args[%d] %s" % (index, signature_type_string(sig))
		address = fp + _signed(sig, "mFramePointerOffset")
		if sig_type == SIG_AGGREGATE:
			size = max(_unsigned(sig, "mSize"), 1)
			byte_type = _basic_type(frame, lldb.eBasicTypeUnsignedChar)
			return _array_at(frame, name, address, byte_type, size)
		if sig_type == SIG_POINTER:
			return _at(frame, name, address, _void_pointer_type(frame))
		if 0 < sig_type < len(SIG_BASIC_TYPES):
			return _at(frame, name, address, _basic_type(frame, SIG_BASIC_TYPES[sig_type]))
		return _at(frame, name, address, _basic_type(frame, lldb.eBasicTypeUnsignedChar))


class InvocationStackFrameProvider(ChildListProvider):
	def compute_children(self, invocation):
		return [
			_renamed(invocation, "frame", _member(invocation, "mValue")),
			_renamed(invocation, "next arg", _member(invocation, "mNextArg")),
		]


class CallStackProvider(ChildListProvider):
	"""Bond::AutoStack<Bond::StackFrame>: the call stack, from the top down, without the VM's empty
	bottom frame."""

	def compute_children(self, stack):
		frames = [f for f in call_stack_frames(stack) if _unsigned(f, "mFunction") != 0]
		return [_renamed(stack, "[%d]" % i, f) for i, f in enumerate(frames)]


class AutoStackProvider(ChildListProvider):
	def compute_children(self, stack):
		children = []
		element = _member(stack, "mTop")
		while element.GetValueAsUnsigned(0) != 0 and len(children) < MAX_STACK_FRAMES:
			element = element.Dereference()
			children.append(_renamed(stack, "[%d]" % len(children), _member(element, "mValue")))
			element = _member(element, "mNext")
		return children


class AutoStackElementProvider(ChildListProvider):
	def compute_children(self, element):
		value = _member(element, "mValue")
		return [value.GetChildAtIndex(i) for i in range(value.GetNumChildren())]


class VMProvider(ChildListProvider):
	"""The stack base is read from the empty bottom frame, which VM::VM points at mStack, so that the
	internals of std::unique_ptr do not matter."""

	def compute_children(self, vm):
		stack_frames = _member(vm, "mStackFrames")
		stack_base_value = _member(_member(_member(vm, "mDummyFrame"), "mValue"), "mStackPointer")
		stack_base = stack_base_value.GetValueAsUnsigned(0)
		children = [
			_renamed(vm, "call stack", stack_frames),
			_renamed(vm, "code segment", _referent(_member(vm, "mCodeSegment"))),
			_renamed(vm, "stack base", stack_base_value, _void_pointer_type(vm)),
			_renamed(vm, "stack size", _member(vm, "mStackSize")),
		]
		top = _member(stack_frames, "mTop")
		if top.GetValueAsUnsigned(0) != 0:
			top_sp = _unsigned(_member(top.Dereference(), "mValue"), "mStackPointer")
			long_type = _basic_type(vm, lldb.eBasicTypeLongLong)
			children.append(_integer(vm, "stack in use", top_sp - stack_base, long_type))
		children.append(_renamed(vm, "allocator", _referent(_member(vm, "mAllocator"))))
		return children


class CodeSegmentProvider(ChildListProvider):
	def compute_children(self, segment):
		children = []
		for name, table, count in (
				("functions", "mFunctionTable", "mFunctionCount"),
				("static initializers", "mStaticInitializerTable", "mStaticInitializerCount"),
				("data", "mDataTable", "mDataCount")):
			pointer = _member(segment, table)
			n = _unsigned(segment, count)
			if n > 0:
				element_type = pointer.GetType().GetPointeeType()
				children.append(_array_at(segment, name, pointer.GetValueAsUnsigned(0), element_type, n))
			else:
				children.append(_renamed(segment, name, pointer))
		return children


# Registration.

SUMMARIES = (
	# (function, type name, is regex)
	("qualified_name_summary", "Bond::QualifiedName", False),
	("return_signature_summary", "Bond::ReturnSignature", False),
	("param_signature_summary", "Bond::ParamSignature", False),
	("param_list_signature_summary", "Bond::ParamListSignature", False),
	("function_summary", "Bond::Function", False),
	("stack_frame_summary", "Bond::StackFrame", False),
	("invocation_stack_frame_summary", "Bond::InvocationStackFrame", False),
	# Exact names take precedence over regular expressions.
	("call_stack_summary", "Bond::AutoStack<Bond::StackFrame>", False),
	("auto_stack_element_summary", "^Bond::AutoStack<.+>::Element$", True),
	("auto_stack_summary", "^Bond::AutoStack<.+>$", True),
	("vm_summary", "Bond::VM", False),
	("code_segment_summary", "Bond::CodeSegment", False),
	("data_entry_summary", "Bond::DataEntry", False),
	("value32_summary", "Bond::Value32", False),
	("value64_summary", "Bond::Value64", False),
)

SYNTHETICS = (
	# (class, type name, is regex)
	("ParamListSignatureProvider", "Bond::ParamListSignature", False),
	("FunctionProvider", "Bond::Function", False),
	("StackFrameProvider", "Bond::StackFrame", False),
	("InvocationStackFrameProvider", "Bond::InvocationStackFrame", False),
	("CallStackProvider", "Bond::AutoStack<Bond::StackFrame>", False),
	("AutoStackElementProvider", "^Bond::AutoStack<.+>::Element$", True),
	("AutoStackProvider", "^Bond::AutoStack<.+>$", True),
	("VMProvider", "Bond::VM", False),
	("CodeSegmentProvider", "Bond::CodeSegment", False),
)


def __lldb_init_module(debugger, internal_dict):
	module = __name__
	for function, type_name, is_regex in SUMMARIES:
		debugger.HandleCommand('type summary add -w bond %s-F %s.%s "%s"' % (
			"-x " if is_regex else "", module, function, type_name))
	for cls, type_name, is_regex in SYNTHETICS:
		debugger.HandleCommand('type synthetic add -w bond %s-l %s.%s "%s"' % (
			"-x " if is_regex else "", module, cls, type_name))
	debugger.HandleCommand("type category enable bond")
