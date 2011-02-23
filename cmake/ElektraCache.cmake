#
# CACHE
#
# Here the cache variables are set

set (PLUGINS dump resolver CACHE STRING "Which plugins should be compiled?")

set (KDB_DB_SYSTEM "/etc" CACHE PATH "Where should be the system configuration?")

option (WITH_DOC "Build the documentation (API, man pages)" ON)

option (DEBUG_BUILD "Build with extra debug print messages.")
if (DEBUG_BUILD)
	set (DEBUG "1")
else (DEBUG_BUILD)
	set (DEBUG "0")
endif (DEBUG_BUILD)

option (ELEKTRA_VERBOSE_BUILD "Build with even more print messages.")
if (ELEKTRA_VERBOSE_BUILD)
	set (VERBOSE "1")
else (ELEKTRA_VERBOSE_BUILD)
	set (VERBOSE "0")
endif (ELEKTRA_VERBOSE_BUILD)

option (BUILD_SHARED "Build the shared version of elektra." ON)
option (BUILD_FULL "Build the full version of elektra (shared with all selected backends included)." ON)
option (BUILD_STATIC "Build the static version of elektra (all selected backends included statically)." ON)

set (CMAKE_DESTINATION
		"${CMAKE_INSTALL_PREFIX}/share/cmake-2.6/Modules"
		CACHE PATH
		"Where to install cmake files?"
    )

set (PKGCONFIG_DESTINATION
		"${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
		CACHE PATH
		"Where to install pkgconfig files?"
    )

set (DOCUMENTATION_DESTINATION
		"${CMAKE_INSTALL_PREFIX}/share/doc/elektra-api"
		CACHE PATH
		"Where to install documentation files?"
    )

set (MEMORYCHECK_COMMAND
		/usr/bin/valgrind
		CACHE FILEPATH
		"Path to valgrind the memory checker"
    )
