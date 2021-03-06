/*
bundle config --local path vendor/bundle
bundle install
bundle exec arduino_ci.rb  --skip-examples-compilation
bundle exec arduino_ci.rb  --skip-unittests
 */
#include "Arduino.h"
#include "ArduinoUnitTests.h"
#include "SD_CI.h"
#include <fstream>
#include <iostream>

unittest_setup() { SD.removeAll(); }

unittest_teardown() { SD.removeAll(); }

unittest(exists) {
  assertFalse(SD.exists("file.txt"));
  File file = SD.open("file.txt", FILE_WRITE);
  file.close();
  assertTrue(SD.exists("file.txt"));
}

unittest(mkdir) {
  assertTrue(SD.mkdir("test_directory"));
  assertTrue(SD.mkdir("test_directory/a"));
  assertTrue(SD.mkdir("test_directory/a/b"));
  assertTrue(SD.mkdir("test_directory/a/c"));
  assertTrue(SD.exists("test_directory"));
  assertTrue(SD.exists("test_directory/a/b"));
  assertTrue(SD.exists("test_directory/a/c"));
}

unittest(open) {
  // create file
  File writeFile = SD.open("file.txt", FILE_WRITE);
  writeFile.close();
  // open file for read should exist
  File readFile2 = SD.open("file.txt", FILE_READ);
  assertTrue(readFile2.isOpen());
  readFile2.close();
}

unittest(close) {
  // close write file
  File file = SD.open("file.txt", FILE_WRITE);
  assertTrue(file.isOpen());
  file.close();
  assertFalse(file.isOpen());

  // close read file
  File readFile = SD.open("file.txt", FILE_READ);
  assertTrue(readFile.isOpen());
  readFile.close();
  assertFalse(readFile.isOpen());
}

unittest(remove) {
  // set up
  File file = SD.open("file.txt", FILE_WRITE);
  file.close();
  assertTrue(SD.exists("file.txt"));

  SD.remove("file.txt");
  assertFalse(SD.exists("file.txt"));
}

unittest(rmdir) {
  // set up
  SD.mkdir("test_directory");
  SD.mkdir("test_directory/a");
  SD.mkdir("test_directory/a/a");
  SD.mkdir("test_directory/a/b");
  SD.mkdir("test_directory/a/c");
  File file = SD.open("test_directory/a/a/file.txt", FILE_WRITE);
  file.close();

  // remove directory
  assertTrue(SD.rmdir("test_directory/a/c"));
  // make sure non-removed dirs still exist
  assertTrue(SD.exists("test_directory"));
  assertTrue(SD.exists("test_directory/a/a"));
  // make sure removed dir no longer exists
  assertFalse(SD.exists("test_directory/a/c"));
}

unittest(name) {
  // set up
  File file = SD.open("newFile.txt", FILE_WRITE);
  assertEqual("newFile.txt", file.name());
  file.close();

  SD.mkdir("dir");
  file = SD.open("dir");
  assertEqual("dir", file.name());
  file.close();
}

unittest(seek) {
  // set up
  File file = SD.open("seek.txt", FILE_WRITE);
  char write[] = "Hide and Seek.";
  file.write(write, sizeof(write) - 1);
  file.close();
  File read = SD.open("seek.txt", FILE_READ);

  // Testing
  char readFrom[4];
  char expected[] = "and";
  read.seek(5);
  read.read(readFrom, 3);
  readFrom[3] = '\0';
  assertEqual(expected, readFrom);
}

unittest(read) {
  // set up
  File file = SD.open("birthday.txt", FILE_WRITE);
  char toWrite[] = "Happy Birthday to You!";
  file.write(toWrite, sizeof(toWrite) - 1);
  file.close();

  File file2 = SD.open("lines.txt", FILE_WRITE);
  char toWrite2[] = "line 1\nline2";
  file2.write(toWrite2, sizeof(toWrite2) - 1);
  file2.close();

  // testing
  File readFile = SD.open("birthday.txt", FILE_READ);
  size_t size = readFile.size();
  char readFromFile[size + 1];
  readFile.read(readFromFile, size);
  readFromFile[size] = '\0';
  readFile.close();

  // assertEqual(toWrite, readFromFile);

  File readFile2 = SD.open("lines.txt", FILE_READ);
  char readFromFile2[7 + 1];
  char readFromFile3[5 + 1];
  readFile2.read(readFromFile2, 7);
  readFile2.read(readFromFile3, 5);
  readFromFile2[7] = '\0';
  readFromFile3[5] = '\0';

  char expected2[] = "line 1\n";
  char expected3[] = "line2";
  assertEqual(expected2, readFromFile2);
  assertEqual(expected3, readFromFile3);
}

