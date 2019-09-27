
TEMPLATE = aux
DEFINES += __KERNEL__
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += /usr/src/linux-headers-4.15.0-20-generic/include

SOURCES +=      \
    $${PWD}/../../../src/tests/drventry_basic_file_ops_test.c


OTHER_FILES +=  \
    $${PWD}/basic_file_ops_test.Makefile
