#/!bin/sh

die() {
    echo " *** ERROR: " $*
    exit 1
}

python --version || die

[ $BUILD_TARGET != "test-py-2.7" ] || {
    ./buildconf || die
    ./configure || die
    make check || die
}

[ $BUILD_TARGET != "test-py-3.6" ] || {
    ./buildconf || die
    ./configure || die
    make check || die
}
