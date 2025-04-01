if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    if (WIN32)
        set(VCPKG_TARGET_TRIPLET x64-windows)
    else ()
        if (FORCE_INTEL)
            set(CMAKE_OSX_DEPLOYMENT_TARGET 15.0)
            set(VCPKG_TARGET_ARCHITECTURE "x64")
            set(VCPKG_TARGET_TRIPLET x64-osx)
        else ()
            set(CMAKE_OSX_ARCHITECTURES arm64)
            set(VCPKG_TARGET_ARCHITECTURE "arm64")
            set(VCPKG_TARGET_TRIPLET arm64-osx)
        endif ()
        set(triplet_dir ${VCPKG_TARGET_TRIPLET})
        set(VCPKG_HOST_TRIPLET ${VCPKG_TARGET_TRIPLET})
    endif ()
    # Set up a local installation with vcpkg for the dependencies
    # starting with cloning the vcpkg with the git submodule setup (cmake handles cloning
    # if not found) and proceeding with bootstrapping it.
    set(local_vcpkg "${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg")
    # Sets up shop so that all dependencies can be found at .../include or .../lib
    set(VCPKG_INSTALLED_DIR "${local_vcpkg}_installed")
    set(vcpkg_platform_dir ${VCPKG_INSTALLED_DIR})
    set(vcpkg_installed_deps ${vcpkg_platform_dir}/${triplet_dir})
    # Bootstrap vcpkg
    set(vcpkg_cmake_script "${local_vcpkg}/scripts/buildsystems/vcpkg.cmake")
    # Clone the vcpkg submodule if not found and set cmake_toolchain
    # This should propagate "manifest" mode for vcpkg and further installation of dependencies
    set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    include(vcpkg)
endif ()
