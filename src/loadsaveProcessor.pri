QT       += core xml xmlpatterns
include($$PWD/../lib/common/src/common.pri)
include($$PWD/../lib/QAes/QAes/QAes.pri)

SOURCES +=  $$PWD/loadsaveprocessorxml.cpp \
	    $$PWD/loadsaveprocessorjson.cpp


INCLUDEPATH += $$PWD\

HEADERS  += $$PWD/iloadsaveprocessor.h \
            $$PWD/loadsaveprocessorxml.h \
	    $$PWD/loadsaveprocessorjson.h


DISTFILES += \
    $$PWD/../HISTORY.txt \
    $$PWD/../README.txt
