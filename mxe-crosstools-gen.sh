#!/bin/bash

if test "x$1" = "x"; then
	CROSSTOOLS_BIN=~/bin/crosstools
else
	CROSSTOOLS_BIN=$1
fi

if ! test -d ${CROSSTOOLS_BIN}; then
	mkdir -p ${CROSSTOOLS_BIN} || exit $?
fi

if test "x${MXE_HOME}" = "x"; then
	echo "You will need to have MXE_HOME environment variable to contain the path to your favoured MXE."
	exit 1
fi

test -d ${CROSSTOOLS_BIN} || mkdir ${CROSSTOOLS_BIN}

for f in $(ls -1 ${MXE_HOME}/usr/bin/i686-w64-mingw32.static-*); do
	b=$(basename $f)
	test -e ${CROSSTOOLS_BIN}/$b || {
		cat <<EOF > ${CROSSTOOLS_BIN}/$b
#!/bin/bash
\${MXE_HOME}/usr/bin/$b \$@
exit \$?
EOF
		chmod u+x ${CROSSTOOLS_BIN}/$b
	}
done

test -L ${CROSSTOOLS_BIN}/i686-w64-mingw32.static-pkg-config && rm ${CROSSTOOLS_BIN}/i686-w64-mingw32.static-pkg-config
cat <<EOF > ${CROSSTOOLS_BIN}/i686-w64-mingw32.static-pkg-config
#!/bin/bash

export PKG_CONFIG_LIBDIR=\${PKG_CONFIG_LIBDIR}:\${MXE_HOME}/usr/i686-w64-mingw32.static/lib:\${MXE_HOME}/usr/i686-w64-mingw32.static/qt/lib
export PKG_CONFIG_PATH=\${PKG_CONFIG_PATH}:\${MXE_HOME}/usr/i686-w64-mingw32.static/lib/pkgconfig:\${MXE_HOME}/usr/i686-w64-mingw32.static/qt/lib/pkgconfig
pkg-config \$@
exit \$?
EOF
chmod u+x ${CROSSTOOLS_BIN}/i686-w64-mingw32.static-pkg-config

