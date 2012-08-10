SET(SRC_FILES
    "common.c"
    "proto.c"
    "log.c"
    "data.c"
    "conn.c"
    "handlers.c"
    "buf.c"
    )

# main targets
ADD_EXECUTABLE("xa-tags-daemon" ${SRC_FILES} "daemon.c")
TARGET_LINK_LIBRARIES("xa-tags-daemon" ${SQLITE3_LIBRARY})
