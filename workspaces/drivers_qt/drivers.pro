

TEMPLATE = subdirs

SUBDIRS +=	    \
    $${PWD}/../../prj/tests/init_clean_module_test/init_clean_module_test.pro		\
    $${PWD}/../../prj/tests/hello_world_test						\
    $${PWD}/../../prj/tests/basic_file_ops_test



OTHER_FILES +=	    \
    $${PWD}/../../README.md								\
    $${PWD}/../../.gitignore								\
    $${PWD}/../../index.html
