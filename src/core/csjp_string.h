/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#ifndef CSJP_STRING_H
#define CSJP_STRING_H

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#include <csjp_str.h>

#include <csjp_exception.h>

namespace csjp {

class String : public AStr
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
private:
	size_t size; // != capacity; capacity == size - 1;
	/*
	 * Invariants:
	 * - if data != NULL and points to valid area :
	 *   - len < size
	 *   - data[len] == 0
	 * - if data == NULL :
	 *   - len == 0
	 */

#define StringInitializer : \
	        AStr(), \
		size(0)

public:

	explicit String() StringInitializer { }
	explicit String(const String & orig) StringInitializer { assign(orig.data, orig.length);}
	explicit String(const Str & str) StringInitializer { assign(str); }
	explicit String(const AStr & astr) StringInitializer { assign(astr); }
	//template<typename... Args>
	//explicit String(const char * fmt, const Args & ... args) { catf(fmt, args...); }
	virtual ~String() { if(data) free(data); }

	String(String && temp);
	const String & operator=(String && temp);

	iterator begin() const { if(!data) return 0; return iterator(data); }
	iterator end() const { return iterator(data + len); }

public:
	void setCapacity(size_t);
	void extendCapacity(size_t); // reserves extra space for later usage
	size_t capacity() const;
	void setLength(size_t length);
/*
	const char& operator[](unsigned char i) const { return data[i]; }
	inline char& operator[](unsigned char i){ return data[i]; }
	const char& operator[](int i) const { return data[i]; }
	inline char& operator[](int i){ return data[i]; }
	const char& operator[](long int i) const { return data[i]; }
	inline char& operator[](long int i){ return data[i]; }
	const char& operator[](long long int i) const { return data[i]; }
	inline char& operator[](long long int i){ return data[i]; }
	const char& operator[](unsigned i) const { return data[i]; }
	inline char& operator[](unsigned i){ return data[i]; }
	const char& operator[](long unsigned i) const { return data[i]; }
	inline char& operator[](long unsigned i){ return data[i]; }
	const char& operator[](long long unsigned i) const { return data[i]; }
	inline char& operator[](long long unsigned i){ return data[i]; }
*/
	int compare(const char * str, size_t _length) const { return AStr::compare(str, _length); }
	int compare(const Str & str) const { return AStr::compare(str.data, str.len); }

	bool isEqual(const char * str, size_t _length) const { return AStr::isEqual(str, _length); }
	bool isEqual(const Str & str) const { return isEqual(str.data, str.len); }

