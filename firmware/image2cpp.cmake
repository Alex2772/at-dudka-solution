function(image2cpp target path)
    find_package(Python REQUIRED COMPONENTS Interpreter)

    get_filename_component(_filename ${path} NAME)
    get_filename_component(path ${path} ABSOLUTE)

    set(_dst ${CMAKE_BINARY_DIR}/image2cpp/image2cpp.${_filename}.cpp)

    add_custom_command(OUTPUT ${_dst}
                       COMMAND Python::Interpreter ARGS ${CMAKE_CURRENT_SOURCE_DIR}/image2cpp.py ${path} ${_dst})

    target_sources(${target} PUBLIC ${_dst})
endfunction()