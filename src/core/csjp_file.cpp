/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h> // FIXME replace with system call

#include <string.h>

#include <csjp_string.h>
#include <csjp_carray.h>

#include "csjp_file.h"

namespace csjp {

File::File(const char * fileName) :
	file(NULL),
	writable(false),
	fileSize(0),
	fileName(fileName)
{
}

File::File(const String & fileName) :
	file(NULL),
	writable(false),
	fileSize(0),
	fileName(fileName)
{
}

File::File(const StringChunk & fileName) :
	file(NULL),
	writable(false),
	fileSize(0),
	fileName(fileName.str, fileName.length)
{
}

File::~File()
{
	if(file)
		close(false);
}

const String& File::name() const
{
	return fileName;
}

bool File::exists() const
{
	struct stat fileStat;
	int err = 0;
	if(stat(fileName.str, &fileStat) < 0){
		err = errno;
		if(err == ENOENT || err == ENOTDIR)
			return false;
		throw FileError(errno, "Could not check for the existence of file (%).",
				fileName.str);
	}
	return true;
}

bool File::isRegular() const
{
	struct stat fileStat;
	if(stat(fileName.str, &fileStat) < 0)
		throw FileError(errno, "Could not check if file (%) is regular file.",
				fileName.str);

	if(S_ISREG(fileStat.st_mode))
		return true;
	return false;
}

bool File::isDir() const
{
	struct stat fileStat;
	if(stat(fileName.str, &fileStat) < 0)
		throw FileError(errno, "Could not check if (%) is directory.", fileName.str);

	if(S_ISDIR(fileStat.st_mode))
		return true;
	return false;
}

long unsigned File::size() const
{
	if(!file)
		openForRead();

	clearerr(file);
	long lastPos = 0;
	lastPos = ftell(file);
	if(lastPos < 0)
		throw FileError(errno, "Could not use ftell() to get position in file %.",
				fileName.str);

	clearerr(file);
	if(fseek(file, 0, SEEK_END) < 0)
		throw FileError(errno, "Could not seek to end of file %.", fileName.str);

	clearerr(file);
	long size = 0;
	size = ftell(file);
	if(size < 0)
		throw FileError(errno, "Could not use ftell() to get position in  file %.",
				fileName.str);

	clearerr(file);
	if(fseek(file, lastPos, SEEK_SET) < 0)
		throw FileError(errno, "Could not seek in file %.", fileName.str);

	return size;
}

void File::rename(const char * name)
{
	if(file)
		close();

	String oldFileName(fileName);
//	char buf[PATH_MAX];
//	String cwd(char *getcwd(buf, PATH_MAX));
	fileName.assign(name);
	int fn = ::rename(oldFileName.str, name);
	if(fn < 0){
		int errNo = errno;
		fileName = move_cast(oldFileName);
		throw FileError(errNo, "Could not rename file % to %.", fileName.str, name);
	}
}

void File::rename(const String & name)
{
	rename(name.str);
}

void File::resize(long unsigned size)
{
	int errNo = 0;

	if(file)
		close();

	int fn = open(fileName.str, O_RDWR);
	if(fn < 0)
		throw FileError(errno, "Could not get file number to be used for resizing "
				 "file % to size of %.", fileName.str, size);

	if(ftruncate(fn, size) < 0)
		errNo = errno;

	close(fn);
	if(errNo)
		throw FileError(errNo, "Could not resize file % to size of %.", fileName.str, size);
}

void File::create()
{
	if(exists())
		return;

	FILE* f = fopen(fileName.str, "w+b");
	if(!f)
		throw FileError(errno, "Could not create file %.", fileName.str);
	close();
}

void File::createDir()
{
	if(exists()){
		if(!isDir())
			throw FileError(errno, "'%' is already existing and is not a directory.",
					 fileName.str);
		return;
	}

	if(mkdir(fileName.str, 0700) < 0)
		throw FileError(errno, "Failed to create directory (%).", fileName.str);
}

void File::removeDir()
{
	if(rmdir(fileName.str) < 0)
		throw FileError(errno, "Failed to remove directory (%).", fileName.str);
}

void File::unlink()
{
	if(file)
		close();
	if(::unlink(fileName.str) < 0)
		throw FileError(errno, "Could not unlink file %.", fileName.str);
}

void File::openForRead() const
{
	if(file && !writable)
		return;
	if(file)
		close();

	file = fopen(fileName.str, "rb");
	if(!file)
		throw FileError(errno, "Could not open file (%) for reading.", fileName.str);
	writable = false;
}

void File::openForWrite()
{
	if(file && writable)
		return;
	if(!file && !exists())
		create();
	if(file)
		close();

	file = fopen(fileName.str, "r+b");
	if(!file)
		throw FileError(errno, "Could not open file (%) for writing (and reading).",
				fileName.str);

	try{
		fileSize = size();
	} catch(...) {
		/* If we can not init fileSize, we should not write into the file. */
		close(false); /* Try to cloe to be proper. */
		throw;
	}

	writable = true;
}

/** In theory close() might return with the belows:
 * - EBADF  fd isn't a valid open file descriptor.
 * - EINTR  The close() call was interrupted by a signal; see signal(7).
 * - EIO    An I/O error occurred.
 * We do handle the case of EINTR by retries, but there is no possible way
 * to fix up EBADF or EIO.
 *
 * Since in some cases is critical to close successfully. When 'throws' is
 * false, we do not throw the generated exception, just doing some extra
 * loggin and pretend successfull operation by setting the descriptor to
 * NULL before returning.
 */
void File::close(bool throws) const
{
	if(!file)
		return;

	int err;
	TEMP_FAILURE_RETRY_RESULT(err, fclose(file));
	if(err){
		FileError e(errno, "Could not close file %.", fileName.str);
		if(throws)
			throw (FileError&&)e;

		e.note("Absorbing (not throwing) exception in file close.");
		EXCEPTION(e);
	}

	file = NULL;
	writable = false;
}

bool File::eof() const
{
	if(!file)
		return false;

	return feof(file);
}

void File::rewind() const
{
	if(!file)
		return;

	errno = 0;
	clearerr(file);
	::rewind(file);
	if(errno)
		 throw FileError(errno, "Could not rewind file position in file %.", fileName.str);
}

String File::readAll() const
{
	if(!file)
		openForRead();

	CArray<char> buffer(4096);
	long unsigned capacity = buffer.size - 1;

	errno = 0;
	while(buffer.len < capacity){
		clearerr(file);
		buffer.len += fread(buffer.ptr + buffer.len, 1, buffer.size - capacity, file);
		int errNo = errno;
		if(feof(file))
			break;
		if(errNo)
			throw FileError(errNo, "Error after reading % bytes from file %.",
					buffer.len, fileName.str);
		if(buffer.len == capacity){
			capacity += 4096;
			buffer.resize(capacity + 1);
		}
	}

	String str;
	str.adopt(buffer.ptr, buffer.len, buffer.size);
	return str;
}

String File::read(long unsigned bytes) const
{
	if(!file)
		openForRead();

	CArray<char> buffer(bytes + 1);

	errno = 0;
	while(buffer.len < bytes){
		clearerr(file);
		buffer.len += fread(buffer.ptr + buffer.len, 1, bytes - buffer.len, file);
		int errNo = errno;
		if(feof(file))
			break;
		if(errNo)
			throw FileError(errNo, "Error after reading % bytes from file %.",
					buffer.len, fileName.str);
	}

	if(buffer.len != bytes)
		throw FileError("Found end of file after reading %/% bytes from file %.",
				buffer.len, bytes, fileName.str);

	String str;
	str.adopt(buffer.ptr, buffer.len, buffer.size);
	return str;
}

String File::readAllFromPos(long unsigned pos) const
{
	if(!file)
		openForRead();

	clearerr(file);
	if(fseek(file, pos, SEEK_SET) < 0)
		throw FileError(errno, "Could not seek in file %.", fileName.str);

	return readAll();
}

String File::readFromPos(long unsigned pos, long unsigned bytes) const
{
	if(!file)
		openForRead();

	clearerr(file);
	if(fseek(file, pos, SEEK_SET) < 0)
		throw FileError(errno, "Could not seek in file %.", fileName.str);

	return read(bytes);
}
/*
void File::getLine(String & buffer) const
{
	if(!file)
		openForRead();

	char* buf = NULL;
	long unsigned size = 0;

	clearerr(file);
	int length = getline(&buf, &size, file);
	if(length < 0){
		if(feof(file)){
			buffer.clear();
			return;
		}
		throw FileError(errno, "Could not read line from file %s.", fileName.str);
	}

	buffer.adopt(buf, length, size);
}
*/

void File::write(const String & data)
{
	if(!file || !writable)
		openForWrite();

	long unsigned written = 0;
	while(written < data.length){
		clearerr(file);
		errno = 0;
		written += fwrite(data.str, 1, data.length, file);
		if(errno){
			int errNo = errno;
			FileError e(errNo, "Error after writting % bytes into file %.",
					written, fileName.str);
			if(written){
				try {
					resize(fileSize);
					close(file);
				} catch (std::exception &ee) {
					e.note("Failed to revert partial write.\n"
							"What: %", ee.what());
				}
			}
			throw e;
		}
	}

	fileSize += written;
}

void File::writeAtPos(const String & data, long unsigned pos)
{
	if(!file || !writable)
		openForWrite();

	clearerr(file);
	if(fseek(file, pos, SEEK_SET) < 0)
		throw FileError(errno, "Could not seek in file %.", fileName.str);

	write(data);
}

void File::append(const String & data)
{
	if(!file || !writable)
		openForWrite();

	clearerr(file);
	if(fseek(file, 0L, SEEK_END) < 0)
		throw FileError(errno, "Could not seek to end of %.", fileName.str);
	write(data);
}

void File::appendPrintf(const char * format, ...)
{
	if(format == NULL)
		return;

	String str;
	va_list args;

	va_start(args, format);
	int _length = vsnprintf(NULL, 0, format, args);
	va_end(args);
	if(_length < 0)
		throw InvalidArgument(_length, "Libc vsnprintf() failed in %.",
				__PRETTY_FUNCTION__);

	va_start(args, format);
	str.appendVPrintf(format, args, _length);
	va_end(args);

	append(str);
}

}