unittest(write) {
  // open new file for writing
  File writeFile = SD.open("wood.txt", FILE_WRITE);
  char toWrite[] = "How much wood could a wood pecker peck?\n";
  writeFile.write(toWrite, sizeof(toWrite) - 1);

  // read from same write file
  size_t size = writeFile.size();
  char readFromFile[size + 1];
  writeFile.seek(0);
  writeFile.read(readFromFile, size);
  readFromFile[size] = '\0';
  // assertEqual(toWrite, readFromFile);
  writeFile.close();

  // open old writing file to write at end.
  File writeFile2 = SD.open("wood.txt", FILE_WRITE);
  char toWrite2[] = "A lot of wood.\n";
  writeFile2.write(toWrite2, sizeof(toWrite2) - 1);
  writeFile2.close();

  // check old writing file
  File readWrite2 = SD.open("wood.txt", FILE_READ);
  size_t size2 = readWrite2.size();
  char toRead2[size2 + 1];
  readWrite2.read(toRead2, size2);
  toRead2[size] = '\0';
  char expected2[] =
      "How much wood could a wood pecker peck?\nA lot of wood.\n";
  // assertEqual(expected2, toRead2);
  readWrite2.close();
}

unittest(size) {
  // setup
  File sizeFile = SD.open("size.txt", FILE_WRITE);
  char toWrite[] = "Test text\n";
  sizeFile.write(toWrite, sizeof(toWrite) - 1);
  sizeFile.close();

  // test
  uint32_t fileSize = sizeFile.size();
  assertEqual(10, fileSize);
}

unittest(peek) {
  // set up
  File peekFile = SD.open("peek.txt", FILE_WRITE);
  char toWrite[] = "Peek file content\n";
  peekFile.write(toWrite, sizeof(toWrite) - 1);
  peekFile.close();

  // Test
  File readPeek = SD.open("peek.txt", FILE_READ);
  assertEqual('P', readPeek.peek());
  assertEqual('P', readPeek.peek());
  readPeek.close();

  File readWritePeek = SD.open("peek.txt", FILE_WRITE);
  readWritePeek.seek(0);
  assertEqual('P', readWritePeek.peek());
  assertEqual('P', readWritePeek.peek());
  readWritePeek.close();
}

unittest(position) {
  // set up
  File posFile = SD.open("pos.txt", FILE_WRITE);
  char toWrite[] = "This is the position file.\n";
  posFile.write(toWrite, sizeof(toWrite) - 1);

  // test
  assertEqual(27, posFile.position());
  posFile.seek(5);
  assertEqual(5, posFile.position());
  posFile.close();
}

unittest(isDirectory) {
  // set up
  SD.mkdir("test");
  File toRead = SD.open("read.txt", FILE_WRITE);
  toRead.close();
  File testFile = SD.open("test.txt", FILE_WRITE);
  File readFile = SD.open("read.txt", FILE_READ);

  // test
  File myDir = SD.open("test");
  assertTrue(myDir.isDirectory());
  assertFalse(testFile.isDirectory());
  assertFalse(readFile.isDirectory());

  // tear down
  testFile.close();
  readFile.close();
}

unittest(next) {
  /*
   * /c
   * /d/
   * /d/x
   * /d/y
   * /e
   */
  // create things out-of-order to confirm that we find in order
  SD.open("e", O_WRITE).close();
  SD.mkdir("d");
  SD.open("c", O_WRITE).close();
  SD.open("d/x", O_WRITE).close();
  SD.open("d/y", O_WRITE).close();

  File root = SD.open("/");
  assertTrue(root);
  assertTrue(root.isDirectory());

  File c = root.openNextFile();
  assertTrue(c);
  assertEqual(String("c"), c.name());
  assertFalse(c.isDirectory());
  c.close();

  File d = root.openNextFile();
  assertTrue(d);
  assertEqual(String("d"), d.name());
  assertTrue(d.isDirectory());
  File d1 = d.openNextFile();
  assertTrue(d1);
  assertEqual(String("x"), d1.name());
  assertFalse(d1.isDirectory());
  d1.close();
  File d2 = d.openNextFile();
  assertTrue(d2);
  assertEqual(String("y"), d2.name());
  assertFalse(d2.isDirectory());
  d2.close();
  File end = d.openNextFile();
  assertFalse(end);
  d.close();

  File e = root.openNextFile();
  assertTrue(e);
  assertEqual(String("e"), e.name());
  assertFalse(e.isDirectory());
  e.close();

  end = root.openNextFile();
  assertFalse(end);

  root.rewindDirectory();
  c = root.openNextFile();
  assertTrue(c);
  assertEqual(String("c"), c.name());
  c.close();
}

unittest_main()
