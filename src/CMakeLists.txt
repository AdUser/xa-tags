SET(SRC_FILES "proto.c" "log.c" "data.c" "conn.c" "handlers.c" "buf.c")

# main targets
ADD_EXECUTABLE("xa-tags-daemon" ${SRC_FILES} "daemon.c")
