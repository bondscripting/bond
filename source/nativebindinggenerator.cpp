#include "bond/compiler/parsenodetraverser.h"
#include "bond/compiler/parsenodes.h"
#include "bond/io/outputstream.h"
#include "bond/stl/autostack.h"
#include "bond/tools/nativebindinggenerator.h"
#include "bond/types/simplestring.h"
#include <cctype>

namespace Bond
{

class NativeBindingGeneratorCore: private ParseNodeTraverser
{
public:
	NativeBindingGeneratorCore(
			const TranslationUnit *translationUnitList,
			OutputStream &cppStream,
			OutputStream &hStream,
			const char *collectionName,
			const char *includeName):
		mTranslationUnitList(translationUnitList),
		mCppStream(cppStream),
		mHStream(hStream),
		mCollectionName(collectionName),
		mIncludeName(includeName),
		mNumFunctions(0)
	{}

	virtual ~NativeBindingGeneratorCore() {}

	NativeBindingGeneratorCore(const NativeBindingGeneratorCore &other) = delete;
	NativeBindingGeneratorCore &operator=(const NativeBindingGeneratorCore &other) = delete;

	void Generate();

private:
	struct NamespaceItem
	{
		NamespaceItem(const char *name): mName(name), mPrinted(false) {}
		const char *mName;
		bool mPrinted;
	};
	typedef AutoStack<NamespaceItem> NamespaceStack;

	virtual void Visit(const TranslationUnit *translationUnit) override;
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) override;
	virtual void Visit(const NativeBlock *nativeBlock) override;
	virtual void Visit(const EnumDeclaration *enumDeclaration) override;
	virtual void Visit(const Enumerator *enumerator) override;
	virtual void Visit(const FunctionDefinition *functionDefinition) override;
	virtual void Visit(const NamedInitializer *namedInitializer) override;

	size_t SplitIdentifiers(const char *str, SimpleString *identifiers, size_t maxIdentifiers) const;
	void OpenNamespaces(OutputStream &stream, const SimpleString *identifiers, size_t numIdentifiers);
	void CloseNamespaces(OutputStream &stream, size_t numIdentifiers);
	void PrintNamespaceStack(OutputStream &stream, NamespaceStack::Iterator &it);
	void PrintQualifiedBondName(OutputStream &stream, const Symbol *symbol);
	void PrintQualifiedCppName(OutputStream &stream, const Symbol *symbol);

	NamespaceStack mNamespaceStack;
	BoolStack mInNativeBlockStack;
	const TranslationUnit *mTranslationUnitList;
	OutputStream &mCppStream;
	OutputStream &mHStream;
	const char *mCollectionName;
	const char *mIncludeName;
	uint32_t mNumFunctions;
};


void NativeBindingGenerator::Generate(
	const TranslationUnit *translationUnitList,
	OutputStream &hStream,
	OutputStream &cppStream,
	const char *collectionName,
	const char *includeName)
{
	NativeBindingGeneratorCore generator(translationUnitList, hStream, cppStream, collectionName, includeName);
	generator.Generate();
}


