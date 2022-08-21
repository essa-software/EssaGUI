function(add_essaengine_library TARGET_NAME LIB_NAME)
    add_library(EssaEngine-${TARGET_NAME} ${SOURCES})
    add_library(EssaGUI::EssaEngine-${TARGET_NAME} ALIAS EssaEngine-${TARGET_NAME})
    set_target_properties(EssaEngine-${TARGET_NAME} PROPERTIES OUTPUT_NAME "essaengine-${LIB_NAME}")
    essautil_setup_target(EssaEngine-${TARGET_NAME})
    install(TARGETS EssaEngine-${TARGET_NAME} EXPORT EssaGUI DESTINATION lib)
endfunction()

add_subdirectory(3D)
