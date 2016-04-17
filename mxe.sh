#!/bin/bash

JOBS=$(expr $(cat /proc/cpuinfo | grep processor | wc -l) + 1)

DISTRIB_CODENAME=$(source /etc/os-release; echo ${ID}"-"${VERSION_ID})

function exec_in_dir ()
{
	pushd $1 > /dev/null || exit $?
	shift
	$@
	let RET=$?
	popd > /dev/null
	return ${RET}
}

#export LDFLAGS="-Wl,-subsystem,console"
#export LIBS="-lpthread"

#export PKG_CONFIG_SYSROOT_DIR=;
#export PKG_CONFIG_LIBDIR=/$(MXE_TC)/lib/pkgconfig;

function config()
{
	local DIST=$1
	shift
	TCROOT=opt/mxe
	export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:/${TCROOT}/lib
	export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/${TCROOT}/lib/pkgconfig
	./mxe-crosstools-gen.sh ./${DIST}/crosstools || exit $?
	export PATH=${PWD}/${DIST}/crosstools:$PATH

	./dist-config.sh \
		--target=mxe \
		--packaging=windows \
		-- \
		--target=i686-w64-mingw32.static \
		--prefix= \
		--tcroot=${TCROOT} \
		--host-prefix=opt/extras.ubuntu.com/csjp \
		--gnu-source \
		--static \
		--libs=\\\"-lgnurx -pthread\\\" \
		--ldflags="-Wl,-subsystem,console" \
		|| exit $?

	exec_in_dir build-for-${DIST} ./configure || exit $?
}

CMD=$1
DIST=${DISTRIB_CODENAME}-x-mxe

case "${CMD}" in
	(debian)
		shift
		config ${DIST} || exit $?
		exec_in_dir build-for-${DIST} debuild \
			--no-tgz-check \
			--preserve-envvar MXE_HOME \
			--preserve-envvar PATH \
			--preserve-envvar PKG_CONFIG_LIBDIR \
			--preserve-envvar PKG_CONFIG_PATH \
			-B $@ \
			--lintian-opts --no-lintian || exit $?
	;;
	(debian-nc)
		shift
		config ${DIST} || exit $?
		exec_in_dir build-for-${DIST} debuild --no-tgz-check -nc
		;;
	(debian-src)
		shift
		config ${DIST} || exit $?
		exec_in_dir build-for-${DIST} debuild --no-tgz-check -S
		;;
	(code)
		shift
		config ${DIST} || exit $?
		exec_in_dir build-for-${DIST} make -j1 $@ || exit $?
	;;
	(*)
		config ${DIST} || exit $?
		exec_in_dir build-for-${DIST} make -j${JOBS} $@ || exit $?
	;;
esac
