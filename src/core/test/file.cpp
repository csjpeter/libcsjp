/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include <csjp_file.h>
#include <csjp_test.h>

class TestFile
{
public:
	void createAndUnlink();
	void rename();
	void resize();
	void read();
	void append();
	//void lock();
	void temporary();
};

void TestFile::createAndUnlink()
{
	csjp::File testDir(TESTDIR);
	if(!testDir.exists() || !testDir.isDir())
		FATAL("This test requires the directory " TESTDIR " to exist and to be writable.");

	csjp::File dir(TESTDIR "/dir.test");

	TESTSTEP("Unlink dir if exists");
	if(dir.exists()) dir.removeDir();
	VERIFY(!dir.exists());

	TESTSTEP("Create directory");
	NOEXC_VERIFY(dir.createDir());

	TESTSTEP("Check dir");
	VERIFY(dir.exists());
	VERIFY(!dir.isRegular());
	VERIFY(dir.isDir());

	TESTSTEP("Remove directory");
	NOEXC_VERIFY(dir.removeDir());
	VERIFY(!dir.exists());

	csjp::File file(TESTDIR "/file.test");

	TESTSTEP("Unlink file if exists");
	if(file.exists()) file.unlink();
	VERIFY(!file.exists());

	TESTSTEP("Create file");
	NOEXC_VERIFY(file.create());

	TESTSTEP("Check file");
	VERIFY(file.exists());
	VERIFY(file.isRegular());
	VERIFY(!file.isDir());
	VERIFY(file.size() == 0);

	TESTSTEP("Unlink file");
	NOEXC_VERIFY(file.unlink());
	VERIFY(!file.exists());
}

void TestFile::rename()
{
	csjp::File file(TESTDIR "/file.test");

	TESTSTEP("Unlink file if exists");
	if(file.exists()) file.unlink();
	VERIFY(!file.exists());

	csjp::String data("7 bytes");

	TESTSTEP("Write into file");
	NOEXC_VERIFY(file.write(data));
	VERIFY(file.size() == 7);

	TESTSTEP("Rename file");
	NOEXC_VERIFY(file.rename(TESTDIR "/file2.test"));
	csjp::File file2(TESTDIR "/file2.test");
	csjp::String input = file2.readAll();
	VERIFY(input == "7 bytes");
}

void TestFile::resize()
{
	csjp::File file(TESTDIR "/file.test");

	TESTSTEP("Unlink file if exists");
	if(file.exists()) file.unlink();
	VERIFY(!file.exists());

	csjp::String data("7 bytes");
	csjp::String input;

	TESTSTEP("Write into file");
	NOEXC_VERIFY(file.write(data));
	VERIFY(file.size() == 7);

	TESTSTEP("Resize file");
	NOEXC_VERIFY(file.resize(6));
	VERIFY(file.size() == 6);
}

void TestFile::read()
{
	csjp::File file(TESTDIR "/file.test");

	TESTSTEP("Unlink file if exists");
	if(file.exists()) file.unlink();
	VERIFY(!file.exists());

	csjp::String data("7 byte");
	csjp::String dataS("s");
	csjp::String input;

	TESTSTEP("Write into file");
	NOEXC_VERIFY(file.write(data));
	VERIFY(file.size() == 6);

	TESTSTEP("Read all from file");
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(input = file.readAll());
	LOG("read in data: [%]", input);
	VERIFY(input == "7 byte");
	VERIFY(file.eof());

	TESTSTEP("Write at pos");
	NOEXC_VERIFY(file.rewind());
	file.writeAtPos(dataS, 6);

	TESTSTEP("Read all");
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(input = file.readAll());
	VERIFY(input == "7 bytes");
	VERIFY(file.eof());

	TESTSTEP("Read all from pos");
	NOEXC_VERIFY(input = file.readAllFromPos(2));
	VERIFY(input == "bytes");
	VERIFY(file.eof());

	TESTSTEP("Read from pos");
	NOEXC_VERIFY(input = file.readFromPos(2, 1));
	VERIFY(input == "b");

	TESTSTEP("Read");
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(input = file.read(1));
	VERIFY(input == "7");

	TESTSTEP("static readAll");
	NOEXC_VERIFY(input = csjp::File::readAll(TESTDIR "/file.test"));
	VERIFY(input == "7 bytes");
	csjp::Str chunk(TESTDIR "/file.test");
	NOEXC_VERIFY(input = csjp::File::readAll(chunk));
	VERIFY(input == "7 bytes");
	csjp::String str(TESTDIR "/file.test");
	NOEXC_VERIFY(input = csjp::File::readAll(str));
	VERIFY(input == "7 bytes");

	TESTSTEP("Unlink file");
	NOEXC_VERIFY(file.unlink());
	VERIFY(!file.exists());
}

