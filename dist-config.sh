#!/bin/bash

source config

DISTRIB_CODENAME=$(source /etc/os-release; echo ${ID}"-"${VERSION_ID})

BUILD_DIST=${DISTRIB_CODENAME}
HOST_DIST=${DISTRIB_CODENAME}
TARGET_DIST=${DISTRIB_CODENAME}
CURRENT_DATE=$(LC_ALL=en_US.UTF-8 date +"%a, %d %b %Y %H:%M:%S %z")
CURRENT_YEAR=$(LC_ALL=en_US.UTF-8 date +"%Y")
VERSION_PACKAGING=${VERSION}
PACKAGING="debian"

while ! test "x$1" = "x"; do
	! test "x$1" = "x--" || {
		shift
		break
	}

	KEY=$(echo $1 | sed 's|=.*||')
	VALUE=$(echo $1 | sed 's|.*=||')
	case ${KEY} in
		(--build)
			! test "x${VALUE}" = "x" || {
				echo "Missing build architecture value."
				exit $?
			}
			BUILD_DIST="${VALUE}"
		;;
		(--host)
			! test "x${VALUE}" = "x" || {
				echo "Missing host architecture value."
				exit $?
			}
			HOST_DIST="${VALUE}"
		;;
		(--target)
			! test "x${VALUE}" = "x" || {
				echo "Missing target architecture value."
				exit $?
			}
			TARGET_DIST="${VALUE}"
		;;
		(--version-postfix)
			! test "x${VALUE}" = "x" || {
				echo "Missing version postfix value."
				exit $?
			}
			VERSION_PACKAGING=${VERSION}-${VALUE}
		;;
		(--packaging)
			PACKAGING=${VALUE}
		;;
		(*)
			echo "Invalid argument ($1) found on command line."
			exit 1
		;;
	esac
	shift
done

PRECONFIGURATION=$*

test "x${HOST_DIST}" = "x${TARGET_DIST}" && {
	PKGNAME=${PKGNAME_BASE}
	DIST_DIR=build-for-${HOST_DIST}
} || {
	PKGNAME=${TARGET_DIST}-${PKGNAME_BASE}
	DIST_DIR=build-for-${HOST_DIST}-x-${TARGET_DIST}
}

# param 1 : inflie
# stdout is the outfile
function dist-generator ()
{
cat << EOF > generator.${PACKAGING}.sh 
	cat \$1 | sed \\
		-e "s|@PRJNAME@|${PRJNAME}|g" \\
		-e "s|@PRJNAME_BASE@|${PRJNAME_BASE}|g" \\
		-e "s|@PRJDESC@|${PRJDESC}|g" \\
		-e "s|@PRJDESCRIPTION@|${PRJDESCRIPTION}|g" \\
		-e "s|@PKGNAME@|${PKGNAME}|g" \\
		-e "s|@PKGNAME_BASE@|${PKGNAME_BASE}|g" \\
		-e "s|@VERSION_MAJOR@|${VERSION_MAJOR}|g" \\
		-e "s|@VERSION_MINOR@|${VERSION_MINOR}|g" \\
		-e "s|@VERSION_PATCH@|${VERSION_PATCH}|g" \\
		-e "s|@VERSION_API@|${VERSION_API}|g" \\
		-e "s|@VERSION@|${VERSION}|g" \\
		-e "s|@VERSION_PACKAGING@|${VERSION_PACKAGING}|g" \\
		-e "s|@DEVELOPMENT_START_YEAR@|${DEVELOPMENT_START_YEAR}|g" \\
		-e "s|@AUTHOR@|${AUTHOR}|g" \\
		-e "s|@EMAIL@|${EMAIL}|g" \\
		-e "s|@TARGET_DIST@|${TARGET_DIST}|g" \\
		-e "s|@HOST_DIST@|${HOST_DIST}|g" \\
		-e "s|@DIST_DIR@|${DIST_DIR}|g" \\
		-e "s|@CURRENT_DATE@|${CURRENT_DATE}|g" \\
		-e "s|@CURRENT_YEAR@|${CURRENT_YEAR}|g" \\
		-e "s|@PRECONFIGURATION@|${PRECONFIGURATION}|g" \\
		|| exit \$?
EOF
	chmod u+x generator.${PACKAGING}.sh
}

