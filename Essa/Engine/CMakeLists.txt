function(add_essaengine_library TARGET_NAME LIB_NAME)
    add_library(Engine-${TARGET_NAME} ${SOURCES})
    add_library(Essa::Engine-${TARGET_NAME} ALIAS Engine-${TARGET_NAME})
    set_target_properties(Engine-${TARGET_NAME} PROPERTIES OUTPUT_NAME "essaengine-${LIB_NAME}")
    essautil_setup_target(Engine-${TARGET_NAME})
    install(TARGETS Engine-${TARGET_NAME} EXPORT Essa DESTINATION lib)
endfunction()

add_subdirectory(3D)