void NativeBindingGeneratorCore::Generate()
{
	BoolStack::Element inNativeBlockElement(mInNativeBlockStack, true);

	const size_t MAX_IDENTIFIER_DEPTH = 128;
	SimpleString identifiers[MAX_IDENTIFIER_DEPTH];
	const size_t numIdentifiers = SplitIdentifiers(mCollectionName, identifiers, MAX_IDENTIFIER_DEPTH);

	if (numIdentifiers > 0)
	{
		const SimpleString &collectionName = identifiers[numIdentifiers - 1];

		// Top of the .h file.
		const size_t BUFFER_SIZE = 1024;
		char includeGuard[BUFFER_SIZE];
		const char *n = mIncludeName;
		size_t i = 0;
		while ((*n != '\0') && (i < (BUFFER_SIZE - 1)))
		{
			const char c = *n++;
			includeGuard[i++] = char(isalnum(c) ? toupper(c) : '_');
		}
		includeGuard[i] = '\0';
		mHStream.Print("#ifndef %s\n#define %s\n\n#include \"bond/api/nativebinding.h\"\n\n", includeGuard, includeGuard);
		OpenNamespaces(mHStream, identifiers, numIdentifiers);
		mHStream.Print("extern const Bond::NativeBindingCollection ");
		collectionName.PrintTo(mHStream);
		mHStream.Print(";\n");
		CloseNamespaces(mHStream, numIdentifiers);
		mHStream.Print("\n");

		// Top of the .cpp file.
		mCppStream.Print("#include \"%s\"\n\n", mIncludeName);
		OpenNamespaces(mCppStream, identifiers, numIdentifiers);
		mCppStream.Print("\nconst Bond::NativeFunctionBinding ");
		collectionName.PrintTo(mCppStream);
		mCppStream.Print("_FUNCTIONS[] =\n{\n");

		// Spit out the function bindings.
		TraverseList(mTranslationUnitList);

		// Bottom of the .h file.
		mHStream.Print("\n#endif\n");

		// Bottom of the .cpp file.
		mCppStream.Print("\t{nullptr, nullptr}\n};\n\nconst Bond::NativeBindingCollection ");
		collectionName.PrintTo(mCppStream);
		mCppStream.Print(" =\n{\n\t");
		collectionName.PrintTo(mCppStream);
		mCppStream.Print("_FUNCTIONS,\n\t%" BOND_PRIu32 "\n};\n\n", mNumFunctions);
		CloseNamespaces(mCppStream, numIdentifiers);
	}
}


void NativeBindingGeneratorCore::Visit(const TranslationUnit *translationUnit)
{
	if (translationUnit->RequiresCodeGeneration())
	{
		ParseNodeTraverser::Visit(translationUnit);
	}
}


void NativeBindingGeneratorCore::Visit(const NamespaceDefinition *namespaceDefinition)
{
	const char *name = namespaceDefinition->GetName()->GetRawText();
	NamespaceStack::Element namespaceItem(mNamespaceStack, NamespaceItem(name));
	ParseNodeTraverser::Visit(namespaceDefinition);

	if (namespaceItem.GetValue().mPrinted)
	{
		mHStream.Print("}\n");
	}
}


void NativeBindingGeneratorCore::Visit(const NativeBlock *nativeBlock)
{
	BoolStack::Element inNativeBlockElement(mInNativeBlockStack, true);
	ParseNodeTraverser::Visit(nativeBlock);
}


void NativeBindingGeneratorCore::Visit(const EnumDeclaration *enumDeclaration)
{
	if (mInNativeBlockStack.GetTop())
	{
		NamespaceStack::Iterator it = mNamespaceStack.begin();
		PrintNamespaceStack(mHStream, it);

		mHStream.Print("enum %s\n{\n", enumDeclaration->GetName()->GetRawText());
		TraverseList(enumDeclaration->GetEnumeratorList());
		mHStream.Print("};\n");
	}
}


void NativeBindingGeneratorCore::Visit(const Enumerator *enumerator)
{
	mHStream.Print("\t%s = %" BOND_PRId32 ",\n", enumerator->GetName()->GetRawText(), enumerator->GetTypeAndValue()->GetIntValue());
}


void NativeBindingGeneratorCore::Visit(const FunctionDefinition *functionDefinition)
{
	if (functionDefinition->IsNative())
	{
		NamespaceStack::Iterator it = mNamespaceStack.begin();
		PrintNamespaceStack(mHStream, it);

		// Generate the function prototype.
		mHStream.Print("void ");
		if (functionDefinition->GetScope() == SCOPE_STRUCT_MEMBER)
		{
			mHStream.Print("%s__", functionDefinition->GetParentSymbol()->GetName()->GetRawText());
		}
		mHStream.Print("%s(Bond::CalleeStackFrame &frame);\n", functionDefinition->GetName()->GetRawText());

		// Generate the function binding.
		mCppStream.Print("\t{\"");
		PrintQualifiedBondName(mCppStream, functionDefinition);
		mCppStream.Print("\", ");
		PrintQualifiedCppName(mCppStream, functionDefinition);
		mCppStream.Print("},\n");
		++mNumFunctions;
	}
}


