

TEMPLATE = subdirs

SUBDIRS +=	    \
    $${PWD}/../../prj/tests/init_clean_module_test/init_clean_module_test.pro		\
    $${PWD}/../../prj/tests/hello_world_test						\
    $${PWD}/../../prj/tests/basic_file_ops_test						\
    $${PWD}/../../prj/simple/user_space_fs						\
    $${PWD}/../../prj/utests/driver_test_qt/driver_test.pro



OTHER_FILES +=	    \
    $${PWD}/../../README.md								\
    $${PWD}/../../.gitignore								\
    $${PWD}/../../index.html
