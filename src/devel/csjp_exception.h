/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef CSJP_EXCEPTION_H
#define CSJP_EXCEPTION_H

#include <stdarg.h>

#include <stdexcept>

#include <csjp_defines.h>
#include <csjp_ctypes.h>

namespace csjp {

struct NotesNode;

class PrimeException : public std::exception
{
public:
	class iterator
	{
	public:
		iterator(const NotesNode * node) : node(node) {}
		iterator operator++();
		bool operator!=(const iterator & other) { return node != other.node; }
		const char * operator*() const;
	private:
		const NotesNode * node;
	};
public:
	/* FIXME
	 * Shit versions of gcc require copy construction on throwing, but
	 * I cheet and I implement moving instead, which however might 
	 * result in some unexpected situation. We will see. */
	explicit PrimeException(const PrimeException &);
	const PrimeException & operator=(const PrimeException &) = delete;

	PrimeException(PrimeException && temp);
	const PrimeException & operator=(PrimeException && temp)
	{
		id = temp.id;
		noMem = temp.noMem;
		name = temp.name;
		lastNode = temp.lastNode;

		temp.id = 0;
		temp.noMem = false;
		temp.name = 0;
		temp.lastNode = 0;

		return *this;
	}

	virtual ~PrimeException() throw();

	void note(const char * msg);
	void notePrintf(const char * format, ...) __attribute__ ((format (printf, 2, 3)));
	void vnotePrintf(const char * format, va_list args);
	virtual const char * what() const throw();
	iterator begin() const { return iterator(lastNode); }
	iterator end() const { return iterator(0); }

protected:
	explicit PrimeException();
	explicit PrimeException(const std::exception & e);
	explicit PrimeException(int err);
	void noteBacktrace();

protected:
	unsigned id;
	bool noMem;
	const char * name;
	NotesNode * lastNode;
};

void set_segmentation_fault_signal_handler();

#ifdef PERFMODE
#define ENSURE(expr, except) {;}
#define IN_SAFEMODE(expr) {;}
#else
#define ENSURE(expr, exc) \
	if(!(expr)) \
		throw exc("In file " __FILE__ " at line " STRING(__LINE__)  " in function ", \
				__PRETTY_FUNCTION__, " : Failed check: " #expr);
#define IN_SAFEMODE(expr) {expr;}
#endif

#define EXCLI "Thrown from file " __FILE__ " line " STRING(__LINE__) "\n"
}

#endif

/* STL exceptions:
 *
 * exception {};                         // <exception>
 * bad_alloc  : public exception {};     // new  <new>
 * bad_cast   : public exception {};     // dynamic_cast
 * bad_typeid : public exception {};     // typeid(0)
 * ios_base::failure : public exception {};  // unexpected() <ios>
 * bad_exception : public exception {};  // unexpected()
 *
 * runtime_error : public exception {};  // math. computation
 *
 * range_error     : public runtime_error {};
 * overflow_error  : public runtime_error {};
 * underflow_error : public runtime_error {};
 *
 * logic_error : public exception {};
 *
 * domain_error : public logic_error {}; // domain error
 * invalid_argument : public logic_error {}; // bitset char != 0 or 1
 * length_error : public logic_error {}; // std::string length exceeded
 * out_of_range : public logic_error {}; // bad index in cont. or string
 */
