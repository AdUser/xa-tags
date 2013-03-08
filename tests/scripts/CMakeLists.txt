SET (SE_OPTS "--color")

IF    (MODE STREQUAL "DUMB_TOOL")
  ADD_TEST("script-testing/basic"    ${SHELLTEST_EXEC} ${SE_OPTS} "dumb-tool-basic.test")
  ADD_TEST("script-testing/extended" ${SHELLTEST_EXEC} ${SE_OPTS} "dumb-tool-extended.test")
ENDIF (MODE STREQUAL "DUMB_TOOL")

IF    (MODE STREQUAL "SMART_TOOL")
  CONFIGURE_FILE("smart-tool-basic.test.in" "smart-tool-basic.test.out")
  ADD_TEST("script-testing/basic"    ${SHELLTEST_EXEC} ${SE_OPTS} "smart-tool-basic.test.out")
ENDIF (MODE STREQUAL "SMART_TOOL")