void NativeBindingGeneratorCore::Visit(const NamedInitializer *namedInitializer)
{
	if (namedInitializer->IsNativeStructMember())
	{
		NamespaceStack::Iterator it = mNamespaceStack.begin();
		PrintNamespaceStack(mHStream, it);

		// Generate the getter and setter function prototypes.
		const char *structName = namedInitializer->GetParentSymbol()->GetName()->GetRawText();
		const char *memberName = namedInitializer->GetName()->GetRawText();
		mHStream.Print("void %s__%s__get(Bond::CalleeStackFrame &frame);\n", structName, memberName);
		mHStream.Print("void %s__%s__set(Bond::CalleeStackFrame &frame);\n", structName, memberName);

		// Generate the function bindings.
		mCppStream.Print("\t{\"");
		PrintQualifiedBondName(mCppStream, namedInitializer);
		mCppStream.Print(".%s\", ", BOND_NATIVE_GETTER_SUFFIX);
		PrintQualifiedCppName(mCppStream, namedInitializer);
		mCppStream.Print("__get},\n");

		mCppStream.Print("\t{\"");
		PrintQualifiedBondName(mCppStream, namedInitializer);
		mCppStream.Print(".%s\", ", BOND_NATIVE_SETTER_SUFFIX);
		PrintQualifiedCppName(mCppStream, namedInitializer);
		mCppStream.Print("__set},\n");

		mNumFunctions += 2;
	}
}


size_t NativeBindingGeneratorCore::SplitIdentifiers(const char *str, SimpleString *identifiers, size_t maxIdentifiers) const
{
	bool inIdentifier = false;
	const char *start = str;
	const char *next = str;
	size_t numIdentifiers = 0;
	while ((*next != '\0') && (numIdentifiers < maxIdentifiers))
	{
		const char c = *next;
		const bool isIdentifierChar = isalnum(c) || (c == '_');
		if (inIdentifier)
		{
			if (!isIdentifierChar)
			{
				identifiers[numIdentifiers++] = SimpleString(start, next - start);
				inIdentifier = false;
			}
		}
		else
		{
			if (isIdentifierChar)
			{
				start = next;
				inIdentifier = true;
			}
		}
		++next;
	}

	if (inIdentifier)
	{
		identifiers[numIdentifiers++] = SimpleString(start, next - start);
	}

	return numIdentifiers;
}


void NativeBindingGeneratorCore::OpenNamespaces(OutputStream &stream, const SimpleString *identifiers, size_t numIdentifiers)
{
	if (numIdentifiers > 1)
	{
		for (size_t i = 0; i < (numIdentifiers - 1); ++i)
		{
			stream.Print("namespace ");
			identifiers[i].PrintTo(stream);
			stream.Print("\n{\n");
		}
	}
}


void NativeBindingGeneratorCore::CloseNamespaces(OutputStream &stream, size_t numIdentifiers)
{
	if (numIdentifiers > 1)
	{
		for (size_t i = 0; i < (numIdentifiers - 1); ++i)
		{
			stream.Print("}\n");
		}
	}
}


void NativeBindingGeneratorCore::PrintNamespaceStack(OutputStream &stream, NamespaceStack::Iterator &it)
{
	if ((it != mNamespaceStack.end()) && !it->mPrinted)
	{
		NamespaceItem &item = *it++;
		PrintNamespaceStack(stream, it);
		stream.Print("namespace %s\n{\n", item.mName);
		item.mPrinted = true;
	}
}


void NativeBindingGeneratorCore::PrintQualifiedBondName(OutputStream &stream, const Symbol *symbol)
{
	if ((symbol != nullptr) && (symbol->GetName() != nullptr))
	{
		const Symbol *parent = symbol->GetParentSymbol();
		if ((parent != nullptr) && (parent->GetName() != nullptr))
		{
			PrintQualifiedBondName(stream, parent);
			stream.Print(".");
		}
		stream.Print("%s", symbol->GetName()->GetRawText());
	}
}


void NativeBindingGeneratorCore::PrintQualifiedCppName(OutputStream &stream, const Symbol *symbol)
{
	if ((symbol != nullptr) && (symbol->GetName() != nullptr))
	{
		PrintQualifiedCppName(stream, symbol->GetParentSymbol());
		const char *suffix = "";
		switch (symbol->GetSymbolType())
		{
			case Symbol::TYPE_NAMESPACE:
				suffix = "::";
				break;
			case Symbol::TYPE_STRUCT:
				suffix = "__";
				break;
			default:
				suffix = "";
				break;
		}
		stream.Print("%s%s", symbol->GetName()->GetRawText(), suffix);
	}
}

}
