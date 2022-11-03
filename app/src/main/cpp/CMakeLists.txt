# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        inlinehook32/inlineHook.c
        inlinehook32/relocate.c
        inlinehook32/hooktest32.cpp

        inlinehook64/And64InlineHook.hpp
        inlinehook64/And64InlineHook.cpp
        inlinehook64/hook_main.cpp
        inlinehook64/hooktest64.h

        native-lib.cpp
        dvm/dvm.cpp
#        dvm/dvm_getuid.cpp
#        dvm/anti_root.cpp
#        dvm/anti_get_upload_feild_id.cpp
#        dvm/anti_xposed.cpp
        dvm/anti_get_dex_so_feild_id.cpp

#        antiXposed/xposed-detector.cpp
#        #antiXposed/xposed.cpp
#        antiXposed/find-name.cpp
#        antiXposed/classloader.cpp
#        antiXposed/logging.cpp
#        antiXposed/hash.cpp
#        antiXposed/plt.cpp
#        antiXposed/tinynew.cpp


        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})