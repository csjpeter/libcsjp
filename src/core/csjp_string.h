/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef CSJP_STRING_H
#define CSJP_STRING_H

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <csjp_exception.h>
#include <csjp_logger.h>

namespace csjp {

class String
{
	class iterator
	{
	public:
		iterator(char * ptr): ptr(ptr){}
		iterator operator++() { ++ptr; return *this; }
		bool operator!=(const iterator & other) { return ptr != other.ptr; }
		const char & operator*() const { return *ptr; }
		char & operator*() { return *ptr; }
	private:
		char * ptr;
	};
#define StringInitializer : \
		val(NULL), \
		len(0), \
		size(0), \
		length(len)

public:
	explicit String(const String & orig) StringInitializer {
		assign(orig.val, orig.length);
	}

	String(String && temp) :
		val(temp.val),
		len(temp.len),
		size(temp.size),
		length(len)
	{
		temp.val = 0;
		temp.len = 0;
		temp.size = 0;
	}

	const String & operator=(String && temp)
	{
		val= temp.val;
		len = temp.len;
		size = temp.size;

		temp.val = 0;
		temp.len = 0;
		temp.size = 0;

		return *this;
	}

public:
	explicit String() StringInitializer { }
	virtual ~String() { if(val) free(val); }

private:
	char * val;
	size_t len;
	size_t size; // != capacity; capacity == size - 1;
	/*
	 * Invariants:
	 * - if val != NULL and points to valid area :
	 *   - len < size
	 *   - val[len] == 0
	 * - if val == NULL :
	 *   - len == 0
	 */
public:
	const size_t & length;

	iterator begin() const { if(!val) return 0; return iterator(val); }
	iterator end() const { return iterator(val + len); }

public:
	explicit String(size_t s) StringInitializer { setCapacity(s); }
	explicit String(const char * str, size_t _length) StringInitializer {assign(str, _length);}
	explicit String(const char * str) StringInitializer { assign(str); }

	const char * c_str() const { return val; }
	operator const char * () const { return val; }

	const char& operator[](unsigned char i) const { return val[i]; }
	inline char& operator[](unsigned char i){ return val[i]; }
	const char& operator[](int i) const { return val[i]; }
	inline char& operator[](int i){ return val[i]; }
	const char& operator[](long int i) const { return val[i]; }
	inline char& operator[](long int i){ return val[i]; }
	const char& operator[](long long int i) const { return val[i]; }
	inline char& operator[](long long int i){ return val[i]; }
	const char& operator[](unsigned i) const { return val[i]; }
	inline char& operator[](unsigned i){ return val[i]; }
	const char& operator[](long unsigned i) const { return val[i]; }
	inline char& operator[](long unsigned i){ return val[i]; }
	const char& operator[](long long unsigned i) const { return val[i]; }
	inline char& operator[](long long unsigned i){ return val[i]; }

	int compare(const char * str, size_t _length) const;
	int compare(const char * str) const;
	int compare(const String & str) const;

	bool isEqual(const char * str, size_t _length) const;
	bool isEqual(const char * str) const;
	bool isEqual(const String & str) const;

