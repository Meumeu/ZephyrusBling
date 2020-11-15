cmake_minimum_required(VERSION 3.0)

project(zephyrusbling LANGUAGES CXX)
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
#
# qdbus --system org.rogcore.Daemon /org/rogcore/Daemon org.freedesktop.DBus.Introspectable.Introspect > ../dbus/org.rogcore.Daemon.xml
# sdbus-c++-xml2cpp dbus/org.rogcore.Daemon.xml --proxy=dbus/OrgRogcoreDaemonProxy.h && clang-format -i dbus/OrgRogcoreDaemonProxy.h

# Common options to the service and the client
add_library(zephyrusbling_common STATIC
	Leds.cpp
	Image.cpp
	Bling.cpp
	BlingDaemon.cpp
	Effects.cpp
)
# Include ${CMAKE_CURRENT_LIST_DIR}/../ to make CMake generate zephyrusbling/<header>.h imports for external project
# instead of just <header>.h
target_include_directories(zephyrusbling_common PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../")
target_compile_features(zephyrusbling_common PUBLIC cxx_std_17)
target_compile_options(zephyrusbling_common PUBLIC -Wall -Wextra -pedantic -fdiagnostics-color=always -Werror)
target_compile_options(zephyrusbling_common PUBLIC -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable)

target_compile_options(zephyrusbling_common PUBLIC -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fno-omit-frame-pointer)
target_compile_options(zephyrusbling_common PUBLIC -fsanitize=leak)
# target_compile_options(zephyrusbling_common INTERFACE -fsanitize=thread)
target_compile_options(zephyrusbling_common PUBLIC -fsanitize=undefined)
target_compile_options(zephyrusbling_common PUBLIC -fsanitize-address-use-after-scope)

target_link_options(zephyrusbling_common PUBLIC -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fno-omit-frame-pointer)
target_link_options(zephyrusbling_common PUBLIC -fsanitize=leak)
# target_link_options(zephyrusbling_common INTERFACE -fsanitize=thread)
target_link_options(zephyrusbling_common PUBLIC -fsanitize=undefined)
target_link_options(zephyrusbling_common PUBLIC -fsanitize-address-use-after-scope)

target_link_libraries(zephyrusbling_common
	Boost::boost
	fmt::fmt
	Microsoft.GSL::GSL
	SDBusCpp::sdbus-c++
	Threads::Threads
)

add_executable(zephyrusblingd
	zephyrusblingd.cpp
	Leds.cpp
	Image.cpp
	Bling.cpp
	BlingDaemon.cpp
	Effects.cpp)
target_link_libraries(zephyrusblingd zephyrusbling_common Boost::filesystem)
install(TARGETS zephyrusblingd RUNTIME DESTINATION bin)

add_executable(zephyrusbling zephyrusbling.cpp)
target_link_libraries(zephyrusbling zephyrusbling_common CLI11::CLI11)
install(TARGETS zephyrusbling RUNTIME DESTINATION bin)
