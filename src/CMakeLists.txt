SET(SRC_FILES_DAEMON
    "common.c"
    "proto.c"
    "log.c"
    "data.c"
    "conn.c"
    "handlers.c"
    "buf.c"
    "db.c"
    )

SET(SRC_FILES_CLIENT
    "common.c"
    "log.c"
    "data.c"
    "file.c"
    )

# main targets
ADD_EXECUTABLE("xa-tags" ${SRC_FILES_CLIENT} "client.c")

IF     (NOT ${OPERATION_MODE} STREQUAL "DUMB_TOOL")
  TARGET_LINK_LIBRARIES("xa-tags" ${SQLITE3_LIBRARY})

  ADD_EXECUTABLE("xa-tags-daemon" ${SRC_FILES_DAEMON} "daemon.c")
  TARGET_LINK_LIBRARIES("xa-tags-daemon" ${SQLITE3_LIBRARY})
ENDIF  (NOT ${OPERATION_MODE} STREQUAL "DUMB_TOOL")
