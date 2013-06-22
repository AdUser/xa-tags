SET(SRC_DIR "../../src")

ADD_EXECUTABLE("uuid_id_validate"  "uuid_id_validate.c")
ADD_EXECUTABLE("uuid_id_parse"     "uuid_id_parse.c")

TARGET_LINK_LIBRARIES("uuid_id_validate"  "tests")
TARGET_LINK_LIBRARIES("uuid_id_parse"     "tests")

ADD_TEST("uuid/id_parse"       "uuid_id_parse")
ADD_TEST("uuid/id_validate"    "uuid_id_validate")
