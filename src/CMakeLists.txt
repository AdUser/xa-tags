SET(SRC_FILES_DAEMON
    "common.c"
    "proto.c"
    "log.c"
    "data.c"
    "conn.c"
    "handlers.c"
    "buf.c"
    "uuid.c"
    "db.c"
    )

SET(SRC_FILES_CLIENT
    "common.c"
    "log.c"
    "uuid.c"
    "data.c"
    "file.c"
    )

# main targets

IF     (NOT ${OPERATIONAL_MODE} STREQUAL "DUMB_TOOL")
  SET(SRC_FILES_CLIENT "${SRC_FILES_CLIENT}" "db.c")
  ADD_EXECUTABLE("xa-tags" ${SRC_FILES_CLIENT} "client.c")
  TARGET_LINK_LIBRARIES("xa-tags" ${SQLITE3_LIBRARY})

  ADD_EXECUTABLE("xa-tags-daemon" ${SRC_FILES_DAEMON} "daemon.c")
  TARGET_LINK_LIBRARIES("xa-tags-daemon" ${SQLITE3_LIBRARY})
ELSE   (NOT ${OPERATIONAL_MODE} STREQUAL "DUMB_TOOL")
  ADD_EXECUTABLE("xa-tags" ${SRC_FILES_CLIENT} "client.c")
ENDIF  (NOT ${OPERATIONAL_MODE} STREQUAL "DUMB_TOOL")
