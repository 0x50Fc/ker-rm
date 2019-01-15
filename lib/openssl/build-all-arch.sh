#!/bin/bash
#
# http://wiki.openssl.org/index.php/Android
#
set -e

#archs=(armeabi arm64-v8a mips mips64 x86 x86_64)

archs=(armeabi)

for arch in ${archs[@]}; do
    xLIB="/lib"
    case ${arch} in
        "armeabi")
            _ANDROID_TARGET_SELECT=arch-arm
            _ANDROID_ARCH=arch-arm
            _ANDROID_EABI=arm-linux-androideabi-4.9
            _ANDROID_ABI=arm-linux-androideabi
            configure_platform="android-armv7" ;;
        "arm64-v8a")
            _ANDROID_TARGET_SELECT=arch-arm64-v8a
            _ANDROID_ARCH=arch-arm64
            _ANDROID_EABI=aarch64-linux-android-4.9
            _ANDROID_ABI=aarch64-linux-android
            #no xLIB="/lib64"
            configure_platform="linux-generic64 -DB_ENDIAN" ;;
        "mips")
            _ANDROID_TARGET_SELECT=arch-mips
            _ANDROID_ARCH=arch-mips
            _ANDROID_EABI=mipsel-linux-android-4.9
            _ANDROID_ABI=mipsel-linux-android
            configure_platform="android -DB_ENDIAN" ;;
        "mips64")
            _ANDROID_TARGET_SELECT=arch-mips64
            _ANDROID_ARCH=arch-mips64
            _ANDROID_EABI=mips64el-linux-android-4.9
            _ANDROID_ABI=mips64el-linux-android
            xLIB="/lib64"
            configure_platform="linux-generic64 -DB_ENDIAN" ;;
        "x86")
            _ANDROID_TARGET_SELECT=arch-x86
            _ANDROID_ARCH=arch-x86
            _ANDROID_EABI=x86-4.9
            _ANDROID_ABI=i686-linux-android
            configure_platform="android-x86" ;;
        "x86_64")
            _ANDROID_TARGET_SELECT=arch-x86_64
            _ANDROID_ARCH=arch-x86_64
            _ANDROID_EABI=x86_64-4.9
            _ANDROID_ABI=x86_64-linux-android
            xLIB="/lib64"
            configure_platform="linux-generic64" ;;
        *)
            configure_platform="linux-elf" ;;
    esac

    . ./setenv-android-mod.sh

    echo "CROSS COMPILE ENV : $CROSS_COMPILE"
    cd openssl-OpenSSL_1_0_1-stable

    xCFLAGS="-DSHARED_EXTENSION=.so -D__ANDROID_API__ -fPIC -DOPENSSL_PIC -DDSO_DLFCN -DHAVE_DLFCN_H -mandroid -I$ANDROID_NDK_ROOT/sysroot/usr/include -I$ANDROID_NDK_ROOT/sysroot/usr/include/$_ANDROID_ABI -L$ANDROID_DEV$xLIB  -O3 -fomit-frame-pointer -Wall"

    perl -pi -e 's/install: all install_docs install_sw/install: install_docs install_sw/g' Makefile.org
    ./Configure shared no-threads no-asm no-zlib no-ssl2 no-ssl3 no-comp no-hw no-engine --openssldir=/usr/local/ssl/$_ANDROID_API/ $configure_platform $xCFLAGS

    # patch SONAME

    perl -pi -e 's/SHLIB_EXT=\.so\.\$\(SHLIB_MAJOR\)\.\$\(SHLIB_MINOR\)/SHLIB_EXT=\.so/g' Makefile
    perl -pi -e 's/SHARED_LIBS_LINK_EXTS=\.so\.\$\(SHLIB_MAJOR\) \.so//g' Makefile
    # quote injection for proper SONAME, fuck...
    perl -pi -e 's/SHLIB_MAJOR=1/SHLIB_MAJOR=`/g' Makefile
    perl -pi -e 's/SHLIB_MINOR=0.0/SHLIB_MINOR=`/g' Makefile
    make clean
    make depend
    make all

    file libcrypto.so
    file libssl.so
    file libcrypto.a
    file libssl.a

    if [ -d ../${arch} ]; then
    rm -rf ../${arch}
    fi

    mkdir ../${arch}

    cp libcrypto.so ../${arch}/libcrypto.so
    cp libssl.so ../${arch}/libssl.so
    cp libcrypto.a ../${arch}/libcrypto.a
    cp libssl.a ../${arch}/libssl.a
    cd ..
done
exit 0

