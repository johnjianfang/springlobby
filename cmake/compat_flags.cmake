Include(TestCXXAcceptsFlag)
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
				OUTPUT_VARIABLE GCC_VERSION)

# try to use compiler flag -std=c++11
include(TestCXXAcceptsFlag)
MACRO(AddSTDFlag FLAG)
	set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${FLAG} ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${FLAG} ")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${FLAG} ")
	set(CXX_STD0X_FLAGS "${FLAG}" )
ENDMACRO(AddSTDFlag FLAG)

# newer c++ standards are badly supported, disable it
#if(WIN32)
#	CHECK_CXX_ACCEPTS_FLAG("-std=gnu++0x" CXX_FLAG_GXX0X)
#	if(CXX_FLAG_GXX0X)
#		AddSTDFlag("-std=gnu++0x")
#	endif(CXX_FLAG_GXX0X)
#else(WIN32)
#	CHECK_CXX_ACCEPTS_FLAG("-std=c++11" CXX_FLAG_CXX11)
#	if(CXX_FLAG_CXX11)
#		AddSTDFlag("-std=c++11")
#	else()
#		# try to use compiler flag -std=c++0x for older compilers
#		CHECK_CXX_ACCEPTS_FLAG("-std=c++0x" CXX_FLAG_CXX0X)
#		if(CXX_FLAG_CXX0X)
#			AddSTDFlag("-std=c++0x")
#		endif(CXX_FLAG_CXX0X)
#	endif(CXX_FLAG_CXX11)
#endif(WIN32)

#if(NOT CXX_STD0X_FLAGS)
#	message(FATAL_ERROR "you need a c++11 compatible compiler")
#endif()

If(NOT DEFINED LTO_FLAGS)
	Set(LTO_FLAGS "")

	Set(LTO       FALSE CACHE BOOL "Link Time Optimizations (LTO)")
	If    (LTO)
		CHECK_CXX_ACCEPTS_FLAG("-flto" HAS_LTO_FLAG)
		If    (HAS_LTO_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -flto")
		Else  (HAS_LTO_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -flto")
		EndIf (HAS_LTO_FLAG)
	EndIf (LTO)

	Set(LTO_WHOPR FALSE CACHE BOOL "Link Time Optimizations (LTO) - Whole program optimizer (WHOPR)")
	If    (LTO_WHOPR)
		CHECK_CXX_ACCEPTS_FLAG("-fwhopr" HAS_LTO_WHOPR_FLAG)
		If    (HAS_LTO_WHOPR_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -fwhopr")
		EndIf (HAS_LTO_WHOPR_FLAG)
	EndIf (LTO_WHOPR)

	If (LTO AND LTO_WHOPR)
		Message( FATAL_ERROR "LTO and LTO_WHOPR are mutually exclusive, please enable only one at a time." )
	EndIf (LTO AND LTO_WHOPR)

EndIf (NOT DEFINED LTO_FLAGS)

# Add Link-Time-Optimization flags, if supported (GCC >= 4.5) and enabled
Set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}           ${LTO_FLAGS}")
Set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    ${LTO_FLAGS}")
Set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LTO_FLAGS}")
