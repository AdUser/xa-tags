SET (SE_OPTS "--color")

IF    (MODE STREQUAL "DUMB_TOOL")
  ADD_CUSTOM_TARGET(test-scripts ALL
                    COMMAND ${SHELLTEST_EXEC} ${SE_OPTS} "dumb-tool-basic.test"
                    COMMAND ${SHELLTEST_EXEC} ${SE_OPTS} "dumb-tool-extended.test"
                    DEPENDS "xa-tags"
                    COMMENT "Testing executable with scripts")
ENDIF (MODE STREQUAL "DUMB_TOOL")

IF    (MODE STREQUAL "SMART_TOOL")
  CONFIGURE_FILE("smart-tool-basic.test.in" "smart-tool-basic.test.out")
  ADD_CUSTOM_TARGET(test-scripts ALL
                    COMMAND ${SHELLTEST_EXEC} ${SE_OPTS} "smart-tool-basic.test.out"
                    DEPENDS "xa-tags"
                    COMMENT "Testing executable with scripts")
ENDIF (MODE STREQUAL "SMART_TOOL")
