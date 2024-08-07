# CMAKE version of Ubuntu 16.04 LTS is 3.5.
cmake_minimum_required(VERSION 3.5)
project(test)

set(LINKLIBS)
set(COMPILE_OPTION)
set(SRC)
set(INCL)

include(${CMAKE_CURRENT_LIST_DIR}/lib/RtAudioWrapper/RtAudio.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/lib/WAV/WAV.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/lib/STFT/cpp/STFT_cpp.cmake)
  
if(NOT MSVC)
	list(APPEND LINKLIBS
	# <math.h>
	m
	# openMP
    -fopenmp
	)
else()
	list(APPEND LINKLIBS
    
	)	
endif()

list(APPEND SRC
	test.cpp

	
	# TODO : Add path to algorithm source here.
)
	
list(APPEND INCL
	src
	lib/STFT/cpp
	lib/WAV
)

if(NOT MSVC)
	list(APPEND COMPILE_OPTION
	# openMP
	  -fopenmp
	# debugging
	  -g
	# -O3 : maximize performace
	# -O2 : maximize performance as long as not increasing size of executable
	# -O1 : minimize compile time and others are secondary
	# -Os : minimize size of executable
	  -O3
	)
	
else()
list(APPEND COMPILE_OPTION
# Visual Studio uses '/' for compile option

	  /openmp
	  # /MP - compiled in mult-process : faster build time
	  /MP
	)
endif()


add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME}	PRIVATE	${INCL}  )
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME} PROPERTIES  CXX_STANDARD 17 )


