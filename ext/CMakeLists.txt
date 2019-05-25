aux_source_directory(./ EXT_ENTITY_SRC)
aux_source_directory(./src EXT_ENTITY_SRC1)
add_library(_entity_lib SHARED ${EXT_ENTITY_SRC} ${EXT_ENTITY_SRC1})

add_custom_target(entity_ext COMMAND make clean && make && sudo make install
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ext/entity/ext)

add_custom_target(entity_ext_clear COMMAND phpize --clean
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ext/entity/ext)

add_custom_target(entity_ext_all COMMAND phpize --clean && phpize && ./configure &&  make && make install
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ext/entity/ext)