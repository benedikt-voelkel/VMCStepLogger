

include_guard()


function(mcsl_add_root_dictionary)
  cmake_parse_arguments(
    PARSE_ARGV
    0
    A
    ""
    "LINKDEFDIR;TARGETNAME"
    "HEADERS"
  )
  if(A_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR "Unexpected unparsed arguments: ${A_UNPARSED_ARGUMENTS}")
  endif()

  set(basename "MCStepLogger")
  set(target "${basename}${A_TARGETNAME}")
  set(linkdef "${LINKDEFDIR}/${target}LinkDef.h")
  set(dictionaryFile "G__${target}.cxx")
  set(pcm "${PROJECT_BINARY_DIR}/lib${target}_rdict.pcm")
  set(rootmap "${PROJECT_BINARY_DIR}/lib${target}.rootmap")
  set(includeDirs $<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>)
  add_custom_command(
    OUTPUT ${dictionaryFile} ${pcm} ${rootmap}
    VERBATIM
    COMMAND
    ${CMAKE_BINARY_DIR}/rootcling_wrapper.sh
      --rootmap_file ${rootmap}
      --dictionary_file ${dictionaryFile}
      --ld_library_path ${LD_LIBRARY_PATH}
      --rootmap_library_name $<TARGET_FILE_NAME:${target}>
      --include_dirs -I$<JOIN:${includeDirs},$<SEMICOLON>-I>
      #$<$<BOOL:${prop}>:--compile_defs>
      #$<$<BOOL:${prop}>:-D$<JOIN:${prop},$<SEMICOLON>-D>>
      #--pcmdeps "$<REMOVE_DUPLICATES:${list_pcm_deps_${target}}>"
      --headers "${A_HEADERS}"
  )

  ROOT_GENERATE_DICTIONARY(${dictionaryFile} ${A_HEADERS} LINKDEF ${linkdef})
  target_sources(${target} PRIVATE ${dictionaryFile})

  # Files produced by the dictionary generation
  SET(ROOT_DICT_LIB_FILES
      "${PROJECT_BINARY_DIR}/lib${target}_rdict.pcm"
      "${PROJECT_BINARY_DIR}/lib${target}.rootmap")

  install(FILES ${ROOT_DICT_LIB_FILES}
          DESTINATION ${CMAKE_INSTALL_LIBDIR})
endfunction()
