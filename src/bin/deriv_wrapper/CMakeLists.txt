set(headers_list "")

set(sources_list "")
# List of sources
list(APPEND sources_list main.cc )

# If you want to remove some sources specify them explictly here
if(DEVELOPMENT_CODE)
   list(REMOVE_ITEM sources_list "")
else()
   list(REMOVE_ITEM sources_list "")
endif()


# Build static library
add_library(deriv_wrapper STATIC ${sources_list})
# Specify dependencies for the library (if any)
add_dependencies(deriv_wrapper mints)
set_property(GLOBAL APPEND PROPERTY PSILIB deriv_wrapper)
if(BUILD_CUSTOM_BOOST)
   add_dependencies(deriv_wrapper custom_boost)
endif()

# Sets install directory for all the headers in the list
install_list_FILES("${headers_list}" include/psi4/bin/deriv_wrapper)
