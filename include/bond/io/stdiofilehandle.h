#ifndef BOND_IO_STDIOFILEHANDLE_H
#define BOND_IO_STDIOFILEHANDLE_H

#include <cstdio>

namespace Bond
{

/// \brief A handle to a stdio FILE* to ensure that the file is properly disposed using RAII.
/// \ingroup io
class StdioFileHandle
{
public:
	/// \brief Constructs an empty handle that is not bound to a file.
	StdioFileHandle():
		mFile(nullptr)
	{}

	/// \brief Constructs a handle that takes ownership of the given FILE pointer.
	/// \param file Pointer to an open FILE object to be owned by this handle. May be null.
	explicit StdioFileHandle(FILE *file):
		mFile(file)
	{}

	/// \brief Opens a file and constructs a handle that owns the resulting FILE pointer.
	/// \param fileName The path to the file to open.
	/// \param mode The fopen mode string used to open the file.
	/// \remarks If fopen fails, the handle remains unbound.
	StdioFileHandle(const char *fileName, const char *mode):
		mFile(fopen(fileName, mode))
	{}

	/// \brief Move-constructs a handle by transferring ownership from another handle.
	/// \param other Source handle whose ownership is transferred.
	/// \remarks After construction, other is left unbound.
	StdioFileHandle(StdioFileHandle &&other) noexcept:
		mFile(other.mFile)
	{
		other.mFile = nullptr;
	}

	/// \brief Destroys the handle and closes the owned file, if any.
	~StdioFileHandle()
	{
		Reset();
	}

	/// \brief Move-assigns by closing any currently owned file and taking ownership from another handle.
	/// \param other Source handle whose ownership is transferred.
	/// \returns A reference to this handle.
	/// \remarks After assignment, other is left unbound.
	StdioFileHandle &operator=(StdioFileHandle &&other) noexcept
	{
		if (this != &other)
		{
			Reset(other.Release());
		}
		return *this;
	}

	/// \brief Rebinds the handle to the given FILE pointer.
	/// \param file Pointer to an open FILE object to be owned by this handle. May be null.
	/// \returns A reference to this handle.
	/// \remarks Any previously owned file is closed first, unless file equals the currently owned pointer.
	StdioFileHandle &operator=(FILE *file)
	{
		Reset(file);
		return *this;
	}

	StdioFileHandle(const StdioFileHandle &other) = delete;
	StdioFileHandle &operator=(const StdioFileHandle &other) = delete;

	/// \brief Returns the currently owned FILE pointer.
	/// \returns The owned FILE pointer, or null if no file is bound.
	FILE *GetFile() const { return mFile; }

	/// \brief Releases ownership of the current FILE pointer without closing it.
	/// \returns The previously owned FILE pointer, or null if no file is bound.
	/// \remarks After this call, the handle is unbound and will not close the returned pointer.
	FILE *Release()
	{
		FILE *file = mFile;
		mFile = nullptr;
		return file;
	}

	/// \brief Replaces the owned FILE pointer, closing the previously owned file if needed.
	/// \param file New FILE pointer to own. Defaults to null, which simply unbinds the handle.
	/// \remarks If file equals the currently owned pointer, no close is performed.
	void Reset(FILE *file = nullptr)
	{
		if ((mFile != nullptr) && (mFile != file))
		{
			fclose(mFile);
		}
		mFile = file;
	}

	/// \brief Indicates whether this handle currently owns a FILE pointer.
	/// \returns True if a file is bound; otherwise false.
	bool IsBound() const { return mFile != nullptr; }

private:
	FILE *mFile;
};

}

#endif
