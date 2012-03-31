#ifndef BOND_CBOPARSER_H
#define BOND_CBOPARSER_H

#include "bond/types.h"

namespace Bond
{

class CboHandler;

class CboParser
{
public:
	void Parse(CboHandler &handler, unsigned char *byteCode, size_t length);
};

}

#endif
