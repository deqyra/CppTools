@PACKAGE_INIT@
if (NOT TARGET cpptools::cpptools)
  add_library(cpptools::cpptools ALIAS cpptools::cpptools_@CPPTOOLS_STATIC_OR_SHARED@)
endif()