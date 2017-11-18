/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#ifndef CSJP_FILE_H
#define CSJP_FILE_H

#include <stdio.h>

#include <csjp_string.h>

namespace csjp {

/** File Operations
 *
 * File operations are fragile. It is not possible to make them fully transactional.
 * There are three main problems.
 * 1) Writing a file is not transactional by nature.
 * 2) Closing a file is not garanteed to be successfull.
 * 3) Caching of file operations by the underlying system.
 *
 * On Writting
 *	A write operation might be only partially successfull, which is clearly
 *	not a transactional behaviour. For transactionality, we try to call
 *	write multiple times, until either all data is written or some failure
 *	happens.
 *	On failure, we try to revert the partial result, by truncating the
 *	file. For this, we use the previous filesize, which is queried by
 *	openForWrite(), and we also need to close the file before truncating.
 *	However, this might fail, in which case we log a critical and add a
 *	note to the exception generated for the failed write.
 *
 * On Closing
 *	When destructing a file, or doing some operation in order to keep
 *	consistency, we just have to close the file successfully. However,
 *	there is no garantee for successfull close. So, as a workaround, in
 *	destructor we pretend the file as closed. This is still better then
 *	throwing from destructor and causing the software to abort.
 *
 * On Caching
 *	Somewhat fixable, but current implementation does not care about it.  FIXME
 *
 * FIXME: lets make write operations exclusive, direct and synchronized.
 */
class File
{
public:
	static String readAll(const Str & fileName) { File f(fileName); return f.readAll();}

public:
	explicit File() = delete;
	explicit File(const File & orig) = delete;
	const File & operator=(const File &) = delete;

	File(File && temp) :
		file(temp.file),
		writable(temp.writable),
		eofbit(temp.eofbit),
		locked(temp.locked),
		fileSize(temp.fileSize),
		fileName(move_cast(temp.fileName))
	{
		temp.file = 0;
		temp.writable = false;
		temp.eofbit = false;
		temp.locked = false;
		temp.fileSize = 0;
	}
	const File & operator=(File && temp)
	{
		file = temp.file;
		writable = temp.writable;
		eofbit = temp.eofbit;
		locked = temp.locked;
		fileSize = temp.fileSize;
		fileName = move_cast(temp.fileName);

		temp.file = 0;
		temp.writable = false;
		temp.eofbit = false;
		temp.locked = false;
		temp.fileSize = 0;
		temp.fileName.clear();

		return *this;
	}

	explicit File(const Str & fileName);
	virtual ~File();
#if 0
	void lock(off_t length = 0);
	void unlock();
#endif
	const String & name() const;
	bool exists() const;
	bool isRegular() const;
	bool isDir() const;
	long unsigned size() const;
/* FIXME: Things to implement here:
	void lastModified(DateTime & dt) const;
	void createdAt(DateTime & dt) const;
	void owner(String & userName) const;
	void group(String & groupName) const;
	void execute() const; ??? evaluate data ???

	void moveTo(String & path);*/
	void rename(const Str & name);

	void resize(long unsigned size);
	void create();
	void createDir();
	void removeDir();
	void unlink();

	void openForRead() const;
	void openForWrite();
	inline void close() const {close(true);}
	bool eof() const;
	void rewind() const;

	String readAll() const;
	String read(long unsigned bytes) const;
	String readAllFromPos(long unsigned pos) const;
	String readFromPos(long unsigned pos, long unsigned bytes) const;
/* FIXME : getline() system call is not supported on android-9
	void getLine(String & buffer) const;*/

	void write(const Str & data);
	void writeAtPos(const Str & data, long unsigned pos);
	void append(const Str & data);
	void appendPrintf(const char * format, ...)
			// We need to use 2, 3 because 'this' pointer is the first parameter.
			__attribute__ ((format (printf, 2, 3)));
	template<typename... Args> void appendf(const char * fmt, const Args & ... args)
		{ String buf; buf.catf(fmt, args...); append(buf); }
	void overWrite(const Str & data) { if(exists()) resize(0); write(data); }

protected:
	void close(bool throws) const;

	mutable int file;
	mutable bool writable;
	mutable bool eofbit;
	bool locked;

	/* Should only used by write() to avoid file size check before each write operation. */
	long unsigned fileSize;

	String fileName;
};

class TempFile : public File
{
public:
	/** Param templateFileName should have such form: temp_file_XXXXXX,
	 * where "temp_file_" will remain as is and last six characters
	 * of template must be "XXXXXX" that will be changed by generated
	 * random such that the resulted file name will be unique.
	 * For details see linux man page mkstemp in section 3.
	 */
	explicit TempFile(const Str & templateFileName);
	explicit TempFile(const char * templateFileName);
	virtual ~TempFile();

	explicit TempFile() = delete;
};

}

#endif
