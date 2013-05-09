ENABLE_TESTING()

ADD_SUBDIRECTORY("buf")
ADD_SUBDIRECTORY("common")
ADD_SUBDIRECTORY("list")
ADD_SUBDIRECTORY("search")
ADD_SUBDIRECTORY("file")
ADD_SUBDIRECTORY("proto")
ADD_SUBDIRECTORY("uuid")

SET(SRC_FILES "../src")

ADD_LIBRARY(tests SHARED
  "${SRC_FILES}/common.c"
  "${SRC_FILES}/buf.c"
  "${SRC_FILES}/file.c"
  "${SRC_FILES}/list.c"
  "${SRC_FILES}/log.c"
  "${SRC_FILES}/proto.c"
  "${SRC_FILES}/search.c"
  "${SRC_FILES}/uuid.c")

FIND_PROGRAM(SHELLTEST_EXEC "shelltest")
IF    (SHELLTEST_EXEC)
  ADD_SUBDIRECTORY("scripts")
ELSE  (SHELLTEST_EXEC)
  MESSAGE(STATUS "shelltest not found, testing scripts disabled")
ENDIF (SHELLTEST_EXEC)