	bool findFirst(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirst(size_t &, const Str &, size_t from, size_t until) const;
	bool findFirst(size_t &, const Str &, size_t from = 0) const;
	bool findFirst(size_t &, char, size_t from = 0) const;

	bool findFirstOf(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const Str &, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const Str &, size_t from = 0) const;

	bool findFirstNotOf(size_t &, const char *, size_t _length, size_t from, size_t until)const;
	bool findFirstNotOf(size_t &, const Str &, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const Str &, size_t from = 0) const;

	bool findLast(size_t &, const char *, size_t _length, size_t until) const;
	bool findLast(size_t &, const Str &, size_t until) const;
	bool findLast(size_t &, const Str &) const;
	bool findLast(size_t &, char, size_t until) const;
	bool findLast(size_t &, char) const;

	bool findLastOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastOf(size_t &, const Str &, size_t until) const;
	bool findLastOf(size_t &, const Str &) const;

	bool findLastNotOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastNotOf(size_t &, const Str &, size_t until) const;
	bool findLastNotOf(size_t &, const Str &) const;

	bool contains(const Str & str) const { size_t p; return findFirst(p, str); }

	bool startsWith(const char *, size_t length) const;
	bool startsWith(const char *) const;
	bool endsWith(const char *, size_t length) const;
	bool endsWith(const char *) const;

	size_t count(const char *, size_t _length, size_t from, size_t until) const;
	size_t count(const Str & str, size_t from, size_t until) const;
	size_t count(const Str & str, size_t from = 0) const;

	const String & operator=(char c) { chop(); append(c); return *this; }
	const String & operator=(unsigned char c) { chop(); append(c); return *this; }
	const String & operator=(unsigned u) { chop(); append(u); return *this; }
	const String & operator=(long unsigned u) {chop(); append(u); return *this; }
	const String & operator=(long long unsigned u) {chop(); append(u); return *this; }
	const String & operator=(int i) { chop(); append(i); return *this; }
	const String & operator=(long int i) { chop(); append(i); return *this; }
	const String & operator=(long long int i) { chop(); append(i); return *this; }
	const String & operator=(const String & s) { assign(s); return *this; }
	const String & operator=(const Str & s) { assign(s); return *this; }
	const String & operator+=(const char * str){ append(str); return *this; }
	const String & operator+=(const Str & str) { append(str.data, str.len); return *this; }

	void assign(const char *, size_t _length);
	void assign(const Str &);
	void assign(const AStr &);

	void fill(char, size_t);
	void fill(char);

	void shiftForward(size_t from, size_t until, size_t offset);
	void shiftBackward(size_t from, size_t until, size_t offset);

	void prepend(char);
	void prepend(const char *, size_t _length);
	void prepend(const Str & str) { prepend(str.data, str.len); }

	void append(const char *, size_t _length);
	void append(const char *); // FIXME remove, but causes infinite loop
	void append(const void * ptr) { appendPrintf("%p", ptr); }
	void append(const AStr & astr) { append(astr.c_str(), astr.length); }
	void append(const Str & str) { append(str.c_str(), str.length); }
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
	template<typename Arg>
	void cat(const Arg & arg) { *this << arg; }
	template<typename Arg, typename... Args>
	void cat(const Arg & arg, const Args & ... args) { *this << arg; cat(args...); }

	void catf(const char * fmt) { append(fmt); }
	template<typename Arg>
	void catf(const char * fmt, const Arg & arg);
	template<typename Arg, typename... Args>
	void catf(const char * fmt, const Arg & arg, const Args & ... args);

	void insert(size_t pos, const char *, size_t _length);
	void insert(size_t pos, const Str & str) { insert(pos, str.data, str.len); }

	void erase(size_t from, size_t until);

	void write(size_t pos, const char *, size_t _length);
	void write(size_t pos, const Str & str) { write(pos, str.data, str.len); }

	String read(size_t from, size_t until) const;

	void replace(const char * what, size_t whatLength, const char * to, size_t toLength,
						size_t from, size_t until, size_t maxNumOfRepl);
	void replace(const char * what, size_t whatLength, const char * to, size_t toLength,
						size_t from, size_t until);
	void replace(const char * what, size_t whatLength, const char * to, size_t toLength,
						size_t from = 0);
	void replace(const char * what, const char * to,
						size_t from, size_t until, size_t maxNumOfRepl);
	void replace(const char * what, const char * to, size_t from, size_t until);
	void replace(const char * what, const char * to, size_t from = 0);
	void replace(const Str & what, const Str & to,
						size_t from, size_t until, size_t maxNumOfRepl);
	void replace(const Str & what, const Str & to, size_t from, size_t until);
	void replace(const Str & what, const Str & to, size_t from = 0);

	void clear();

	void chopFront(size_t _length);
	void chopBack(size_t _length);
	void cutAt(size_t);
	void chop();

	void trim(const char * toTrim, size_t _length);
	void trim(const Str & str) { trim(str.data, str.len); }
	void trimFront(const char * toTrim, size_t _length);
	void trimFront(const Str & str) { trimFront(str.data, str.len); }
	void trimBack(const char * toTrim, size_t _length);
	void trimBack(const Str & str) { trimBack(str.data, str.len); }

	Array<Str> split(const char * delimiters, bool avoidEmptyResults = true) const;
	bool isRegexpMatch(const char * regexp);
	Array<Str> regexpMatches(const char * regexp, unsigned numOfExpectedMatches = 100);

	void adopt(char *& str, size_t _length, size_t size);
	void adopt(char *& str, size_t size);
	void adopt(char *& str);

	void lower();
	void upper();

	String toLower() const;
	String toUpper() const;

	String encodeBase64() const { return AStr::encodeBase64(); }
	String decodeBase64() const { return AStr::decodeBase64(); }
};

inline bool operator==(const String & a, const char * b) { return a.isEqual(b); }
inline bool operator!=(const String & a, const char * b) { return !a.isEqual(b); }
inline bool operator==(const String & a, const String & b) { return a.isEqual(b); }
inline bool operator!=(const String & a, const String & b) { return !a.isEqual(b); }
inline bool operator<(const String & a, const String & b) { return a.compare(b) == -1; }
inline bool operator<(const String & a, const char * b) { return a.compare(b) == -1; }
inline bool operator<(const char * a, const String & b) { return b.compare(a) == 1; }

/* concatenation operators {{{ */

inline String & operator<<(String & lhs, const std::exception & e) { lhs += e.what(); return lhs; }

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
inline String & operator<<(String & lhs, const Str & rhs)
					{ lhs.append(rhs.c_str(), rhs.length); return lhs; }

//inline String & operator<<=(String & lhs, const char * rhs) { lhs.chop(); lhs << rhs; return lhs; }
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
inline String & operator<<=(String & lhs, const Str & rhs)
					{ lhs.assign(rhs.c_str(), rhs.length); return lhs; }



int &			operator<<=(int & i,			const String & str);
long int &		operator<<=(long int & i,		const String & str);
long long int &		operator<<=(long long int & i,		const String & str);
unsigned &		operator<<=(unsigned & i,		const String & str);
long unsigned &		operator<<=(long unsigned & i,		const String & str);
long long unsigned &	operator<<=(long long unsigned & i,	const String & str);
float &			operator<<=(float & i,			const String & str);
double &		operator<<=(double & i,			const String & str);
long double &		operator<<=(long double & i,		const String & str);

/*}}}*/

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


/* Logging macros {{{*/

#ifdef DEBUG
#define DBG(...){ \
			csjp::String _csjpStr; \
			_csjpStr.cat(VT_MAGENTA VT_TA_BOLD "DEBUG    " \
					VT_NORMAL VT_BLUE, \
					__PRETTY_FUNCTION__, ":", \
					__LINE__, VT_NORMAL); \
			_csjpStr.catf(__VA_ARGS__); \
			csjp::msgLogger(csjp::verboseMode ? stderr : NULL, \
					_csjpStr.c_str(), _csjpStr.length); \
		}
