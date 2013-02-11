SET (SE_OPTS "--color")

IF    (MODE STREQUAL "DUMB_TOOL")
  ADD_CUSTOM_TARGET(test-scripts ALL
                    COMMAND ${SHELLTEST_EXEC} ${SE_OPTS} "dumb-tool-basic.test"
                    COMMAND ${SHELLTEST_EXEC} ${SE_OPTS} "dumb-tool-extended.test"
                    DEPENDS "xa-tags"
                    COMMENT "Testing executable with scripts")
ENDIF (MODE STREQUAL "DUMB_TOOL")

