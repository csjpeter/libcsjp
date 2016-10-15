/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016-2016 Csaszar, Peter
 */

#include <csjp_test.h>
#include <csjp_exception.h>

namespace csjp {
	extern char g_ExitCodeOnSegmentationViolation;
}

class TestException
{
public:
	void segmentation_violation();
};

void do_segmentation_violation()
{
	volatile char * d = 0;
	*d++;
}

void TestException::segmentation_violation()
{
	csjp::g_ExitCodeOnSegmentationViolation = 0; // so that the test won't fail
	csjp::set_segmentation_fault_signal_handler();
	do_segmentation_violation();
}


TEST_INIT(Exception)

	TEST_RUN(segmentation_violation);

TEST_FINISH(Exception)
