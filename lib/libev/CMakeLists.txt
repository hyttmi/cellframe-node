# Copyright (C) 2007-2013 LuaDist.
# Created by Peter Drahoš, Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( ev C )
cmake_minimum_required ( VERSION 3.1 )
include ( cmake/dist.cmake )
include ( configure )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set ( EV_SRC 
  ev.c
  event.c
)


if (WIN32) 
  list ( APPEND EV_LIBS Ws2_32 )
endif ()
set(_CCOPT "-std=c99")

add_library ( ${PROJECT_NAME} ${EV_SRC} )
target_link_libraries ( ${PROJECT_NAME} ${EV_LIBS} )
target_include_directories( ${PROJECT_NAME} INTERFACE .)


install_library ( ${PROJECT_NAME} )
install_header ( ev.h ev++.h event.h )
install_data ( LICENSE README )
