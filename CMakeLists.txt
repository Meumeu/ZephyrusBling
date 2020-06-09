cmake_minimum_required(VERSION 3.0)

project(zephyrusbling LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(default_build_type Debug)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)

	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(CLI11 REQUIRED)
find_package(fmt REQUIRED)
find_package(glm REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(sdbus-c++ REQUIRED)
find_package(Threads REQUIRED)

# DBus adaptor / proxy created by:
# sdbus-c++-xml2cpp dbus/org.meumeu.bling.xml --adaptor=dbus/OrgMeumeuBlingAdaptor.h --proxy=dbus/OrgMeumeuBlingProxy.h && clang-format -i dbus/OrgMeumeuBling*.h
# sdbus-c++-xml2cpp dbus/org.rogcore.Daemon.xml --proxy=dbus/OrgRogcoreDaemonProxy.h && clang-format -i dbus/OrgRogcoreDaemonProxy.h

add_executable(zephyrusbling
	main.cpp
	Leds.cpp
	Image.cpp
	Bling.cpp
	BlingDaemon.cpp
	Effects.cpp)

set_property(TARGET zephyrusbling PROPERTY CXX_STANDARD 17)
set_property(TARGET zephyrusbling PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(zephyrusbling
	Boost::boost
	Boost::filesystem
	CLI11::CLI11
	fmt::fmt
	Microsoft.GSL::GSL
	SDBusCpp::sdbus-c++
	Threads::Threads
)

target_compile_options(zephyrusbling PRIVATE -Wall -Wextra -pedantic -fdiagnostics-color=always -Werror)
target_compile_options(zephyrusbling PRIVATE -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable)

install(TARGETS zephyrusbling RUNTIME DESTINATION bin)
