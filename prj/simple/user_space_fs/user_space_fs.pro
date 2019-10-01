
TEMPLATE = aux
DEFINES += __KERNEL__
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += /usr/src/linux-headers-4.15.0-20-generic/include
INCLUDEPATH += /usr/src/linux-headers-4.15.0-20/arch/x86/include
INCLUDEPATH += $${PWD}/../../../include

SOURCES +=      \
    $${PWD}/../../../src/simple/drventry_user_space_fs.c	    \
    $${PWD}/../../../src/simple/fs_funcs_user_space_fs.c
    

HEADERS +=	\
    $${PWD}/../../../include/usr/user_space_fs_drv_user.h	    \
    $${PWD}/../../../include/kernel/user_space_fs_drv_kernel.h


OTHER_FILES +=  \
    $${PWD}/user_space_fs.Makefile