	bool findFirst(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirst(size_t &, const char *, size_t from, size_t until) const;
	bool findFirst(size_t &, const char *, size_t from = 0) const;
	bool findFirst(size_t &, const String &, size_t from, size_t until) const;
	bool findFirst(size_t &, const String &, size_t from = 0) const;
	bool findFirst(size_t &, char, size_t from = 0) const;

	bool findFirstOf(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const char *, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const char *, size_t from = 0) const;
	bool findFirstOf(size_t &, const String &, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const String &, size_t from = 0) const;

	bool findFirstNotOf(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const char *, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const char *, size_t from = 0) const;
	bool findFirstNotOf(size_t &, const String &, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const String &, size_t from = 0) const;

	bool findLast(size_t &, const char *, size_t _length, size_t until) const;
	bool findLast(size_t &, const char *, size_t until) const;
	bool findLast(size_t &, const char *) const;
	bool findLast(size_t &, const String &, size_t until) const;
	bool findLast(size_t &, const String &) const;
	bool findLast(size_t &, char, size_t until) const;
	bool findLast(size_t &, char) const;

	bool findLastOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastOf(size_t &, const char *, size_t until) const;
	bool findLastOf(size_t &, const char *) const;
	bool findLastOf(size_t &, const String &, size_t until) const;
	bool findLastOf(size_t &, const String &) const;

	bool findLastNotOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastNotOf(size_t &, const char *, size_t until) const;
	bool findLastNotOf(size_t &, const char *) const;
	bool findLastNotOf(size_t &, const String &, size_t until) const;
	bool findLastNotOf(size_t &, const String &) const;

	bool startsWith(const char *, size_t length) const;
	bool startsWith(const char *) const;
	bool endsWith(const char *, size_t length) const;
	bool endsWith(const char *) const;

	size_t count(const char *, size_t _length, size_t from, size_t until) const;
	size_t count(const char *, size_t from, size_t until) const;
	size_t count(const char *, size_t from = 0) const;
	size_t count(const String & str, size_t from, size_t until) const;
	size_t count(const String & str, size_t from = 0) const;

	const String & operator=(const char * str) { assign(str); return *this; }
	const String & operator=(char c) { chop(); append(c); return *this; }
	const String & operator=(unsigned char c) { chop(); append(c); return *this; }
	const String & operator=(unsigned u) { chop(); append(u); return *this; }
	const String & operator=(long unsigned u) {chop(); append(u); return *this; }
	const String & operator=(long long unsigned u) {chop(); append(u); return *this; }
	const String & operator=(int i) { chop(); append(i); return *this; }
	const String & operator=(long int i) { chop(); append(i); return *this; }
	const String & operator=(long long int i) { chop(); append(i); return *this; }
	const String & operator=(const String & s) { assign(s); return s; }
	const String & operator+=(const char * str){ append(str); return *this; }
	const String & operator+=(const String &);

	void assign(const char *, size_t _length);
	void assign(const char *);
	void assign(const String &);

	void setCapacity(size_t);

	void extendCapacity(size_t); // reserves extra space over the given size for future usage

	size_t capacity() const;

	void fill(char, size_t);

	void shiftForward(size_t from, size_t until, size_t offset);
	void shiftBackward(size_t from, size_t until, size_t offset);

	void prepend(char);
	void prepend(const char *, size_t _length);
	void prepend(const char *);
	void prepend(const String &);

	void append(const char *, size_t _length);
	void append(const char *);
	void append(const void * ptr) { appendPrintf("%p", ptr); }
	void append(const String &);
	void append(bool b) { append( b ? "true" : "false"); }
	void append(char);
	void append(unsigned char);
	void append(unsigned);
	void append(long unsigned);
	void append(long long unsigned);
	void append(int);
	void append(long int);
	void append(long long int);
	void append(double);
	void append(long double, unsigned precision = 6);

	void appendVPrintf(const char * format, va_list args, size_t length = 256);
	void appendPrintf(const char * format, ...)
			/* We need to use 2, 3 because 'this' pointer is the first parameter. */
			__attribute__ ((format (printf, 2, 3)));

	// http://goo.gl/19TOHg
	template<typename Arg> void cat(const Arg & arg) { *this << arg; }
	template<typename Arg, typename... Args>
	void cat(const Arg & arg, const Args & ... args) { *this << arg; cat(args...); }

	void catf(const char * fmt) { append(fmt); }
	template<typename Arg> void catf(const char * fmt, const Arg & arg)
	{
		while(*fmt != 0){
			if(*fmt == '%'){
				fmt++;
				if(*fmt != '%'){
					*this << arg;
					append(fmt);
					return;
				}
			}
			append(*fmt);
			fmt++;
		}
		*this << arg;
	}
	template<typename Arg, typename... Args>
	void catf(const char * fmt, const Arg & arg, const Args & ... args)
	{
		while(*fmt != 0){
			if(*fmt == '%'){
				fmt++;
				if(*fmt != '%'){
					*this << arg;
					catf(fmt, args...);
					return;
				}
			}
			append(*fmt);
			fmt++;
		}
		cat(args...);
	}

	void insert(size_t pos, const char *, size_t _length);
	void insert(size_t pos, const char *);
	void insert(size_t pos, const String &);

	void erase(size_t from, size_t until);

	void write(size_t pos, const char *, size_t _length);
	void write(size_t pos, const char *);
	void write(size_t pos, const String &);

	String read(size_t from, size_t until) const;

	void replace(const char * what, size_t whatLength, const char * to, size_t toLength, size_t from, size_t until, size_t maxNumOfRepl);
	void replace(const char * what, size_t whatLength, const char * to, size_t toLength, size_t from, size_t until);
	void replace(const char * what, size_t whatLength, const char * to, size_t toLength, size_t from = 0);
	void replace(const char * what, const char * to, size_t from, size_t until, size_t maxNumOfRepl);
	void replace(const char * what, const char * to, size_t from, size_t until);
	void replace(const char * what, const char * to, size_t from = 0);
	void replace(const String & what, const String & to, size_t from, size_t until, size_t maxNumOfRepl);
	void replace(const String & what, const String & to, size_t from, size_t until);
	void replace(const String & what, const String & to, size_t from = 0);

	void clear();

	void chopFront(size_t _length);
	void chopBack(size_t _length);
	void cutAt(size_t);
	void chop();

	void trim(const char * toTrim, size_t _length);
	void trim(const char * toTrim);
	void trim(const String &);
	void trimFront(const char * toTrim, size_t _length);
	void trimFront(const char * toTrim);
	void trimFront(const String &);
	void trimBack(const char * toTrim, size_t _length);
	void trimBack(const char * toTrim);
	void trimBack(const String &);

	void adopt(char *& str, size_t _length, size_t size);
	void adopt(char *& str, size_t size);
	void adopt(char *& str);
};

inline bool operator==(const String & a, const char * b) { return a.isEqual(b); }
inline bool operator!=(const String & a, const char * b) { return !a.isEqual(b); }
inline bool operator==(const String & a, const String & b) { return a.isEqual(b); }
inline bool operator!=(const String & a, const String & b) { return !a.isEqual(b); }
inline bool operator<(const String & a, const String & b) { return a.compare(b) == -1; }
inline bool operator<(const String & a, const char * b) { return a.compare(b) == -1; }
inline bool operator<(const char * a, const String & b) { return b.compare(a) == 1; }

inline String & operator<<(String & lhs, const std::exception & e){lhs.append(e.what());return lhs;}

inline String & operator<<(String & lhs, char rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, unsigned char rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const char * rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const void * rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const String & rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const unsigned rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const long unsigned rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const long long unsigned rhs){lhs.append(rhs); return lhs;}
inline String & operator<<(String & lhs, const int rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const long int rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const long long int rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const float rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const double rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const long double rhs) { lhs.append(rhs); return lhs; }
inline String & operator<<(String & lhs, const UInt & rhs) { lhs << rhs.val; return lhs; }
inline String & operator<<(String & lhs, const Double & rhs) { lhs << rhs.val; return lhs; }

inline String & operator<<=(String & lhs, char rhs) { lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const unsigned rhs){ lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const long unsigned rhs){lhs.chop();lhs<<rhs; return lhs;}
inline String & operator<<=(String & lhs, const long long unsigned rhs)
						{ lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const int rhs) { lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const long int rhs) {lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const long long int rhs){lhs.chop();lhs<<rhs; return lhs;}
inline String & operator<<=(String & lhs, const float rhs) { lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const double rhs) { lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const long double rhs){lhs.chop(); lhs<<rhs; return lhs;}
inline String & operator<<=(String & lhs, const YNBool & rhs)
					{ if(rhs.val) lhs <<= 'Y'; else lhs <<= 'N'; return lhs; }
inline String & operator<<=(String & lhs, const UInt & rhs) { lhs.chop(); lhs << rhs; return lhs; }
inline String & operator<<=(String & lhs, const Double & rhs){ lhs.chop(); lhs << rhs; return lhs; }



inline int & operator<<=(int & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline long int & operator<<=(long int & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline long long int & operator<<=(long long int & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline unsigned & operator<<=(unsigned & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline long unsigned & operator<<=(long unsigned & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline long long unsigned & operator<<=(long long unsigned & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline float & operator<<=(float & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline double & operator<<=(double & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }
inline long double & operator<<=(long double & i, const String & str)
		{ if(!str) i = 0; else i <<= CString(str); return i; }




class ErrNo : public String
{
public:
	ErrNo()
	{
		int errNo = errno;
		char errorMsg[512] = {0};
		const char * msg = NULL;
		errno = 0;
#ifdef _GNU_SOURCE
		msg = strerror_r(errNo, errorMsg, sizeof(errorMsg));
#else
		int r = strerror_r(errNo, errorMsg, sizeof(errorMsg));
		(void)r;
		msg = errorMsg;
#endif
		int _errNo = errno;
		if(_errNo)
			catf("Error in strerror_r: number % - %", _errNo, strerror(_errNo));
		catf("% - %", errNo, msg);
	}
};





#ifdef DEBUG
#define DBG(...){ \
			csjp::String str; \
			str.cat(VT_MAGENTA VT_TA_BOLD "DEBUG    " VT_NORMAL \
					VT_BLUE, __PRETTY_FUNCTION__, ":", \
					__LINE__, VT_NORMAL); \
			str.catf(__VA_ARGS__); \
			csjp::msgLogger(csjp::verboseMode ? stderr : NULL, str, str.length); \
		}
#else
#define DBG(...) {;}
#endif

#define LOG(...){ \
			csjp::String str; \
			str.append(VT_CYAN VT_TA_BOLD "LOG      " VT_NORMAL); \
			str.catf(__VA_ARGS__); \
			csjp::msgLogger(csjp::verboseMode ? stderr : NULL, str, str.length); \
		}

#define FATAL(...){ \
			csjp::String str; \
			str.append(VT_RED VT_TA_UNDERSCORE VT_TA_BOLD \
					"FATAL    " VT_NORMAL); \
			str.catf(__VA_ARGS__); \
			fflush(stdout); \
			csjp::msgLogger(stderr, str, str.length); \
			fflush(stderr); \
			exit(-1); \
		}

#define EXCEPTION(exc)	{ \
				csjp::String str; \
				str.catf("% %, details are below:\n", \
						VT_RED VT_TA_BOLD "EXCEPTION" \
						VT_NORMAL, exc.name); \
				for(const auto & iter : exc) \
					str.cat(iter, '\n'); \
				fflush(stdout); \
				csjp::msgLogger(stderr, str, str.length); \
				fflush(stderr); \
			}

#define DECL_EXCEPTION(parent, exc) \
	class exc : public parent \
	{ \
	public: \
		exc() : parent() \
		{ \
			name = #exc; \
		} \
		void note(const char * fmt) \
		{\
			PrimeException::note(fmt); \
		}\
		template<typename... Args> \
		void note(const char * fmt, const Args & ... args) \
		{\
			csjp::String buf; \
			buf.catf(fmt, args...); \
			note(buf); \
		}\
		exc(const char * msg){\
			name = #exc; \
			note(msg); \
		}\
		template<typename... Args> \
		exc(const char * fmt, const Args & ... args) \
		{ \
			name = #exc; \
			note(fmt, args...); \
		} \
		exc(const std::exception & e) : parent(e) \
		{ \
			name = #exc; \
		} \
		exc(std::exception && e) : parent(e) \
		{ \
			name = #exc; \
		} \
		exc(const std::exception & e, const char * msg) : parent(e) \
		{ \
			name = #exc; \
			note(msg); \
		}\
		template<typename... Args> \
		exc(const std::exception & e, const char * fmt, const Args & ... args) : parent(e) \
		{ \
			name = #exc; \
			note(fmt, args...); \
		} \
		exc(int err) : parent(err) \
		{ \
			name = #exc; \
		} \
		exc(int err, const char * msg) : parent(err) \
		{\
			name = #exc; \
			note(msg); \
		}\
		template<typename... Args> \
		exc(int err, const char * fmt, const Args & ... args) : parent(err) \
		{ \
			name = #exc; \
			note(fmt, args...); \
		} \
		exc(exc && temp) : parent((parent &&)temp) \
		{ \
		} \
		const exc & operator=(exc && temp) \
		{ \
			parent::operator=((parent &&)temp); \
			return *this; \
		} \
		virtual ~exc() throw() {} \
	}

DECL_EXCEPTION(PrimeException, Exception);

DECL_EXCEPTION(Exception, ResourceError);
DECL_EXCEPTION(Exception, LogicError);
DECL_EXCEPTION(Exception, NotImplemented);
DECL_EXCEPTION(Exception, SystemError);

DECL_EXCEPTION(ResourceError, OutOfMemory);
DECL_EXCEPTION(ResourceError, BufferFull);
DECL_EXCEPTION(ResourceError, FileError);
DECL_EXCEPTION(ResourceError, ParseError);
DECL_EXCEPTION(ResourceError, ConversionError);

DECL_EXCEPTION(LogicError, ShouldNeverReached);
DECL_EXCEPTION(LogicError, InvalidState);
DECL_EXCEPTION(LogicError, InvalidArgument);
DECL_EXCEPTION(LogicError, InvalidAssignment);
DECL_EXCEPTION(LogicError, IndexOutOfRange);
DECL_EXCEPTION(LogicError, ObjectNotFound);
DECL_EXCEPTION(LogicError, InvariantFailure);
DECL_EXCEPTION(LogicError, TestFailure);

}


#endif
