/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef CSJP_TEST_H
#define CSJP_TEST_H

#include <string.h>
#include <stdio.h>

#include <csjp_string.h>

#define TEST(...)	{ \
				csjp::String str; \
				str.append(VT_GREEN VT_TA_BOLD "TEST     " VT_NORMAL); \
				str.catf(__VA_ARGS__); \
				csjp::msgLogger(stdout, str.c_str(), \
						str.length); \
			}
#define TESTSTEP(...)	{ \
				csjp::String str; \
				str.append(VT_BLUE VT_TA_BOLD "TESTSTEP " VT_NORMAL); \
				str.catf(__VA_ARGS__); \
				csjp::msgLogger(stdout, str.c_str(), \
						str.length); \
			}
#define FAIL(...)	{ \
				csjp::String str; \
				str.append(VT_RED VT_TA_BOLD "FAIL     " VT_NORMAL); \
				str.catf(__VA_ARGS__); \
				csjp::msgLogger(stdout, str.c_str(), \
						str.length); \
			}

/* Excepts true return value. */
#define VERIFY(call)		if(!(call)) \
					throw csjp::TestFailure("Verify failed: " #call \
						"\nat line " STRING(__LINE__) " in file " __FILE__);

/* Excepts the given exception to be thrown. */
#define EXC_VERIFY(call, exc)	try{ \
					call; \
					throw csjp::TestFailure("Expected exception '" #exc \
							"' not thrown by '" #call "' at line " \
							STRING(__LINE__) " in file " __FILE__); \
				}catch(exc & e){ \
					; \
				}catch(csjp::TestFailure & e){ \
					throw (csjp::TestFailure &&)e; \
				}catch(csjp::Exception & ex){ \
					csjp::Exception e((csjp::Exception &&)ex); \
					e.note("Other than the expected exception '" \
						#exc "' is thrown by '" #call "'."); \
					throw (csjp::Exception &&)e; \
				}

/* Excepts no exception, nor return value. */
#define NOEXC_VERIFY(call)	try{ \
					call; \
				}catch(csjp::Exception & ex){ \
					csjp::Exception e((csjp::Exception &&)ex); \
					e.note("Unexpected exception thrown by '" \
							#call "'."); \
					throw (csjp::Exception &&)e; \
				}

/* Starts the main() function, instantiates TestObj object. */
#define TEST_INIT(obj)	int main(int argc, char *args[]) \
			{ \
				(void) argc; \
				(void) args; \
				csjp::setBinaryName(args[0]); \
				int argi = 1; \
				while(argi < argc){ \
					if(2 <= argc - argi && ( \
							!strcmp(args[argi], "--logdir") || \
							!strcmp(args[argi], "-l"))){ \
						csjp::setLogDir(args[argi+1]); \
						argi += 2; \
						continue; \
					} \
					if(1 <= argc - argi && ( \
							!strcmp(args[argi], "--verbose") || \
							!strcmp(args[argi], "-v"))){ \
						csjp::verboseMode = true; \
						argi++; \
						continue; \
					} \
					fprintf(stderr, "Bad arguments given.\n"); \
					return 1; \
				} \
				printf("----- Start testing " #obj ": \n"); \
				Test##obj test; \
				const char * testObjName = "Test" #obj; \
				bool good = true;

/* Calls the given named method of the test object. */
#define TEST_RUN(testname)	{ \
					TEST("%::", testObjName, #testname); \
					try { \
						test.testname(); \
					} catch(csjp::Exception & e) { \
						good = false; \
						FAIL("Test failed with the below exception:"); \
						EXCEPTION(e); \
					} \
				}

/* Finishes the main() function, reports the result. */
#define TEST_FINISH(obj)	printf("----- Finished testing " #obj "\n"); \
				return good ? 0 : -1 ; \
			}

#endif
