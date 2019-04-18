cmake_minimum_required(VERSION 3.10)

string(COMPARE EQUAL "$ENV{TRAVIS}" "true" is_travis)
string(COMPARE EQUAL "$ENV{APPVEYOR}" "True" is_appveyor)

option(FN_FAST_INVSQRT "Use fast inverse sqrt" OFF)

if(is_travis OR is_appveyor)
    set(FN_BUILD_APP ON) #force building app on CI
endif()

project(FastNoiseSIMD VERSION 0.8.0)

if(MSVC)
    #setup pdb info
    set(pdb_output_dir "${CMAKE_CURRENT_BINARY_DIR}/pdb-files")

    set(CMAKE_PDB_OUTPUT_DIRECTORY "${pdb_output_dir}")
    set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${pdb_output_dir}") 

    #need to sync pdp files
    add_compile_options("/FS")
endif()

set(FN_SIMD_SSE2 OFF)
set(FN_SIMD_SSE41 OFF)
set(FN_SIMD_AVX2 OFF)
set(FN_SIMD_AVX512 OFF)
set(FN_SIMD_NEON OFF)

set(FN_NO_FILESYSTEM 0)
set(FN_STD_FILESYSTEM 1)
set(FN_EXPERIMENTAL_FILESYSTEM 2)
set(FN_USE_FILESYSTEM ${FN_NO_FILESYSTEM})

set(FN_ARM64 OFF)

if(ANDROID_NDK_VERSION)
    string(SUBSTRING ${CMAKE_ANDROID_ARCH_ABI} 0 5 FM_ANDROID_ARCH)
    string(TOLOWER ${FM_ANDROID_ARCH} FM_ANDROID_ARCH)
    string(COMPARE EQUAL ${FM_ANDROID_ARCH} "arm64" FN_ARM64)
elseif(IOS_SDK_VERSION)
    string(SUBSTRING ${IPHONEOS_ARCHS} 0 3 FM_IOS_ARCH)
    string(TOLOWER ${FM_IOS_ARCH} FM_IOS_ARCH)
    string(COMPARE EQUAL ${FM_IOS_ARCH} "arm" FN_ARM64)
endif()

#get options based on architecture and compilier
if(FN_ARM64)
    set(FN_SIMD_NEON ON)
else()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        message(STATUS "MSVC_VERSION: ${MSVC_VERSION}") 
        add_definitions(-DNOMINMAX)
    
        set(FN_SIMD_SSE2 ON)
        set(FN_SIMD_SSE41 ON)

        if(MSVC_VERSION GREATER_EQUAL 1900)
            set(FN_SIMD_AVX2 ON)
            set(FN_USE_FILESYSTEM ${FN_EXPERIMENTAL_FILESYSTEM})
            if(MSVC_VERSION GREATER 1900)
               set(FN_SIMD_AVX512 ON) #something wrong finding floor function
                include(CheckCXXCompilerFlag)

                CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp_17_flag_supported)
                if(_cpp_17_flag_supported)
                    add_compile_options("/std:c++17")
                    set(FN_USE_FILESYSTEM ${FN_STD_FILESYSTEM})
                endif()
            endif()
        endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        set(FN_SIMD_SSE2 ON)
        set(FN_SIMD_SSE41 ON)
    
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.9)
            set(FN_SIMD_AVX2 ON)
            set(FN_SIMD_AVX512 ON)
        endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(FN_SIMD_SSE2 ON)
        set(FN_SIMD_SSE41 ON)
        set(FN_USE_FILESYSTEM ${FN_EXPERIMENTAL_FILESYSTEM})

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.6)
            set(FN_SIMD_AVX2 ON)
    
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.9)
                set(FN_SIMD_AVX512 ON)
            endif()

            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
                set(FN_USE_FILESYSTEM ${FN_STD_FILESYSTEM})
            endif()
        endif()
    endif()
endif()

message(STATUS "FN_SIMD_SSE2: ${FN_SIMD_SSE2}")
message(STATUS "FN_SIMD_SSE41: ${FN_SIMD_SSE41}")
message(STATUS "FN_SIMD_AVX2: ${FN_SIMD_AVX2}")
message(STATUS "FN_SIMD_AVX512: ${FN_SIMD_AVX512}")

set(fastNoise_sources
    FastNoiseSIMD/FastNoiseSIMD.cpp
    FastNoiseSIMD/FastNoiseSIMD_none.cpp
)

set(fastNoise_headers
    FastNoiseSIMD/FastNoiseSIMD.h
    FastNoiseSIMD/FastNoiseSIMD_internal.h
    FastNoiseSIMD/FastNoiseSIMD_internal.inl
    FastNoiseSIMD/simd_constants.inl
    FastNoiseSIMD/internal_none.inl
    FastNoiseSIMD/simd_init.inl
)

add_library(fastNoise SHARED
    ${fastNoise_sources}
    ${fastNoise_headers}
)
set(library_targets ${library_targets} fastNoise)

target_compile_definitions(fastNoise PRIVATE "-DFN_USE_FILESYSTEM=${FN_USE_FILESYSTEM}")
if(FN_FAST_INVSQRT)
    target_compile_definitions(fastNoise PUBLIC "-DFN_FAST_INVSQRT")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(fastNoise PUBLIC "/bigobj")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    target_compile_options(fastNoise PUBLIC "-ldl")
    if(FN_USE_FILESYSTEM GREATER 0)
        target_compile_options(fastNoise PUBLIC "-lstdc++fs")
        target_link_libraries(fastNoise stdc++fs ${CMAKE_DL_LIBS})
    endif()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND is_travis)
        target_compile_options(fastNoise PUBLIC "-Wno-ignored-attributes") #gcc warnings are mean to travis logs
    endif()
