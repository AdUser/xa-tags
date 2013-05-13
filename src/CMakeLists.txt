SET(SRC_COMMON
    "common.c"
    "log.c"
    "uuid.c"
    "list.c"
    "file.c"
    "search.c"
    )

SET(SRC_DAEMON
    "proto.c"
    "daemon.c"
    "conn.c"
    "handlers.c"
    "buf.c"
    "db.c"
    )

# main targets

IF     (${BUILD_EMB_TOOL} STREQUAL "ON")
  ADD_EXECUTABLE("xat-emb"  ${SRC_COMMON} "tool-embedded.c")
ENDIF  (${BUILD_EMB_TOOL} STREQUAL "ON")

IF     (${BUILD_TOOL} STREQUAL "ON")
  ADD_EXECUTABLE("xat" ${SRC_COMMON} "tool.c" "db.c")
  TARGET_LINK_LIBRARIES("xat"   ${SQLITE3_LIBRARY})
ENDIF  (${BUILD_TOOL} STREQUAL "ON")

#IF     (${BUILD_CLIENT} STREQUAL "ON")
#  ADD_EXECUTABLE("xatc" ${SRC_COMMON} "main/client.c")
#  TARGET_LINK_LIBRARIES("xatc" ${SQLITE3_LIBRARY})
#ENDIF  (${BUILD_DAEMON} STREQUAL "ON")
#
#ENDIF  (${BUILD_DAEMON} STREQUAL "ON")
#  ADD_EXECUTABLE("xatd" ${SRC_COMMON} ${SRC_DAEMON})
#  TARGET_LINK_LIBRARIES("xatd" ${SQLITE3_LIBRARY})
#ENDIF  (${BUILD_CLIENT} STREQUAL "ON")
