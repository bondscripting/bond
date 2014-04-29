#include "bond/compiler/parsenodetraverser.h"
#include "bond/compiler/parsenodes.h"
#include "bond/io/textwriter.h"
#include "bond/stl/autostack.h"
#include "bond/tools/nativebindinggenerator.h"
#include "bond/types/simplestring.h"
#include <ctype.h>

namespace Bond
{

class NativeBindingGeneratorCore: private ParseNodeTraverser
{
public:
	NativeBindingGeneratorCore(
			const TranslationUnit *translationUnitList,
			TextWriter &cppWriter,
			TextWriter &hWriter,
			const char *collectionName,
			const char *includeName):
		mTranslationUnitList(translationUnitList),
		mCppWriter(cppWriter),
		mHWriter(hWriter),
		mCollectionName(collectionName),
		mIncludeName(includeName),
		mNumFunctions(0)
	{}

	virtual ~NativeBindingGeneratorCore() {}

	void Generate();

private:
	struct NamespaceItem
	{
		NamespaceItem(const char *name): mName(name), mPrinted(false) {}
		const char *mName;
		bool mPrinted;
	};
	typedef AutoStack<NamespaceItem> NamespaceStack;

	virtual void Visit(const TranslationUnit *translationUnit);
	virtual void Visit(const NamespaceDefinition *namespaceDefinition);
	virtual void Visit(const EnumDeclaration *enumDeclaration) {}
	virtual void Visit(const FunctionDefinition *functionDefinition);
	virtual void Visit(const NamedInitializer *namedInitializer);

	size_t SplitIdentifiers(const char *str, SimpleString *identifiers, size_t maxIdentifiers) const;
	void OpenNamespaces(TextWriter &writer, const SimpleString *identifiers, size_t numIdentifiers);
	void CloseNamespaces(TextWriter &writer, size_t numIdentifiers);
	void PrintNamespaceStack(TextWriter &writer, NamespaceStack::Iterator &it);
	void PrintQualifiedSymbolName(TextWriter &writer, const Symbol *symbol, const char *prefix = "");

	NamespaceStack mNamespaceStack;
	const TranslationUnit *mTranslationUnitList;
	TextWriter &mCppWriter;
	TextWriter &mHWriter;
	const char *mCollectionName;
	const char *mIncludeName;
	bu32_t mNumFunctions;
};


void NativeBindingGenerator::Generate(
	const TranslationUnit *translationUnitList,
	TextWriter &hWriter,
	TextWriter &cppWriter,
	const char *collectionName,
	const char *includeName)
{
	NativeBindingGeneratorCore generator(translationUnitList, hWriter, cppWriter, collectionName, includeName);
	generator.Generate();
}


void NativeBindingGeneratorCore::Generate()
{
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
			includeGuard[i++] = isalnum(c) ? toupper(c) : '_';
		}
		includeGuard[i] = '\0';
		mHWriter.Write("#ifndef %s\n#define %s\n\n#include \"bond/api/nativebinding.h\"\n\n", includeGuard, includeGuard);
		OpenNamespaces(mHWriter, identifiers, numIdentifiers);
		mHWriter.Write("extern const Bond::NativeBindingCollection ");
		WriteString(mHWriter, collectionName);
		mHWriter.Write(";\n");
		CloseNamespaces(mHWriter, numIdentifiers);
		mHWriter.Write("\n");

		// Top of the .cpp file.
		mCppWriter.Write("#include \"%s\"\n\n", mIncludeName);
		OpenNamespaces(mCppWriter, identifiers, numIdentifiers);
		mCppWriter.Write("\nconst Bond::NativeFunctionBinding ");
		WriteString(mCppWriter, collectionName);
		mCppWriter.Write("_FUNCTIONS[] =\n{\n");

		// Spit out the function bindings.
		TraverseList(mTranslationUnitList);

		// Bottom of the .h file.
		mHWriter.Write("\n#endif\n");

		// Bottom of the .cpp file.
		mCppWriter.Write("\t{0, NULL}\n};\n\nconst Bond::NativeBindingCollection ");
		WriteString(mCppWriter, collectionName);
		mCppWriter.Write(" =\n{\n\t");
		WriteString(mCppWriter, collectionName);
		mCppWriter.Write("_FUNCTIONS,\n\t%" BOND_PRIu32 "\n};\n\n", mNumFunctions);
		CloseNamespaces(mCppWriter, numIdentifiers);
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
	const char *name = namespaceDefinition->GetName()->GetText();
	NamespaceStack::Element namespaceItem(mNamespaceStack, NamespaceItem(name));
	ParseNodeTraverser::Visit(namespaceDefinition);