dist-generator

#
# project directory
#

test -d ${DIST_DIR} || { mkdir -p ${DIST_DIR} || exit $? ; }

make -f source.mk HOST_DIST=${HOST_DIST} TARGET_DIST=${TARGET_DIST} \
	DIST_DIR=${DIST_DIR} PKGNAME=${PKGNAME} PACKAGING=${PACKAGING} \
	source || exit $?
make -f source.mk HOST_DIST=${HOST_DIST} TARGET_DIST=${TARGET_DIST} \
	DIST_DIR=${DIST_DIR} PKGNAME=${PKGNAME} PACKAGING=${PACKAGING} \
	${PACKAGING} || exit $?

cp -p config ${DIST_DIR}/config || exit $?
./generator.${PACKAGING}.sh configure.in > ${DIST_DIR}/configure || exit $?
chmod u+x ${DIST_DIR}/configure || exit $?
#./generator.${PACKAGING}.sh Makefile.in > ${DIST_DIR}/Makefile.in || exit $?
./generator.${PACKAGING}.sh license.in > ${DIST_DIR}/license || exit $?
./generator.${PACKAGING}.sh doxyfile.in > ${DIST_DIR}/doxyfile.in || exit $?
./generator.${PACKAGING}.sh pc.in > ${DIST_DIR}/pc.in || exit $?
./generator.${PACKAGING}.sh test.man.in > ${DIST_DIR}/test.man.in || exit $?

#
# debian packaging directory
#
test -d ${DIST_DIR}/debian || { mkdir -p ${DIST_DIR}/debian || exit $? ; }

./generator.${PACKAGING}.sh license.in > ${DIST_DIR}/debian/copyright || exit $?
./generator.${PACKAGING}.sh debian/changelog.in > ${DIST_DIR}/debian/changelog || exit $?
./generator.${PACKAGING}.sh debian/control.in > ${DIST_DIR}/debian/control || exit $?
test "x${HOST_DIST}" = "x${TARGET_DIST}" && {
	./generator.${PACKAGING}.sh debian/rules.native.in > ${DIST_DIR}/debian/rules.in || exit $?
} || {
	./generator.${PACKAGING}.sh debian/rules.cross.in > ${DIST_DIR}/debian/rules.in || exit $?
}
chmod u+x ${DIST_DIR}/debian/rules.in || exit $?
echo "5" > ${DIST_DIR}/debian/compat || exit $?

test -d ${DIST_DIR}/debian/source || { mkdir -p ${DIST_DIR}/debian/source || exit $? ; }
echo "1.0" > ${DIST_DIR}/debian/source/format || exit $?

./generator.${PACKAGING}.sh debian/pkg.install.in > ${DIST_DIR}/debian/${PKGNAME}.install.in || exit $?
./generator.${PACKAGING}.sh debian/dbg.install.in > ${DIST_DIR}/debian/${PKGNAME}-dbg.install.in || exit $?
./generator.${PACKAGING}.sh debian/test.install.in > ${DIST_DIR}/debian/${PKGNAME}-test.install.in || exit $?
./generator.${PACKAGING}.sh debian/dev.install.in > ${DIST_DIR}/debian/${PKGNAME}-dev.install.in || exit $?
./generator.${PACKAGING}.sh debian/doc.install.in > ${DIST_DIR}/debian/${PKGNAME}-doc.install.in || exit $?
./generator.${PACKAGING}.sh debian/postinst.in > ${DIST_DIR}/debian/${PKGNAME}.postinst.in || exit $?
./generator.${PACKAGING}.sh debian/postrm.in > ${DIST_DIR}/debian/${PKGNAME}.postrm.in || exit $?

