/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#include "csjp_utility.h"

namespace csjp {

void basicLogger(const char * msg, FILE * stdfile, const char * binaryName)
{
	(void)binaryName;
	if(stdfile){
		fputs(msg, stdfile);
		fflush(stdfile);
	}
}

}
