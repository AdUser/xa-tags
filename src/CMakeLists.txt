SET(SRC_FILES "proto.c" "log.c")

# main targets
ADD_EXECUTABLE("xa-tags-daemon" ${SRC_FILES} "daemon.c")

# helpers
ADD_EXECUTABLE("crc24" "crc24.c")
