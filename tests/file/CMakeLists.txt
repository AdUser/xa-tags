SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/list.c"
    "${SRC_DIR}/file.c"
    "${SRC_DIR}/uuid.c"
    )

ADD_EXECUTABLE("file_tags_get" ${SRC_FILES} "file_tags_get.c")
ADD_EXECUTABLE("file_tags_set" ${SRC_FILES} "file_tags_set.c")
ADD_EXECUTABLE("file_tags_clr" ${SRC_FILES} "file_tags_clr.c")
ADD_EXECUTABLE("file_tags_get_bulk" ${SRC_FILES} "file_tags_get_bulk.c")

ADD_EXECUTABLE("file_uuid_get" ${SRC_FILES} "file_uuid_get.c")
ADD_EXECUTABLE("file_uuid_set" ${SRC_FILES} "file_uuid_set.c")
ADD_EXECUTABLE("file_uuid_clr" ${SRC_FILES} "file_uuid_clr.c")

ADD_TEST("file/tags_get"   "file_tags_get")
ADD_TEST("file/tags_set"   "file_tags_set")
ADD_TEST("file/tags_clr"   "file_tags_clr")
ADD_TEST("file/tags_get_bulk"   "file_tags_get_bulk")

ADD_TEST("file/uuid_get"   "file_uuid_get")
ADD_TEST("file/uuid_set"   "file_uuid_set")
ADD_TEST("file/uuid_clr"   "file_uuid_clr")
