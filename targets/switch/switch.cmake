# VK-GL-CTS Nintendo Switch Target Configuration
# Configures dEQP to build for Nintendo Switch using SwitchGLES
#
# Usage:
#   cmake .. -DDEQP_TARGET=switch -DDEQP_TARGET_TOOLCHAIN=switch-toolchain

message("*** Nintendo Switch target (SwitchGLES + deko3d)")
set(DEQP_TARGET_NAME "Nintendo Switch")

# Switch-specific main() with nxlink stdio and default caselist
set(DEQP_PLATFORM_MAIN_SRC
	"${CMAKE_SOURCE_DIR}/framework/platform/switch/tcuSwitchMain.cpp"
	CACHE PATH "Switch-specific main()" FORCE)

# --- Static Linking (mandatory: Switch has no dlopen) ---
set(GLES_ALLOW_DIRECT_LINK ON CACHE BOOL "Direct link GL/EGL functions" FORCE)

# --- SwitchGLES library paths ---
set(SWITCHGLES_PATH "${CMAKE_SOURCE_DIR}/../switchGLES" CACHE PATH "Path to SwitchGLES")
get_filename_component(SWITCHGLES_PATH "${SWITCHGLES_PATH}" ABSOLUTE)

set(DEQP_GLES2_LIBRARIES "${SWITCHGLES_PATH}/lib/libSwitchGLES.a")
set(DEQP_EGL_LIBRARIES   "${SWITCHGLES_PATH}/lib/libSwitchGLES.a")

# Include paths for GL ES 2.0 / EGL headers
include_directories("${SWITCHGLES_PATH}/include")

# --- Additional compiler flags ---
# _XOPEN_SOURCE=600 enables POSIX.1-2001 features needed by delibs (pthreads, semaphores)
add_definitions(-D_XOPEN_SOURCE=600)

# --- Platform source files ---
set(TCUTIL_PLATFORM_SRCS
	switch/tcuSwitchPlatform.cpp
	switch/tcuSwitchPlatform.hpp
)

# --- Platform libraries ---
# Link order matters: SwitchGLES -> deko3d -> libnx -> standard libs
set(DEQP_PLATFORM_LIBRARIES
	"${SWITCHGLES_PATH}/lib/libSwitchGLES.a"
	deko3d
	nx
)