#else
#define DBG(...) {;}
#endif

#define LOG(...){ \
			csjp::String _csjpStr; \
			_csjpStr.append(VT_CYAN VT_TA_BOLD "LOG      " \
					VT_NORMAL); \
			_csjpStr.catf(__VA_ARGS__); \
			csjp::msgLogger(csjp::verboseMode ? stderr : NULL, \
					_csjpStr.c_str(), _csjpStr.length); \
		}

#define FATAL(...){ \
			csjp::String _csjpStr; \
			_csjpStr.append(VT_RED VT_TA_UNDERSCORE VT_TA_BOLD \
					"FATAL    " VT_NORMAL); \
			_csjpStr.catf(__VA_ARGS__); \
			fflush(stdout); \
			csjp::msgLogger(stderr, _csjpStr.c_str(), \
					_csjpStr.length); \
			fflush(stderr); \
			exit(-1); \
		}

#define EXCEPTION(exc)	{ \
				csjp::String _csjpStr; \
				_csjpStr.catf("% %, details are below:\n", \
						VT_RED VT_TA_BOLD \
						"EXCEPTION" \
						VT_NORMAL, exc.name); \
				for(const auto & iter : exc) \
					_csjpStr.cat(iter, '\n'); \
				fflush(stdout); \
				csjp::msgLogger(stderr, _csjpStr.c_str(), \
						_csjpStr.length); \
				fflush(stderr); \
			}

/*}}}*/

/* Exception class template (macro) {{{ */
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
			note(buf.c_str()); \
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
		exc(const std::exception & e, const char * fmt, \
				const Args & ... args) : parent(e) \
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
		exc(int err, const char * fmt, const Args & ... args) : \
			parent(err) \
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
/*}}}*/

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


inline String &	operator<<(String & lhs, const PrimeException & rhs)
		{ lhs += *(rhs.begin()); return lhs; }

}

#include <csjp_array.h>

namespace csjp {

inline String Str::encodeBase64() const { return AStr::encodeBase64(); }
inline String Str::decodeBase64() const { return AStr::decodeBase64(); }

/* Inline implementations {{{*/
inline String::String(String && temp) : size(temp.size)
{
	data = temp.data;
	len = temp.len;
	temp.data = 0;
	temp.len = 0;
	temp.size = 0;
}
inline const String & String::operator=(String && temp)
{
	free(data);

	data = temp.data;
	len = temp.len;
	size = temp.size;

	temp.data = 0;
	temp.len = 0;
	temp.size = 0;

	return *this;
}

template<typename Arg> void String::catf(const char * fmt, const Arg & arg)
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
void String::catf(const char * fmt, const Arg & arg, const Args & ... args)
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

#define CSJP_C0 { if(!str.c_str()) i = 0; else i <<= CString(str.c_str()); return i; }
#define CSJP_C1 { if(!str.c_str()) i = 0u; else i <<= CString(str.c_str()); return i; }
#define CSJP_C2 { if(!str.c_str()) i = 0.0; else i <<= CString(str.c_str()); return i; }
#define CSJP_C3 { if(!str.c_str()) i = '\0'; else i <<= CString(str.c_str()); return i; }
#define CSJP_C4 { if(!str.c_str()) i = false; else i <<= CString(str.c_str()); return i; }

inline char			& operator<<=(char & i,			const String & str) CSJP_C0
inline unsigned char		& operator<<=(unsigned char & i,	const String & str) CSJP_C0
inline int			& operator<<=(int & i,			const String & str) CSJP_C0
inline long int			& operator<<=(long int & i,		const String & str) CSJP_C0
inline long long int		& operator<<=(long long int & i,	const String & str) CSJP_C0
inline unsigned			& operator<<=(unsigned & i,		const String & str) CSJP_C0
inline long unsigned		& operator<<=(long unsigned & i,	const String & str) CSJP_C0
inline long long unsigned	& operator<<=(long long unsigned & i,	const String & str) CSJP_C0
inline float			& operator<<=(float & i,		const String & str) CSJP_C0
inline double			& operator<<=(double & i,		const String & str) CSJP_C0
inline long double		& operator<<=(long double & i,		const String & str) CSJP_C0
inline UInt			& operator<<=(UInt & i,			const String & str) CSJP_C1
inline Double			& operator<<=(Double & i,		const String & str) CSJP_C2
inline Char			& operator<<=(Char & i,			const String & str) CSJP_C3
inline YNBool			& operator<<=(YNBool & i,		const String & str) CSJP_C4

/*}}}*/

}

#endif
