#include <cstdio>
#include <cctype>
#include <climits>
#include <cstring>

void EmbedFile(FILE *cppFile, FILE *hFile, const char *inputFileName)
{
	char baseName[LINE_MAX];
	const char *n = inputFileName;
	size_t j = 0;
	while ((*n != '\0') && (j < (LINE_MAX - 1)))
	{
		const char c = *n++;
		baseName[j++] = isalnum(c) ? toupper(c) : '_';
	}
	baseName[j] = '\0';

	if (hFile != nullptr)
	{
		fprintf(hFile, "extern const Bond::FileData %s;\n", baseName);
	}

	if (cppFile != nullptr)
	{
		const unsigned BUFFER_SIZE = 16;
		unsigned char buffer[BUFFER_SIZE];
		FILE *inputFile = fopen(inputFileName, "rb");
		if (inputFile != nullptr)
		{
			fprintf(cppFile, "extern const Bond::bu8_t %s_DATA[] =\n{\n", baseName);

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

			fprintf(cppFile, "};\n\nextern const Bond::FileData %s(%s_DATA, sizeof(%s_DATA));\n\n\n", baseName, baseName, baseName);
			fclose(inputFile);
		}
	}
}


int main(int argc, const char *argv[])
{
	const size_t MAX_ENTRIES = 256;
	const char *cppFileName = nullptr;
	const char *hFileName = nullptr;
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

	FILE *cppFile = nullptr;
	FILE *hFile = nullptr;

	if (cppFileName != nullptr)
	{
		cppFile = fopen(cppFileName, "w");
	}

	if (hFileName != nullptr)
	{
		hFile = fopen(hFileName, "w");
	}

	for (size_t i = 0; i < numInputFiles; ++i)
	{
		EmbedFile(cppFile, hFile, inputFileNames[i]);
	}

	fclose(cppFile);
	fclose(hFile);

	return 0;
}
