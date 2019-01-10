
cmake_minimum_required(VERSION 3.4.1)

add_library(duktape
        STATIC
        src/main/jni/duktape/src/duktape.c
        )


add_library(sqlite
        STATIC
        src/main/jni/sqlite/sqlite3.c
        )

add_library(event
            STATIC
            IMPORTED)

set_target_properties(event
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jni/libevent/${ANDROID_ABI}/libevent.a)


add_library(md5
        STATIC
        src/main/jni/md5/md5.c
        )

add_library(minizip
        STATIC
        src/main/jni/minizip/ioapi.c
        src/main/jni/minizip/ioapi_buf.c
        src/main/jni/minizip/ioapi_mem.c
        src/main/jni/minizip/unzip.c
        src/main/jni/minizip/zip.c
        )

add_library(ker
            SHARED
            src/main/jni/kk.c
            src/main/jni/global.cc
            src/main/jni/KerPage.cc
            src/main/jni/KerObject.cc
            src/main/jni/KerImage.cc
            src/main/jni/KerView.cc
            src/main/jni/KerCGContext.cc
            src/main/jni/KerUI.cc
            src/main/jni/KerHttp.cc
            src/main/jni/core/kk.cc
            src/main/jni/core/jit.cc
            src/main/jni/core/bio.cc
            src/main/jni/core/event.cc
            src/main/jni/core/timer.cc
            src/main/jni/core/sqlite.cc
            src/main/jni/core/crypto.cc
            src/main/jni/core/dispatch-event.cc
            src/main/jni/core/zombies.cc
            src/main/jni/core/uri.cc
            src/main/jni/core/http.cc
            src/main/jni/core/storage.cc
            src/main/jni/ui/ui.cc
            src/main/jni/ui/view.cc
            src/main/jni/ui/app.cc
            src/main/jni/ui/page.cc
            src/main/jni/ui/package.cc
            src/main/jni/ui/CGContext.cc
            )

include_directories(
            src/main/jni/duktape/src/
            src/main/jni/minizip
            src/main/jni/sqlite
            src/main/jni/libevent/include
            src/main/jni/md5
            src/main/jni/
            )

set_target_properties(ker PROPERTIES
    CXX_STANDARD 11
)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries(
                    ker
                    duktape
                    event
                    sqlite
                    md5
                    minizip
                    z
                    GLESv2
                    EGL
                    log
                    )
