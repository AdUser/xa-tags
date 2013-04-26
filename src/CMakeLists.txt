SET(SRC_FILES_DAEMON
    "common.c"
    "proto.c"
    "log.c"
    "list.c"
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
    "list.c"
    "file.c"
    "search.c"
    )

# main targets

IF     (${MODE} STREQUAL "DUMB_TOOL")
  ADD_EXECUTABLE("xa-tags" ${SRC_FILES_CLIENT} "client.c")
ENDIF  (${MODE} STREQUAL "DUMB_TOOL")

IF     (${MODE} STREQUAL "SMART_TOOL")
  ADD_EXECUTABLE("xa-tags" ${SRC_FILES_CLIENT} "client.c" "db.c")
  TARGET_LINK_LIBRARIES("xa-tags" ${SQLITE3_LIBRARY})
ENDIF  (${MODE} STREQUAL "SMART_TOOL")

IF     (${MODE} MATCHES "(DUMB|SMART)_CLIENT")
  ADD_EXECUTABLE("xa-tags" ${SRC_FILES_CLIENT} "client.c")
  ADD_EXECUTABLE("xa-tags-daemon" ${SRC_FILES_DAEMON} "daemon.c")

  TARGET_LINK_LIBRARIES("xa-tags" ${SQLITE3_LIBRARY})
  TARGET_LINK_LIBRARIES("xa-tags-daemon" ${SQLITE3_LIBRARY})
ENDIF  (${MODE} MATCHES "(DUMB|SMART)_CLIENT")
