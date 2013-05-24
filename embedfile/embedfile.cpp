#include <ctype.h>
#include <stdio.h>
#include <string.h>

void EmbedFile(FILE *cppFile, FILE *hFile, const char *inputFileName)
{
	const size_t MAX_NAME_LENGTH = 512;
	char baseName[MAX_NAME_LENGTH];
	const char *n = inputFileName;
	size_t j = 0;
	while ((*n != '\0') && (j < (MAX_NAME_LENGTH - 1)))
	{
		const char c = *n++;
		baseName[j++] = isalnum(c) ? toupper(c) : '_';
	}
	baseName[j] = '\0';

	const unsigned BUFFER_SIZE = 16;
	unsigned char buffer[BUFFER_SIZE];
	FILE* inputFile = fopen(inputFileName, "rb");

	fprintf(cppFile, "\nextern const Bond::bu8_t %s_DATA[] =\n{\n", baseName);

	while (!feof(inputFile))
	{
		const size_t numBytes = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, inputFile);

		if (numBytes > 0)
		{
			for (size_t i = 0; i < numBytes; ++i)
			{
				fprintf(cppFile, "%c0x%02X,", (i > 0) ? ' ' : '\t', buffer[i]);
			}
			fprintf(cppFile, "\n");
		}
	}

	fprintf(cppFile, "};\n\nextern const Bond::FileData %s(%s_DATA, sizeof(%s_DATA));\n", baseName, baseName, baseName);
	fprintf(hFile, "\nextern const Bond::FileData %s;\n", baseName);
	fclose(inputFile);
}

int main(int argc, const char *argv[])
{
	const size_t MAX_ENTRIES = 256;
	const char *cppFileName = "embeddedfile.cpp";
	const char *hFileName = "embeddedfile.h";
	const char *includeFileName = "embeddedfile.h";
	const char *namespaces[MAX_ENTRIES];
	size_t numNamespaces = 0;
	const char *inputFileNames[MAX_ENTRIES];
	size_t numInputFiles = 0;

	bool error = false;
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-c") == 0)
		{
			if (++i < argc)
			{
				cppFileName = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -c\n");
				error = true;
			}
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			if (++i < argc)
			{
				hFileName = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -h\n");
				error = true;
			}
		}
		else if (strcmp(argv[i], "-i") == 0)
		{
			if (++i < argc)
			{
				includeFileName = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -i\n");
				error = true;
			}
		}
		else if (strcmp(argv[i], "-n") == 0)
		{
			if (++i < argc)
			{
				if (numNamespaces < MAX_ENTRIES)
				{
					namespaces[numNamespaces++] = argv[i];
				}
			}
			else
			{
				fprintf(stderr, "Missing argument to -n\n");
				error = true;
			}
		}
		else if (argv[i][0] == '-')
		{
			fprintf(stderr, "Unknown option '%s'\n", argv[i]);
			error = true;
		}
		else if (numInputFiles < MAX_ENTRIES)
		{
			inputFileNames[numInputFiles++] = argv[i];
		}
	}

	if (error)
	{
		return 1;
	}

	FILE* cppFile = fopen(cppFileName, "w");
	FILE* hFile = fopen(hFileName, "w");

	if ((cppFile == NULL) || (hFile == NULL))
	{
		return 1;
	}

	fprintf(cppFile, "#include \"%s\"\n", includeFileName);
	fprintf(hFile, "#include \"bond/io/fileloader.h\"\n");

	if (numNamespaces > 0)
	{
		fprintf(cppFile, "\n");
		fprintf(hFile, "\n");

		for (size_t i = 0; i < numNamespaces; ++i)
		{
			fprintf(cppFile, "namespace %s\n{\n", namespaces[i]);
			fprintf(hFile, "namespace %s\n{\n", namespaces[i]);
		}
	}

	for (size_t i = 0; i < numInputFiles; ++i)
	{
		EmbedFile(cppFile, hFile, inputFileNames[i]);
	}

	if (numNamespaces > 0)
	{
		fprintf(cppFile, "\n");
		fprintf(hFile, "\n");

		for (size_t i = 0; i < numNamespaces; ++i)
		{
			fprintf(cppFile, "}\n");
			fprintf(hFile, "}\n");
		}
	}

	fclose(cppFile);
	fclose(hFile);

	return 0;
}
