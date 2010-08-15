#ifndef BOND_PARSENODES_H
#define BOND_PARSENODES_H

#include "bond/token.h"

namespace Bond
{

class ParseNode;
class TranslationUnit;
class ExternalDeclaration;
class NamespaceDefinition;
class EnumDeclaration;
class Enumerator;


class ParseNode
{
public:
	ParseNode() {}
	virtual ~ParseNode() {}
};


class TranslationUnit: public ParseNode
{
public:
  TranslationUnit(): mDeclarations(0) {}
  TranslationUnit(ExternalDeclaration *declarations): mDeclarations(declarations) {}
	virtual ~TranslationUnit() {}

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarations; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarations; }
	//void SetExternalDeclarationList(ExternalDeclaration *declarations) { mDeclarations = declarations; }

private:
	ExternalDeclaration *mDeclarations;
};


class ExternalDeclaration: public ParseNode
{
public:
	ExternalDeclaration(): mName(0), mNext(0) {}
	ExternalDeclaration(const Token *name): mName(name), mNext(0) {}
	virtual ~ExternalDeclaration() {}

	const Token *mName;
	const Token *GetName() const { return mName; }
	void SetName(const Token *name) { mName = name; }

	ExternalDeclaration *GetNext() { return mNext; }
	const ExternalDeclaration *GetNext() const { return mNext; }
	void SetNext(ExternalDeclaration *next) { mNext = next; }

private:
	const Token *name;
	ExternalDeclaration *mNext;
};


class NamespaceDefinition: public ExternalDeclaration
{
public:
	NamespaceDefinition(): mDeclarations(0) {}

 NamespaceDefinition(const Token *name, ExternalDeclaration *declarations):
		ExternalDeclaration(name),
	 	mDeclarations(declarations)
	{}

	virtual ~NamespaceDefinition() {}

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarations; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarations; }

private:
	ExternalDeclaration *mDeclarations;
};


class EnumDeclaration: public ExternalDeclaration
{
public:
	EnumDeclaration(): mEnumerators(0) {}

 EnumDeclaration(const Token *name, Enumerator *enumerators):
		ExternalDeclaration(name),
	 	mEnumerators(enumerators)
	{}

	virtual ~EnumDeclaration() {}

	Enumerator *GetEnumeratorList() { return mEnumerators; }
	const Enumerator *GetEnumeratorList() const { return mEnumerators; }

private:
	Enumerator *mEnumerators;
};


class Enumerator: public ParseNode
{
public:
	Enumerator(): mName(0), mValue(0), mNext(0) {}
	Enumerator(const Token *name, int_t value): mName(name), mValue(value), mNext(0) {}
	virtual ~Enumerator() {}

	Enumerator *GetNext() { return mNext; }
	const Enumerator *GetNext() const { return mNext; }
	void SetNext(Enumerator *next) { mNext = next; }

private:
	const Token *mName;
	int_t mValue;
	Enumerator *mNext;
};

}

#endif
