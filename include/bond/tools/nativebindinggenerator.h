#ifndef BOND_TOOLS_NATIVEBINDINGGENERATOR_H
#define BOND_TOOLS_NATIVEBINDINGGENERATOR_H

#include "bond/conf.h"

namespace Bond
{

class TextWriter;
class TranslationUnit;

class NativeBindingGenerator
{
public:
	void Generate(
		const TranslationUnit *translationUnitList,
		TextWriter &cppWriter,
		TextWriter &hWriter,
		const char *collectionName,
		const char *includeName);
};

}

#endif
