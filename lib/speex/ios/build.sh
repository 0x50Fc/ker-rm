#!/bin/bash

DEVELOPER=`xcode-select -print-path`
ROOTDIR=`pwd`
DEST=${ROOTDIR}/
ARCHS="x86_64 armv7 armv7s arm64"

build() {

    LIBS=$1
    SRC=$2
    CONFIG=$3

    cd ../$SRC

    for arch in $ARCHS;
    do
        mkdir -p $DEST/$arch
    done

    for arch in $ARCHS;
    do  
        make clean
        IOSMV="-miphoneos-version-min=9.0 -fembed-bitcode"
        case $arch in
        arm*)  
            echo "Building speex for iPhoneOS $arch ****************"
            PATH=`xcodebuild -version -sdk iphoneos PlatformPath`"/Developer/usr/bin:$PATH" \
            SDK=`xcodebuild -version -sdk iphoneos Path` \
            CC="xcrun --sdk iphoneos clang -arch $arch $IOSMV --sysroot=$SDK -isystem $SDK/usr/include" \
            CXX="xcrun --sdk iphoneos clang++ -arch $arch $IOSMV --sysroot=$SDK -isystem $SDK/usr/include" \
            LDFLAGS="-Wl" \
            ./configure $CONFIG \
            --host=arm-apple-darwin \
            --prefix=$DEST/$arch \
            ;;
        *)
            echo "Building speex for iPhoneSimulator $arch *****************"
            PATH=`xcodebuild -version -sdk iphonesimulator PlatformPath`"/Developer/usr/bin:$PATH" \
            CC="xcrun --sdk iphonesimulator clang -arch $arch $IOSMV" \
            CXX="xcrun --sdk iphonesimulator clang++ -arch $arch $IOSMV" \
            ./configure \
            --host=$arch-apple-darwin \
            --prefix=$DEST/$arch \
            ;;
        esac
        make -j
        make install
    done

    make clean

    echo "Merge into universal binary."

    for i in $LIBS; 
    do
        input=""
        for arch in $ARCHS; 
        do
            input="$input $DEST/$arch/lib/$i"
        done
        lipo -create $input -output $DEST/$i 
    done 

    for arch in $ARCHS; 
    do
        rm -rf $DEST/$arch
    done

}


build libspeex.a speex-1.2rc2 ""
build libspeexdsp.a speexdsp-1.2rc3 --disable-neon
