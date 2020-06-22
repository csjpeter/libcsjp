#!/bin/bash

JOBS=$(expr $(cat /proc/cpuinfo | grep processor | wc -l) + 1)
#JOBS=1

# ubuntu ditributions
#trusty=14.04
#raring=13.04
#quantal=12.10
#precise=12.04
#oneiric=11.10
#lucid=10.04
#hardy=8.04

# debian distributions
#wheezy=7.0
#squeeze=6.0
#lenny=5.0
#etch=4.0

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

function config ()
{
	local DIST=$1
	shift
	eval VERSION_POSTFIX=\$${DIST}-${DIST}
	./dist-config.sh \
		--build=${DIST} \
		--host=${DIST} \
		--target=${DIST} \
		--version-postfix=${VERSION_POSTFIX} \
		-- \
		--prefix=opt/extras.ubuntu.com/csjp \
		--ldflags=-rdynamic \
		--fpic \
		--gnu-source \
		$@ \
		--desktop \
		--multicultural \
		--system \
		|| exit $?

#		--cflags=\\\"-fno-candidate-functions\\\" \

	exec_in_dir build-for-${DIST} ./configure --debug || exit $?
	#exec_in_dir build-for-${DIST} ./configure || exit $?
}

function release ()
{
	local DIST=$1
	local ARCH=$2

	test "x${DIST}" = "x" && DIST=${DISTRIB_CODENAME}
	test "x${ARCH}" = "x" && ARCH=$(dpkg-architecture -qDEB_BUILD_ARCH)
	test "x${ARCH}" = "x$(dpkg-architecture -qDEB_BUILD_ARCH)" && {
		BASE_TGZ=$HOME/pbuilder/${DIST}-base.tgz
	} || {
		BASE_TGZ=$HOME/pbuilder/${DIST}-${ARCH}-base.tgz
	}
	test -e ${BASE_TGZ} || { pbuilder-dist ${DIST} ${ARCH} create || exit $? ; }

	config ${DIST} || exit $?
	exec_in_dir ${DIST} debuild --no-tgz-check -S -us -uc || exit $?
	pbuilder-dist ${DIST} ${ARCH} *.dsc --buildresult ${DIST}/ || exit $?
#		--othermirror "deb http://ppa.launchpad.net/csjpeter/ppa/ubuntu precise main"
#	sudo pbuilder --debuild --distribution ${DIST} --architecture ${ARCH} \
#		--basetgz ${BASE_TGZ} --buildresult ${DIST}/ --override-config *.dsc

#		--othermirror "deb http://ppa.launchpad.net/csjpeter/ppa/ubuntu precise main"
}

CMD=$1

case "${CMD}" in
	(release)
		shift
		release $1 $2
	;;
	(src-release)
		shift
		DIST=$1
		config ${DIST} || exit $?
		exec_in_dir ${DIST} debuild --no-tgz-check -S || exit $?
	;;
	(pump)
		shift
		config ${DISTRIB_CODENAME} || exit $?
		exec_in_dir build-for-${DISTRIB_CODENAME} pump make CXX=distcc \
			$@ || exit $?
	;;
	(debian)
		shift
		config ${DISTRIB_CODENAME} || exit $?
		exec_in_dir build-for-${DISTRIB_CODENAME} debuild \
			--no-tgz-check \
			--preserve-envvar MXE_HOME \
			--preserve-envvar PATH \
			--preserve-envvar PKG_CONFIG_LIBDIR \
			--preserve-envvar PKG_CONFIG_PATH \
			-B $@ \
			--lintian-opts --no-lintian || exit $?
	;;
	(check)
		shift
		config ${DISTRIB_CODENAME} || exit $?
		exec_in_dir build-for-${DISTRIB_CODENAME} make -j${JOBS} $@ \
			|| exit $?
		exec_in_dir build-for-${DISTRIB_CODENAME} make check $@ \
			|| exit $?
	;;
	(code)
		shift
		config ${DISTRIB_CODENAME} || exit $?
		exec_in_dir build-for-${DISTRIB_CODENAME} make -j1 $@ \
			|| exit $?
	;;
	(*)
		config ${DISTRIB_CODENAME} || exit $?
		exec_in_dir build-for-${DISTRIB_CODENAME} make -j${JOBS} $@ \
			|| exit $?
	;;
esac
