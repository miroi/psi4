function(configure_script infile outfile)
    configure_file(${infile} ${outfile} @ONLY)
    execute_process(COMMAND
        chmod 755 ${outfile} OUTPUT_QUIET)
endfunction()

# Convenient wrapper for the install(FILES ... DESTINATION ...) command
# for a list of elements (we tipically use it for header files)
macro(install_list_FILES list_of_files where)
	foreach(filename ${list_of_files})
		install(FILES ${filename} DESTINATION ${where})
	endforeach()
endmacro()

# Generate the FCMangle header and post-process it to add the copyright notice
#RMR-Added a check to see if it exists first.  Recompiling after touching a 
#CMakeLists.txt was causing it to be rebuilt, which in turn caused any file that
#depended on it to also be rebuilt...kinda annoying...
macro(init_FCMangle)
   set(FCMangleHeader ${PROJECT_BINARY_DIR}/include/FCMangle.h)
   if(NOT EXISTS ${FCMangleHeader})
        FortranCInterface_HEADER(${FCMangleHeader} MACRO_NAMESPACE "FC_")
        file(STRINGS ${FCMangleHeader} contents NEWLINE_CONSUME)
        file(WRITE ${FCMangleHeader} 
                    "/*\n *@BEGIN LICENSE\n *@END LICENSE\n */\n\n")
        file(APPEND ${FCMangleHeader}  
             "// Header file automagically generated by CMake. DO NOT TOUCH!\n")
        file(APPEND ${FCMangleHeader} ${contents})
   endif()
endmacro()

function(test_fortran_mangling CODE PREFIX ISUPPER POSTFIX FLAGS DOC SUB RESULT)
  if(ISUPPER)
    string(TOUPPER "${SUB}" sub)
  else(ISUPPER) 
    string(TOLOWER "${SUB}" sub)
  endif(ISUPPER)
  set(FUNCTION "${PREFIX}${sub}${POSTFIX}")
  # create a fortran file with sub called sub
  set(TMP_DIR
    "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/CheckFortranLink")
  file(REMOVE_RECURSE "${TMP_DIR}")
  file(WRITE "${TMP_DIR}/test.f" "${CODE}"    )
  message(STATUS "checking Fortran ${DOC} linkage: ${FUNCTION}")
  file(WRITE "${TMP_DIR}/ctof.c"
    "
      extern ${FUNCTION}();
      int main() { ${FUNCTION}(); return 0;}
    "
    )
  file(WRITE "${TMP_DIR}/CMakeLists.txt"
    "
     project(testf C Fortran)
     set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} ${FLAGS}\")
     add_library(flib test.f)
     add_executable(ctof ctof.c)
     target_link_libraries(ctof flib)
    "
    )
  set(FORTRAN_NAME_MANGLE_TEST FALSE)
  try_compile(FORTRAN_NAME_MANGLE_TEST "${TMP_DIR}" "${TMP_DIR}"
    testf
    OUTPUT_VARIABLE output)
  #if(output)
  #  message(${output})
  #  endif()
  if(FORTRAN_NAME_MANGLE_TEST)
    set(${RESULT} TRUE PARENT_SCOPE)
  else()
    set(${RESULT} FALSE PARENT_SCOPE)
  endif()
endfunction(test_fortran_mangling)

function(get_fc_symbol FCSYMBOLOUT)
    set(TESTCODE 
    "
      subroutine sub
      end subroutine sub
    ")
    #test_fortran_mangling(    CODE    pre isUpper post print_test sub worked )
    test_fortran_mangling("${TESTCODE}" "" True    "_" "" "FUNCTION_" "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 4 PARENT_SCOPE)
        message(STATUS "Upper case with underscore is used")
        return()
    endif()
    test_fortran_mangling("${TESTCODE}" "" False   "_" "" "function_" "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 2 PARENT_SCOPE)
        message(STATUS "Lower case with underscore is used")
        return()
    endif()
    test_fortran_mangling("${TESTCODE}" "" True    "" "" "FUNCTION"  "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 3 PARENT_SCOPE)
        message(STATUS "Upper case (no underscore) is used")
        return()
    endif()
    test_fortran_mangling("${TESTCODE}" "" False   "" "" "function"  "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 1 PARENT_SCOPE)
        message(STATUS "Lower case (no underscore) is used")
        return()
    endif()
    message(STATUS "Unable to detect Fortran name mangling, trying again with 32-bit")
    test_fortran_mangling("${TESTCODE}" "" True    "_" "-m32" "FUNCTION_" "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 4 PARENT_SCOPE)
        message(STATUS "Upper case with underscore is used")
        return()
    endif()
    test_fortran_mangling("${TESTCODE}" "" False   "_" "-m32" "function_" "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 2 PARENT_SCOPE)
        message(STATUS "Lower case with underscore is used")
        return()
    endif()
    test_fortran_mangling("${TESTCODE}" "" True    "" "-m32" "FUNCTION"  "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 3 PARENT_SCOPE)
        message(STATUS "Upper case (no underscore) is used")
        return()
    endif()
    test_fortran_mangling("${TESTCODE}" "" False   "" "-m32" "function"  "sub" FC_LINK_WORKED)
    if(FC_LINK_WORKED)
        set(${FCSYMBOLOUT} 1 PARENT_SCOPE)
        message(STATUS "Lower case (no underscore) is used")
        return()
    endif()
    message(FATAL_ERROR "Unable to detect Fortran name mangling. This should not happen, please set --with-f77symbol manually.")
endfunction(get_fc_symbol)

# This macro configures all files that depend on some variables known only when 
# the setup script or CMake are run
macro(configure_files)
    ## Configure the counter utility script
    #configure_file(${PROJECT_SOURCE_DIR}/tools/counter.py.in counter.py)
    #file(COPY ${PROJECT_BINARY_DIR}/counter.py 
    #  DESTINATION ${PROJECT_BINARY_DIR}/bin
    #  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
    #  GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    ## Configure the create_bar_charts utility script
    #configure_file(${PROJECT_SOURCE_DIR}/tools/create_bar_charts.py.in create_bar_charts.py)
    #file(COPY ${PROJECT_BINARY_DIR}/create_bar_charts.py 
    #  DESTINATION ${PROJECT_BINARY_DIR}/bin
    #  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
    #  GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endmacro()
