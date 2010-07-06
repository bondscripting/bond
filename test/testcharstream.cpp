#include <stdio.h>
#include "bond/charstream.h"

int main()
{
	const char text[] = "ab\n\nc\nd";
	Bond::CharStream cs(text, sizeof(text) - 1);

	while (cs.HasNext())
	{
		const Bond::StreamPos pos = cs.GetStreamPos();
		const char c = cs.Next();
		printf("%3d%3d%3d '%c'\n", pos.index, pos.line, pos.column, c);
	}
	{
		cs.Unget(2);
		const Bond::StreamPos pos = cs.GetStreamPos();
		const char c = cs.Peek();
		printf("%3d%3d%3d '%c'\n", pos.index, pos.line, pos.column, c);
	}
	{
		cs.Unget(1);
		const Bond::StreamPos pos = cs.GetStreamPos();
		const char c = cs.Peek();
		printf("%3d%3d%3d '%c'\n", pos.index, pos.line, pos.column, c);
	}
	{
		cs.Unget(1);
		const Bond::StreamPos pos = cs.GetStreamPos();
		const char c = cs.Peek();
		printf("%3d%3d%3d '%c'\n", pos.index, pos.line, pos.column, c);
	}
	{
		cs.Unget(3);
		const Bond::StreamPos pos = cs.GetStreamPos();
		const char c = cs.Peek();
		printf("%3d%3d%3d '%c'\n", pos.index, pos.line, pos.column, c);
	}

	return 0;
}
