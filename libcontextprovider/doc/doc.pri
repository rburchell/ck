DOXYGEN_BIN=doxygen

QMAKE_EXTRA_UNIX_TARGETS += doc
doc.target = doc
isEmpty(DOXYGEN_BIN) {
    doc.commands = @echo "Unable to detect doxygen in PATH"
} else {
    doc.commands = @$${DOXYGEN_BIN} doc/doxy.cfg  ;
}
doc.depends = FORCE
