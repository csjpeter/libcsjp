/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include <stdio.h> // FIXME replace with system call

#include <string.h>

#include <csjp_string.h>
#include <csjp_carray.h>

#include "csjp_file.h"

namespace csjp {

TempFile::TempFile(const char * templateFileName) :
	File(templateFileName)
{
	file = mkstemp((char *)fileName.c_str());
	if(file == -1)
		throw FileError(errno, "Could not create temporary file %.", fileName);
	writable = true;
}

TempFile::TempFile(const Str & templateFileName) :
	File(templateFileName)
{
	file = mkstemp((char *)fileName.c_str());
	if(file == -1)
		throw FileError(errno, "Could not create temporary file %.", fileName);
	writable = true;
}

TempFile::~TempFile()
{
	if(0 <= file)
		close(false);
}

File::File(const Str & fileName) :
	file(-1),
	writable(false),
	eofbit(false),
	locked(false),
	fileSize(0),
	fileName(fileName)
{
}

File::~File()
{
	if(0 <= file)
		close(false);
}
#if 0
/**
 * Based on stackoverflow answer:
 * http://stackoverflow.com/questions/575328/fcntl-lockf-which-is-better-to-use-for-file-locking?answertab=active#tab-top
 * It says:
 *	Locking in unix/linux is by default advisory.... 
 * Also says:
 *	Linux does support mandatory locking, but only if your file system is
 *	mounted with the option on and the file special attributes set.
 *
 * length is the number of bytes to lock from the current position; 0 means infinity
 */
void File::lock(off_t length)
{
	if(file < 0 || !writable)
		openForWrite();
	if(lockf(file, F_TLOCK, length) < 0)
		throw FileError(errno, "Failed to lock file %.", fileName);
	locked = true;

/*	// alternate implementation
	(void)length;
	if(0 < file)
		close();
	file = open(fileName, O_RDWR|O_CREAT|O_EXCL, 0600);
	if(file < 0)
		throw FileError(errno, "Failed to open file % in exclusive open mode.", fileName);
	writable = true;
	eofbit = false;
	locked = true;
*/
}

void File::unlock()
{
	bool lockState = locked;
	locked = false;
	if(0 <= file){
		try{
			// We must use close and not lockf(,F_ULOCK,) because in case of a daemon
			// and its children the unlock would release the file in the name of all the
			// processes. The close however keeps the lock for the still running
			// processes and thus, the file will be unlock only when all the children
			// and the daemon process itself closed the file.
			close();
		} catch(Exception & e) {
			locked = lockState; // to be transactional
			throw;
		}
	}
}
#endif
const String& File::name() const
{
	return fileName;
}

bool File::exists() const
{
	struct stat fileStat;
	int err = 0;
	if(stat(fileName.c_str(), &fileStat) < 0){
		err = errno;
		if(err == ENOENT || err == ENOTDIR)
			return false;
		throw FileError(errno, "Could not check for the existence of file (%).",
				fileName);
	}
	return true;
}

bool File::isRegular() const
{
	struct stat fileStat;
	if(stat(fileName.c_str(), &fileStat) < 0)
		throw FileError(errno, "Could not check if file (%) is regular file.",
				fileName);

	if(S_ISREG(fileStat.st_mode))
		return true;
	return false;
}

bool File::isDir() const
{
	struct stat fileStat;
	if(stat(fileName.c_str(), &fileStat) < 0)
		throw FileError(errno, "Could not check if (%) is directory.", fileName);

	if(S_ISDIR(fileStat.st_mode))
		return true;
	return false;
}

long unsigned File::size() const
{
	if(file < 0)
		openForRead();

	long lastPos = lseek(file, 0, SEEK_CUR);
	if(lastPos < 0)
		throw FileError(errno, "Could not use lseek to get position in file %.", fileName);

	off_t size = lseek(file, 0, SEEK_END);
	if(size < 0)
		throw FileError(errno, "Could not seek to end of file %.", fileName);

	if(-1 == lseek(file, lastPos, SEEK_SET))
		throw FileError(errno, "Could not seek in file %.", fileName);

	return size;
}

void File::rename(const Str & name)
{
	if(0 <= file)
		close();

	String oldFileName(fileName);
//	char buf[PATH_MAX];
//	String cwd(char *getcwd(buf, PATH_MAX));
	fileName.assign(name);
	int fn = ::rename(oldFileName.c_str(), fileName.c_str());
	if(fn < 0){
		int errNo = errno;
		fileName = move_cast(oldFileName);
		throw FileError(errNo, "Could not rename file % to %.", fileName, name);
	}
}

void File::resize(long unsigned size)
{
	int errNo = 0;

	if(0 <= file)
		close();

	int fn = open(fileName.c_str(), O_RDWR);
	if(fn < 0)
		throw FileError(errno, "Could not get file number to be used for resizing "
				 "file % to size of %.", fileName, size);

	if(ftruncate(fn, size) < 0)
		errNo = errno;

	::close(fn);
	if(errNo)
		throw FileError(errNo, "Could not resize file % to size of %.", fileName, size);
}

void File::create()
{
	if(0 <= file)
		close();
	if(exists())
		return;

	file = open(fileName.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0600);
	if(file < 0)
		throw FileError(errno, "Could not create file %.", fileName);
	close();
}

void File::createDir()
{
	if(exists()){
		if(!isDir())
			throw FileError(errno, "'%' is already existing and is not a directory.",
					 fileName);
		return;
	}

	if(mkdir(fileName.c_str(), 0700) < 0)
		throw FileError(errno, "Failed to create directory (%).", fileName);
}

void File::removeDir()
{
	if(rmdir(fileName.c_str()) < 0)
		throw FileError(errno, "Failed to remove directory (%).", fileName);
}

void File::unlink()
{
	if(0 <= file)
		close();
	if(::unlink(fileName.c_str()) < 0)
		throw FileError(errno, "Could not unlink file %.", fileName);
}

void File::openForRead() const
{
	if(0 <= file && !writable)
		return;
	if(0 <= file)
		close();

	file = open(fileName.c_str(), O_RDONLY, 0600);
	if(file < 0)
		throw FileError(errno, "Could not open file (%) for reading.", fileName);
	writable = false;
	eofbit = false;
}

void File::openForWrite()
{
	if(0 <= file && writable)
		return;
	if(file < 0 && !exists())
		create();
	if(0 <= file)
		close();

	file = open(fileName.c_str(), O_RDWR, 0600);
	if(file < 0)
		throw FileError(errno, "Could not open file (%) for writing (and reading).",
				fileName);

	try{
		fileSize = size();
	} catch(...) {
		/* If we can not init fileSize, we should not write into the file. */
		close(false); /* Try to close to be proper. */
		throw;
	}

	writable = true;
	eofbit = false;
}

/** In theory close() might return with the belows:
 * - EBADF  fd isn't a valid open file descriptor.
 * - EINTR  The close() call was interrupted by a signal; see signal(7).
 * - EIO    An I/O error occurred.
 * We do handle the case of EINTR by retries, but there is no possible way
 * to fix up EBADF or EIO.
 *
 * Since in some cases critical to close successfully. When 'throws' is
 * false, we do not throw the generated exception, just doing some extra
 * loggin and pretend successfull operation by setting the descriptor to
 * NULL before returning.
 */
void File::close(bool throws) const
{
	if(file < 0)
		return;

	if(locked){
		FileError e("Can not close a the locked file %, it has to be unlocked.", fileName); 
		if(throws)
			throw (FileError&&)e;
		else{
			e.note("Absorbing (not throwing) exception in file close.");
			EXCEPTION(e);
		}
	}

	int err;
	TEMP_FAILURE_RETRY_RESULT(err, ::close(file));
	if(err){
		FileError e(errno, "Could not close file %.", fileName);
		if(throws)
			throw (FileError&&)e;

		e.note("Absorbing (not throwing) exception in file close.");
		EXCEPTION(e);
	}

	file = -1;
	writable = false;
}

bool File::eof() const
{
	return eofbit;
}

void File::rewind() const
{
	if(file < 0)
		return;

	if(-1 == lseek(file, 0, SEEK_SET))
		throw FileError(errno, "Could not rewind file position in file %.", fileName);
	eofbit = false;
}

String File::readAll() const
{
	if(file < 0)
		openForRead();

	CArray<char> buffer(4096);
	long unsigned capacity = buffer.size - 1;

	errno = 0;
	while(buffer.len < capacity){
		ssize_t readIn = ::read(file, buffer.ptr + buffer.len, capacity - buffer.len);
		buffer.len += readIn;
		buffer.ptr[buffer.len] = 0;
		if(errno)
			throw FileError(errno, "Error after reading % bytes from file %.",
					buffer.len, fileName);
		if(!readIn){
			eofbit = true;
			break;
		}
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
	if(file < 0)
		openForRead();

	CArray<char> buffer(bytes + 1);

	errno = 0;
	while(buffer.len < bytes){
		ssize_t readIn = ::read(file, buffer.ptr + buffer.len, bytes - buffer.len);
		buffer.len += readIn;
		if(errno)
			throw FileError(errno, "Error after reading % bytes from file %.",
					buffer.len, fileName);
		if(!readIn){
			eofbit = true;
			break;
		}
	}

	if(buffer.len != bytes)
		throw FileError("Found end of file after reading %/% bytes from file %.",
				buffer.len, bytes, fileName);

	String str;
	str.adopt(buffer.ptr, buffer.len, buffer.size);
	return str;
}

String File::readAllFromPos(long unsigned pos) const
{
	if(file < 0)
		openForRead();

	if(lseek(file, pos, SEEK_SET) < 0)
		throw FileError(errno, "Could not seek in file %.", fileName);

	return readAll();
}

String File::readFromPos(long unsigned pos, long unsigned bytes) const
{
	if(file < 0)
		openForRead();

	if(lseek(file, pos, SEEK_SET) < 0)
		throw FileError(errno, "Could not seek in file %.", fileName);

	return read(bytes);
}
/*
void File::getLine(String & buffer) const
{
	if(file < 0)
		openForRead();

	char* buf = NULL;
	long unsigned size = 0;

	int length = getline(&buf, &size, file);
	if(length < 0){
		if(feof(file)){
			buffer.clear();
			return;
		}
		throw FileError(errno, "Could not read line from file %s.", fileName);
	}

	buffer.adopt(buf, length, size);
}
*/

void File::write(const Str & data)
{
	if(file < 0 || !writable)
		openForWrite();

	long unsigned written = 0;
	while(written < data.length){
		errno = 0;
		int justWritten = ::write(file, data.c_str() + written, data.length - written);
		if(errno){
			int errNo = errno;
			FileError e(errNo, "Error after writting % bytes into file %.",
					written, fileName);
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
		written += justWritten;
	}

	fileSize += written; // FIXME what if write is not appending, but overwriting
}

void File::writeAtPos(const Str & data, long unsigned pos)
{
	if(file < 0 || !writable)
		openForWrite();

	if(lseek(file, pos, SEEK_SET) < 0)
		throw FileError(errno, "Could not seek in file %.", fileName);

	write(data);
}

void File::append(const Str & data)
{
	if(file < 0 || !writable)
		openForWrite();

	if(lseek(file, 0L, SEEK_END) < 0)
		throw FileError(errno, "Could not seek to end of %.", fileName);
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
