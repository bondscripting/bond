#ifndef BOND_TEXTWRITER_H
#define BOND_TEXTWRITER_H

namespace Bond
{

class TextWriter
{
public:
	virtual ~TextWriter() {}
	virtual void Write(const char *format, ...) = 0;
};

}

#endif
