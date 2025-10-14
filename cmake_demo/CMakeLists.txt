cmake_minimum_required(VERSION 3.20.0)

# name the project
project(
	hello_world
	VERSION 1.0
	DESCRIPTION "The classic"
	LANGUAGES C
)


# create a static library target named "my_lib"
add_library(
	my_lib
	STATIC
	src/my_lib.c
)

# set the include directories for library. PUBLIC adds directory
# to search path for any targets that link to this library
target_include_directories(
	my_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

# create executable target with the same name as the project name
add_executable(
	${PROJECT_NAME}
	src/main.c
)

# Link the library to executable. PRIVATE means that library is not 
# exposed to targets that tdepend on this target
target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	my_lib
)
