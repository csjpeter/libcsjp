#!/bin/bash

JOBS=$(expr $(cat /proc/cpuinfo | grep processor | wc -l) + 1)

source /etc/lsb-release

function exec_in_dir ()
{
	pushd $1 > /dev/null || exit $?
	shift
	$@
	let RET=$?
	popd > /dev/null
	return ${RET}
}

if test "x$ANDROID_NDK_HOME" = "x" -o "x$ANDROID_SDK_HOME" = "x"; then
	echo "You will need to have ANDROID_NDK_HOME and ANDROID_SDK_HOME"
	echo "environment variables to contain the path to your favoured"
	echo "android ndk."
	exit 1
fi

function config()
{
	#--cflags=\"-flto -fwhole-program\"
	local API=$1
	local ARCH=$2
	local DIST=$3
	shift 3
	TCROOT=opt/${API}-${ARCH}
	export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:/${TCROOT}/lib
	export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/${TCROOT}/lib/pkgconfig
	./arm-linux-androideabi-crosstools-gen.sh ./${DIST}/crosstools || exit $?
	export PATH=${PWD}/${DIST}/crosstools:$PATH

	./dist-config.sh \
		$@ \
		-- \
		--target=arm-linux-androideabi \
		--prefix= \
		--tcroot=${TCROOT} \
		--host-prefix=opt/extras.ubuntu.com/csjp \
		--fpic \
		--non-gnu-source \
		--cflags=-Wno-psabi \
		--cflags=\\\"-D__STDC_INT64__ -DSTDC_INT64 -DSTDC_VERSION -DSTRICT_ANSI\\\" \
		--cflags=\\\"-I${ANDROID_NDK_HOME}/platforms/${API}/arch-${ARCH}/usr/include\\\" \
		--cflags=\\\"-I${ANDROID_NDK_HOME}/sources/cxx-stl/gnu-libstdc++/4.8/include\\\" \
		--cflags=\\\"-I${ANDROID_NDK_HOME}/sources/cxx-stl/gnu-libstdc++/4.8/libs/${ARM_CPU}/include\\\" \
		--ldflags=\\\"--sysroot ${ANDROID_NDK_HOME}/platforms/${API}/arch-${ARCH}\\\" \
		--ldflags=-rdynamic \
		--libs=\\\"${ANDROID_NDK_HOME}/sources/cxx-stl/gnu-libstdc++/4.8/libs/${ARM_CPU}/libsupc++.a\\\" \
		--libs=\\\"${ANDROID_NDK_HOME}/sources/cxx-stl/gnu-libstdc++/4.8/libs/${ARM_CPU}/libgnustl_static.a\\\" \
		--libs=\\\"-llog\\\" \
		--static || exit $?

	exec_in_dir build-for-${DIST} ./configure || exit $?
}

export API=android-14
ARCH=arm
export ARM_CPU=armeabi-v7a
export ANDROID_ARMV=android_armv7
export ARM_CPU=armeabi
export ANDROID_ARMV=android_armv5

CMD=$1

case "${CMD}" in
	(debian)
		shift
		test "x$1" = "x" || { API=$1 ; shift ; }
		test "x$1" = "x" || { ARCH=$1 ; shift ; }
		DIST=${DISTRIB_CODENAME}-x-${API}-${ARCH}

		config ${API} ${ARCH} ${DIST} \
			--packaging=android \
			--target=${API}-${ARCH} || exit $?
		exec_in_dir build-for-${DIST} debuild \
			--no-tgz-check \
			--preserve-envvar ANDROID_SDK_HOME \
			--preserve-envvar ANDROID_NDK_HOME \
			--preserve-envvar PATH \
			--preserve-envvar PKG_CONFIG_LIBDIR \
			--preserve-envvar PKG_CONFIG_PATH \
			-B $@ \
			--lintian-opts --no-lintian || exit $?
	;;
	(code)
		test "x$1" = "x" || { API=$1 ; shift ; }
		test "x$1" = "x" || { ARCH=$1 ; shift ; }
		DIST=${DISTRIB_CODENAME}-x-${API}-${ARCH}

		config ${API} ${ARCH} ${DIST} \
			--packaging=android \
			--target=${API}-${ARCH} || exit $?
		exec_in_dir build-for-${DIST} make -j1 $@ || exit $?
	;;
	(*)
		test "x$1" = "x" || { API=$1 ; shift ; }
		test "x$1" = "x" || { ARCH=$1 ; shift ; }
		DIST=${DISTRIB_CODENAME}-x-${API}-${ARCH}

		config ${API} ${ARCH} ${DIST} \
			--packaging=android \
			--target=${API}-${ARCH} || exit $?
		exec_in_dir build-for-${DIST} make -j${JOBS} $@ || exit $?
	;;
esac

# Android examples:
# http://developer.android.com/resources/browser.html?tag=sample

# Building a sample project:
# 1 cd to sample directory
# 2 /opt/csjp/android-ndk-r8/ndk-build
# 3/a /opt/csjp/android-sdk-linux/tools/android update project --target 1 --path ./
# 3/b /opt/csjp/android-sdk-linux/tools/android update project -p . -s
# 4 ant debug

# Might be interesting later:
# http://groomws.info/index.php?title=AndroidNativeBinaries
# http://developer.nvidia.com/tegra-start

# Gentoo cross development
# http://www.gentoo.org/proj/en/base/embedded/handbook/

# First, create android-9 toolchains:
# /opt/csjp/android-ndk-r8/build/tools/make-standalone-toolchain.sh --arch=x86 --platform=android-9 --install-dir=/opt/csjp/android-9-x86
# /opt/csjp/android-ndk-r8/build/tools/make-standalone-toolchain.sh --arch=arm --platform=android-9 --install-dir=/opt/csjp/android-9-arm
#

# Build process:
# http://developer.android.com/guide/developing/building/index.html#detailed-build
#
# Signing:
# http://developer.android.com/guide/publishing/app-signing.html
#
# Generate key to sign with
# keytool -genkey -keystore release.keystore -alias release -keyalg RSA -keysize 2048 -validity 10000
#
# Sign the package
# jarsigner -verbose -sigalg MD5withRSA -digestalg SHA1 -keystore release.keystore file.apk alias_name
#
# Verify a package
# jarsigner -verify -verbose -certs my_application.apk
#
# Align to 4 bytes boundaries so that android can mmap the file for faster further operations.
# zipalign -v 4 your_project_name-unaligned.apk your_project_name.apk

# May be for a native application
# http://developer.android.com/reference/android/app/NativeActivity.html

# Use this below command to start adb daemon:
# sudo adb start-server

# Use this below command to (re)install to the android device connected to you computer:
# adb -d install -r $(ANDROID_PACKAGES)





# http://stackoverflow.com/questions/5465941/what-is-the-log-api-to-call-from-an-android-jni-program
##include <android/log.h>
#__android_log_write(ANDROID_LOG_ERROR, "Tag", "Error here");//Or ANDROID_LOG_INFO, ... 
#LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog 

# http://mobilepearls.com/labs/native-android-api/