	if (namespaceItem.GetValue().mPrinted)
	{
		mHWriter.Write("}\n");
	}
}


void NativeBindingGeneratorCore::Visit(const FunctionDefinition *functionDefinition)
{
	if (functionDefinition->IsNative())
	{
		NamespaceStack::Iterator it = mNamespaceStack.Begin();
		PrintNamespaceStack(mHWriter, it);

		// Generate the function prototype.
		mHWriter.Write("void ");
		if (functionDefinition->GetScope() == SCOPE_STRUCT_MEMBER)
		{
			mHWriter.Write("%s__", functionDefinition->GetParentSymbol()->GetName()->GetText());
		}
		mHWriter.Write("%s(Bond::CalleeStackFrame &frame);\n", functionDefinition->GetName()->GetText());

		// Generate the function binding.
		mCppWriter.Write("\t{0x%08" BOND_PRIx32 ", ", functionDefinition->GetGlobalHashCode());
		PrintQualifiedSymbolName(mCppWriter, functionDefinition);
		mCppWriter.Write("},\n");
		++mNumFunctions;
	}
}


void NativeBindingGeneratorCore::Visit(const NamedInitializer *namedInitializer)
{
	if (namedInitializer->IsNativeStructMember())
	{
		NamespaceStack::Iterator it = mNamespaceStack.Begin();
		PrintNamespaceStack(mHWriter, it);

		// Generate the getter and setter function prototypes.
		const char *structName = namedInitializer->GetParentSymbol()->GetName()->GetText();
		const char *memberName = namedInitializer->GetName()->GetText();
		mHWriter.Write("void %s__get__%s(Bond::CalleeStackFrame &frame);\n", structName, memberName);
		mHWriter.Write("void %s__set__%s(Bond::CalleeStackFrame &frame);\n", structName, memberName);

		// Generate the function bindings.
		mCppWriter.Write("\t{0x%08" BOND_PRIx32 ", ", namedInitializer->GetGlobalHashCodeWithPrefix("@get_"));
		PrintQualifiedSymbolName(mCppWriter, namedInitializer, "get__");
		mCppWriter.Write("},\n");
		mCppWriter.Write("\t{0x%08" BOND_PRIx32 ", ", namedInitializer->GetGlobalHashCodeWithPrefix("@set_"));
		PrintQualifiedSymbolName(mCppWriter, namedInitializer, "set__");
		mCppWriter.Write("},\n");
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


void NativeBindingGeneratorCore::OpenNamespaces(TextWriter &writer, const SimpleString *identifiers, size_t numIdentifiers)
{
	if (numIdentifiers > 1)
	{
		for (size_t i = 0; i < (numIdentifiers - 1); ++i)
		{
			writer.Write("namespace ");
			WriteString(writer, identifiers[i]);
			writer.Write("\n{\n");
		}
	}
}


void NativeBindingGeneratorCore::CloseNamespaces(TextWriter &writer, size_t numIdentifiers)
{
	if (numIdentifiers > 1)
	{
		for (size_t i = 0; i < (numIdentifiers - 1); ++i)
		{
			writer.Write("}\n");
		}
	}
}


void NativeBindingGeneratorCore::PrintNamespaceStack(TextWriter &writer, NamespaceStack::Iterator &it)
{
	if ((it != mNamespaceStack.End()) && !it->mPrinted)
	{
		NamespaceItem &item = *it++;
		PrintNamespaceStack(writer, it);
		writer.Write("namespace %s\n{\n", item.mName);
		item.mPrinted = true;
	}
}


void NativeBindingGeneratorCore::PrintQualifiedSymbolName(TextWriter &writer, const Symbol *symbol, const char *prefix)
{
	if ((symbol != NULL) && (symbol->GetName() != NULL))
	{
		PrintQualifiedSymbolName(writer, symbol->GetParentSymbol());
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
		writer.Write("%s%s%s", prefix, symbol->GetName()->GetText(), suffix);
	}
}

}
