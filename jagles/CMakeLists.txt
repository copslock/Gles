# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})
set(can_use_assembler TRUE)
enable_language(ASM)

set(CMAKE_CXX_FLAGS "-g -std=c++11  -D__STDC_FORMAT_MACROS -D__ANDROID__")


add_library( # Sets the name of the library.
             JAVideo

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).

                ../../../../src/main/jni/breakpad/client/linux/crash_generation/crash_generation_client.cc
                ../../../../src/main/jni/breakpad/client/linux/dump_writer_common/thread_info.cc
                ../../../../src/main/jni/breakpad/client/linux/dump_writer_common/ucontext_reader.cc
                ../../../../src/main/jni/breakpad/client/linux/handler/exception_handler.cc
                ../../../../src/main/jni/breakpad/client/linux/handler/minidump_descriptor.cc
                ../../../../src/main/jni/breakpad/client/linux/log/log.cc
                ../../../../src/main/jni/breakpad/client/linux/microdump_writer/microdump_writer.cc
                ../../../../src/main/jni/breakpad/client/linux/minidump_writer/linux_dumper.cc
                ../../../../src/main/jni/breakpad/client/linux/minidump_writer/linux_ptrace_dumper.cc
                ../../../../src/main/jni/breakpad/client/linux/minidump_writer/minidump_writer.cc
                ../../../../src/main/jni/breakpad/client/minidump_file_writer.cc
                ../../../../src/main/jni/breakpad/common/android/breakpad_getcontext.S
                ../../../../src/main/jni/breakpad/common/convert_UTF.c
                ../../../../src/main/jni/breakpad/common/md5.cc
                ../../../../src/main/jni/breakpad/common/string_conversion.cc
                ../../../../src/main/jni/breakpad/common/linux/elfutils.cc
                ../../../../src/main/jni/breakpad/common/linux/file_id.cc
                ../../../../src/main/jni/breakpad/common/linux/guid_creator.cc
                ../../../../src/main/jni/breakpad/common/linux/linux_libc_support.cc
                ../../../../src/main/jni/breakpad/common/linux/memory_mapped_file.cc
                ../../../../src/main/jni/breakpad/common/linux/safe_readlink.cc

             ../../../../src/main/jni/gles/GLES/GLESMath.c
             ../../../../src/main/jni/gles/GLES/BaseScreen.cpp
             ../../../../src/main/jni/gles/GLES/CircleDetection.cpp
             ../../../../src/main/jni/gles/GLES/CircleFrame.cpp
             ../../../../src/main/jni/gles/GLES/DynamicLibrary.cpp
             ../../../../src/main/jni/gles/GLES/ExpandFrameDraw.cpp
             ../../../../src/main/jni/gles/GLES/framedraw.cpp
             ../../../../src/main/jni/gles/GLES/GPUMotionDetection.cpp
             ../../../../src/main/jni/gles/GLES/GPUMotionTracking.cpp
             ../../../../src/main/jni/gles/GLES/GraphicBuffer.cpp
             ../../../../src/main/jni/gles/GLES/ImageLoading.cpp
             ../../../../src/main/jni/gles/GLES/JAFileTexture.cpp
             ../../../../src/main/jni/gles/GLES/NormalPlay.cpp
             ../../../../src/main/jni/gles/GLES/NormalScreen.cpp
             ../../../../src/main/jni/gles/GLES/OneScreen.cpp
             ../../../../src/main/jni/gles/GLES/ParametricManager.cpp
             ../../../../src/main/jni/gles/GLES/ParametricSurface.cpp
             ../../../../src/main/jni/gles/GLES/SelectedBox.cpp
             ../../../../src/main/jni/gles/GLES/sphere.cpp
             ../../../../src/main/jni/gles/GLES/SphereOne.cpp
             ../../../../src/main/jni/gles/GLES/SphereStitch.cpp
             ../../../../src/main/jni/gles/GLES/SphereStitch2.cpp
             ../../../../src/main/jni/gles/GLES/SphereStitchImp.cpp
             ../../../../src/main/jni/gles/GLES/ToursLoading.cpp
             ../../../../src/main/jni/gles/GLES/UpDownScreen.cpp
             ../../../../src/main/jni/gles/GLES/VertexObject.cpp
             ../../../../src/main/jni/gles/GLES/VertexObjectImpl.cpp
             ../../../../src/main/jni/gles/Media/ag711.cpp
             ../../../../src/main/jni/gles/Media/h264wh.cpp
             ../../../../src/main/jni/gles/Media/h265wh.cpp
             ../../../../src/main/jni/gles/Media/JAMedia.cpp
             ../../../../src/main/jni/gles/Media/JAPlayer.cpp
             ../../../../src/main/jni/gles/Media/JAText.cpp
             ../../../../src/main/jni/gles/Media/OpenALManager.cpp
             ../../../../src/main/jni/gles/Network/ConnectManager.cpp
             ../../../../src/main/jni/gles/Network/JAConnect.cpp
	         ../../../../src/main/jni/gles/Network/JAP2PConnector.cpp
             ../../../../src/main/jni/gles/Network/JAP2PConnector2.cpp
             ../../../../src/main/jni/gles/Network/JAP2PManager.cpp
             ../../../../src/main/jni/gles/platform/Android/GLVideoConnect.cpp
             ../../../../src/main/jni/gles/platform/Android/GLVideoDisplay.cpp
             ../../../../src/main/jni/gles/platform/Android/HWCodec.cpp
             ../../../../src/main/jni/gles/Utils/mediabuffer.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/android.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alAuxEffectSlot.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alBuffer.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alDatabuffer.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alEffect.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alError.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alExtension.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alFilter.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alListener.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alSource.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alState.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/OpenAL32/alThunk.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/ALc.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/alcConfig.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/alcEcho.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/alcModulator.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/alcReverb.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/alcRing.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/alcThread.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/ALu.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/bs2b.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/null.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/panning.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/mixer.c
            ../../../../src/main/jni/3rdParty/OpenAL-soft-android/Alc/audiotrack.c
            ../../../../src/main/jni/smartLink/com_app_jagles_smartlink_SmartLink.c
             )

