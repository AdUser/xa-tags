SET (POTFILE "${CNAME}.pot")
SET (SRC_FILES "../src/msg.h")
SET (LANGUAGES)
SET (XGETTEXT_OPTS
     "--package-name=${CMAKE_PROJECT_NAME}"
     "--package-version=${VERSION}"
     "--keyword=_"
     "--add-comments=TRANSLATORS:"
     "--foreign-user"
     "--msgid-bugs-address=${EMAIL}")

ADD_CUSTOM_TARGET(${POTFILE} ALL
                   COMMAND ${XGETTEXT_EXEC} --output ${POTFILE} ${XGETTEXT_OPTS} ${SRC_FILES}
                   SOURCES ${SRC_FILES})

FOREACH    (LANG ${LANGUAGES})
  ADD_CUSTOM_TARGET("${LANG}.po" ALL
                     COMMAND "msgfmt" "${LANG}.po"
                     SOURCES ${SRC_FILES})
ENDFOREACH (LANG)


FOREACH    (LANG ${LANGUAGES})
  INSTALL(FILES "${LANG}.mo"
          DESTINATION ${CMAKE_INSTALL_PREFIX}/${LOCALEDIR}/${LANG}
          RENAME "${CNAME}.mo")
ENDFOREACH (LANG)
