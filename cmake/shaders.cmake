function(generate_shaders out_headers)
  set(result)
  foreach (src ${ARGN})
    get_filename_component(name ${src} NAME_WE)
    set(src_file "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    set(out_file "${CMAKE_CURRENT_BINARY_DIR}/${name}.h")

    add_custom_command(OUTPUT ${out_file}
            COMMAND ${CMAKE_COMMAND}
            -DSRC_FILE=${src_file}
            -DSYMBOL=${name}
            -DOUT_FILE=${out_file}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/bin2c.cmake
            DEPENDS ${src}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Creating header for shader ${src}"
            VERBATIM
            )
    list(APPEND result ${out_file})
  endforeach ()
  set(${out_headers} "${result}" PARENT_SCOPE)
endfunction()