include_directories(src/main/jni/gles/GLES/
                    src/main/jni/gles/Media/
                    src/main/jni/gles/Utils/
                    src/main/jni/gles/platform/Android/
                    src/main/jni/gles/Network/
                    src/main/jni/ffmpeg/include/
                    src/main/jni/3rdparty/OpenAL-soft-android
                    src/main/jni/3rdparty/OpenAL-soft-android/include
                    src/main/jni/3rdparty/OpenAL-soft-android/OpenAL32/Include/
                    src/main/jni/
                    src/main/jni/3rdparty/OpenAL-soft-android/Alc/
                    src/main/jni/breakpad/common/android/include/
                                                            src/main/jni/breakpad/client/linux/
                                                            src/main/jni/breakpad/
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
              log )

find_library(GLESv2-lib
             GLESv2)


find_library(EGL-lib
             EGL)



# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


add_library( avcodec
             SHARED
             IMPORTED )

add_library( avformat
             SHARED
             IMPORTED )
add_library( avutil
             SHARED
             IMPORTED )

add_library( swscale
             SHARED
             IMPORTED )

add_library( swresample
             SHARED
             IMPORTED )

add_library( jnnat
             SHARED
             IMPORTED )

add_library( arq
             SHARED
             IMPORTED )

add_library( kp2p
             SHARED
             IMPORTED )

add_library( iot
             SHARED
             IMPORTED )


add_library( RDTAPIs
             SHARED
             IMPORTED )

add_library( IOTCAPIs
             SHARED
             IMPORTED )


set_target_properties(avcodec
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/ffmpeg/lib/libavcodec.so)
set_target_properties(avformat
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/ffmpeg/lib/libavformat.so)
set_target_properties(avutil
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/ffmpeg/lib/libavutil.so)
set_target_properties(swresample
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/ffmpeg/lib/libswresample.so)
set_target_properties(swscale
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/ffmpeg/lib/libswscale.so)
set_target_properties(IOTCAPIs
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/jnnat/libIOTCAPIs.so)
set_target_properties(jnnat
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/jnnat/libjnnat.so)

set_target_properties(arq
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/jnnat/libarq.so)

set_target_properties(kp2p
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/jnnat/libP2PSDKClient.so)

set_target_properties(iot
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/jnnat/libIOTLink.so)


set_target_properties(RDTAPIs
                      PROPERTIES IMPORTED_LOCATION
                       ../../../../src/main/jni/jnnat/libRDTAPIs.so)



target_link_libraries(

                       JAVideo
                        avcodec
                        avformat
                        avutil
                        swresample
                        swscale
                        jnnat
                        kp2p
                        arq
                        iot
                       ${log-lib}
                       ${GLESv2-lib}
                       ${EGL-lib}
                       )


#add_custom_command(TARGET JAVideo POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy ../../../../build/intermediates/cmake/debug/obj/armeabi-v7a/libJAVideo.so  ../../../../src/main/jniLibs/armeabi-v7a/libJAVideo.so
#  )