void TestFile::append()
{
	csjp::File file(TESTDIR "/textfile.test");
	csjp::String str;

	TESTSTEP("Unlink file if exists");
	if(file.exists()) file.unlink();
	VERIFY(!file.exists());

	TESTSTEP("Create file");
	VERIFY(!file.exists());
	NOEXC_VERIFY(file.create());
	VERIFY(file.exists());

	TESTSTEP("Append a string");
	str = "kutyus";
	NOEXC_VERIFY(file.append(str));
	str.clear();
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(str = file.readAll());
	VERIFY(str == "kutyus");

	TESTSTEP("Append empty string");
	str = "";
	NOEXC_VERIFY(file.append(str));
	str.clear();
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(str = file.readAll());
	VERIFY(str == "kutyus");

	TESTSTEP("Append another string");
	str = "ok";
	NOEXC_VERIFY(file.append(str));
	str.clear();
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(str = file.readAll());
	VERIFY(str == "kutyusok");

	TESTSTEP("AppendPrintf another string");
	NOEXC_VERIFY(file.appendPrintf("n%sk", "a"));
	//TESTSTEP("Append another string");
	//NOEXC_VERIFY(file.appendf("%", "nak"));
	str.clear();
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(str = file.readAll());
	VERIFY(str == "kutyusoknak");

	TESTSTEP("Append another string");
	NOEXC_VERIFY(file.appendf("%", "!"));
	str.clear();
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(str = file.readAll());
	VERIFY(str == "kutyusoknak!");

	TESTSTEP("Unlink file");
	NOEXC_VERIFY(file.unlink());
	VERIFY(!file.exists());
}
#if 0
void TestFile::lock()
{
	csjp::File file(TESTDIR "/lockfile.test");

	TESTSTEP("Unlink file if exists");
	if(file.exists()) file.unlink();
	VERIFY(!file.exists());

	TESTSTEP("Lock file");
	NOEXC_VERIFY(file.lock());
	EXC_VERIFY(file.rename(TESTDIR "/renamed_lockfile.test"), csjp::FileError);

	TESTSTEP("Unlock file");
	NOEXC_VERIFY(file.unlock());
}
#endif

void TestFile::temporary()
{
	csjp::TempFile file(TESTDIR "/textfile.test.XXXXXX");
	csjp::String str;

	TESTSTEP("Append a string to file");
	LOG("Temporary file name: %", file.name());
	str = "kutyus";
	NOEXC_VERIFY(file.append(str));
	str.clear();
	NOEXC_VERIFY(file.rewind());
	NOEXC_VERIFY(str = file.readAll());
	VERIFY(str == "kutyus");

	TESTSTEP("Unlink file");
	NOEXC_VERIFY(file.unlink());
	VERIFY(!file.exists());
}

TEST_INIT(File)

	TEST_RUN(createAndUnlink);
	TEST_RUN(rename);
	TEST_RUN(resize);
	TEST_RUN(read);
	TEST_RUN(append);
	//TEST_RUN(lock);
	TEST_RUN(temporary);

TEST_FINISH(File)