endif()
#target_include_directories(fastNoise PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(FN_SIMD_SSE2)
    set(fastNoise_sse2_sources
        FastNoiseSIMD/FastNoiseSIMD_sse2.cpp
    )

    set(fastNoise_sse2_headers
        FastNoiseSIMD/internal_sse2.inl
    )

    add_library(fastNoise_sse2 SHARED
        ${fastNoise_sse2_sources}
        ${fastNoise_sse2_headers}
    )
    set(library_targets ${library_targets} fastNoise_sse2)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(fastNoise_sse2 PUBLIC "/bigobj")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        target_compile_options(fastNoise_sse2 PRIVATE "-msse2")
    endif()
    target_link_libraries(fastNoise_sse2 fastNoise)
endif()

if(FN_SIMD_SSE41)
    set(fastNoise_sse41_sources
        FastNoiseSIMD/FastNoiseSIMD_sse41.cpp
    )

    set(fastNoise_sse41_headers
        FastNoiseSIMD/internal_sse41.inl
    )

    add_library(fastNoise_sse41 SHARED
        ${fastNoise_sse41_sources}
        ${fastNoise_sse41_headers}
    )
    set(library_targets ${library_targets} fastNoise_sse41)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(fastNoise_sse41 PUBLIC "/bigobj")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        target_compile_options(fastNoise_sse41 PRIVATE "-msse4.1")
    endif()
    target_link_libraries(fastNoise_sse41 fastNoise)
endif()

if(FN_SIMD_AVX2)
    set(fastNoise_avx2_sources
        FastNoiseSIMD/FastNoiseSIMD_avx2.cpp
    )

    set(fastNoise_avx2_headers
        FastNoiseSIMD/internal_avx2.inl
    )

    add_library(fastNoise_avx2 SHARED
        ${fastNoise_avx2_sources}
        ${fastNoise_avx2_headers}
    )
    set(library_targets ${library_targets} fastNoise_avx2)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(fastNoise_avx2 PRIVATE "/arch:AVX2" "/bigobj")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        target_compile_options(fastNoise_avx2 PRIVATE "-mavx2" "-mfma")
    endif()
    target_link_libraries(fastNoise_avx2 fastNoise)
endif()

if(FN_SIMD_AVX512)
    set(fastNoise_avx512_sources
        FastNoiseSIMD/FastNoiseSIMD_avx512.cpp
    )

    set(fastNoise_avx512_headers
        FastNoiseSIMD/internal_avx512.inl
    )

    add_library(fastNoise_avx512 SHARED
        ${fastNoise_avx512_sources}
        ${fastNoise_avx512_headers}
    )
    set(library_targets ${library_targets} fastNoise_avx512)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(fastNoise_avx512 PRIVATE "/arch:AVX512" "/bigobj")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        target_compile_options(fastNoise_avx512 PRIVATE "-mavx512f" "-mfma")
    endif()
    target_link_libraries(fastNoise_avx512 fastNoise)
endif()

if(FN_BUILD_APP)
    include_directories(${CMAKE_CURRENT_LIST_DIR})
    add_executable(fastNoiseApp testApp/main.cpp)
    set(library_targets ${library_targets} fastNoiseApp)
    
    set(fastNoise_libs
        fastNoise
    )
    
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(fastNoise_libs ${fastNoise_libs}
            Mincore.lib
        )
    endif()
    target_link_libraries(fastNoiseApp ${fastNoise_libs})
    target_compile_definitions(fastNoiseApp PRIVATE "-DFN_USE_FILESYSTEM=${FN_USE_FILESYSTEM}")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        if(FN_USE_FILESYSTEM GREATER 0)
            target_compile_options(fastNoiseApp PUBLIC "-lstdc++fs")
        endif()
    endif()
    
    
    if(FN_SIMD_SSE2)
        add_dependencies(fastNoiseApp fastNoise_sse2)
    endif()
    if(FN_SIMD_SSE41)
        add_dependencies(fastNoiseApp fastNoise_sse41)
    endif()
    if(FN_SIMD_AVX2)
        add_dependencies(fastNoiseApp fastNoise_avx2)
    endif()
    if(FN_SIMD_AVX512)
        add_dependencies(fastNoiseApp fastNoise_avx512)
    endif()
endif()


#Install -----------------------------------------------------------

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
include(GNUInstallDirs) 

# Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

# Targets:
#   * <prefix>/lib/libname.a
#   * header location after install: <prefix>/include/${PROJECT_NAME}/include.hpp
#   * headers can be included by C++ code `#include <${PROJECT_NAME}/include.hpp>`
install(
    TARGETS ${library_targets}
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Headers:
install(
    FILES "FastNoiseSIMD/FastNoiseSIMD.h" "FastNoiseSIMD/FastNoiseSIMD_export.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)

# Config
#   * <prefix>/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake
#   * <prefix>/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake
install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)

if(MSVC)
    #install pdbs
    get_cmake_property(is_multi GENERATOR_IS_MULTI_CONFIG)

    if(is_multi)
      set(config_suffix "$<CONFIG>")
    else()
      set(config_suffix "")
    endif() 

    if(BUILD_SHARED_LIBS)
      set(pdb_dst ${CMAKE_INSTALL_BINDIR})
    else()
      set(pdb_dst ${CMAKE_INSTALL_LIBDIR})
    endif() 

    install(
        DIRECTORY "${pdb_output_dir}/${config_suffix}/"
        DESTINATION ${pdb_dst}
    )
endif()