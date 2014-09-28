#ifndef BOND_TOOLS_NATIVEBINDINGGENERATOR_H
#define BOND_TOOLS_NATIVEBINDINGGENERATOR_H

#include "bond/conf.h"

namespace Bond
{

class OutputStream;
class TranslationUnit;

class NativeBindingGenerator
{
public:
	void Generate(
		const TranslationUnit *translationUnitList,
		OutputStream &cppStream,
		OutputStream &hStream,
		const char *collectionName,
		const char *includeName);
};

}

#endif
