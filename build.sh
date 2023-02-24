#!/bin/sh

# Defaults
enable_ut="yes"         # unit tests enabled
enable_pt="no"          # performance tests enabled
print_ut_log="no"       # UT log is not printed if all UTs pass
build_type="Debug"      # build type
build_python_tgz="no"   # build Python source package
build_python_pkg="no"   # build Python package


usage() {
    cat <<HERE
Usage: $0 [OPTIONS] [-- buildchain parameters]

OPTIONS:
    -h or --help                Print this usage help and exit
    -b or --build-dir <dir>     Use <dir> as build directory
                                (<PROJECT_DIR>/build by default)
    -t or --build-type <type>   Build type (Debug, Release, RelWithDebInfo
                                and MinSizeRel, default: $build_type)
    -d or --build-debug         Same as -t Debug
    -r or --build-release       Same as -t Release
    -c or --clean               Clean build directory before build starts
                                (BEWARE, removed the whole build directory)
    -u or --enable-ut           Enable unit tests run (default: $enable_ut)
    -U or --disable-ut          Disable unit tests run
    -s or --enable-pt           Enable performance tests run (default: $enable_pt)
    -S or --disable-pt          Disable performance tests run
    -D or --devel               Run in development mode (don't run UTs,
                                generate debug symbols,
                                force colours and use less pager)
    -C or --cxx-flags <flags>   Add custom compiler flags (note that
                                the build script also honours CXXFLAGS
                                environment variable content)
    -l or --print-ut-log        Print UT log even if all UTs pass
    -g or --build-python-tgz    Build Python source package
    -G or --no-python-tgz       Don't build Python source package
    -p or --build-python-pkg    Build Python v3 package
    -P or --no-python-pkg       Don't build Python package

Buildchain parameters are forwarded to underlying buildchain.
Use it e.g. to pass parameters such like -j to make...

EXAMPLE (build in custom directory and set parallel build limit):
$ $0 --build-dir /var/tmp/my_build -- -j4

NOTE: Long options don't work on Mac OS X (with BSD getopt). Use short versions.

HERE
}


echo_colour() {
    colour="$1"; shift
    cmake -E cmake_echo_color "--$colour" "$*"
}


set -e

# Parse options
build_dir=""
clean="no"
pager=""
devel_mode="no"
cxx_flags="${CXXFLAGS}"
platform=$(uname -s)

if test "$platform" = "Linux"; then
    args=$(
        getopt \
            -n "$0" \
            -o hb:t:drcuUsSDC:lgGpP \
            --long help,build-dir:,build-type:,build-debug,build-release,clean,enable-ut,disable-ut,enable-pt,disable-pt,devel,cxx-flags:,print-ut-log,build-python-tgx,no-python-tgz,build-python-pkg,no-python-pkg \
            -- "$@" \
        || (echo >&2; usage >&2; exit 1)
    )
elif test "$platform" = "Darwin"; then
    args=$(getopt hb:t:drcuUsSDC:lgGpP "$@" || (echo >&2; usage >&2; exit 1))
fi

eval set -- "$args"
while true; do
    case "$1" in
        -h|--help)
            usage; exit 0
            ;;

        -b|--build-dir)
            build_dir="$2"; shift; shift
            ;;

        -t|--build-type)
            build_type="$2"; shift; shift
            ;;

        -d|--build-debug)
            build_type="Debug"; shift
            ;;

        -r|--build-release)
            build_type="Release"; shift
            ;;

        -c|--clean)
            clean="yes"; shift
            ;;

        -u|--enable-ut)
            enable_ut="yes"; shift
            ;;

        -U|--disable-ut)
            enable_ut="no"; shift
            ;;

        -s|--enable-pt)
            enable_pt="yes"; shift
            ;;

        -S|--disable-pt)
            enable_pt="no"; shift
            ;;

        -D|--devel)
            devel_mode="yes"
            enable_ut="no"
            cxx_flags="$cxx_flags -g -fdiagnostics-color=always"
            pager="2>&1 | less -R"
            shift
            ;;

        -C|--cxx-flags)
            cxx_flags="$cxx_flags $2"; shift; shift
            ;;

        -l|--print-ut-log)
            print_ut_log="yes"; shift
            ;;

        -g|--build-python-tgz)
            build_python_tgz="yes"; shift
            ;;

        -G|--no-python-tgz)
            build_python_tgz="no"; shift
            ;;

        -p|--build-python-pkg)
            build_python_pkg="yes"; shift
            ;;

        -P|--no-python-pkg)
            build_python_pkg="no"; shift
            ;;

        --) shift; break
            ;;

        *)  echo "INTERNAL ERROR: unresolved legal option '$1'" >&2
            exit 64
            ;;
    esac
done


# Resolve directories
project_dir=$(realpath "$0" | xargs dirname)
source_dir="$project_dir/src"

if test -n "$build_dir"; then
    build_dir=$(realpath "$build_dir")
else
    build_dir="$project_dir/build"
fi


# Report
echo_colour yellow "Build platform: $platform"
echo_colour yellow "Source directory: $source_dir"
echo_colour yellow "Build directory: $build_dir"
echo_colour yellow "Build type: $build_type"
echo_colour yellow "Clean build: $clean"
echo_colour yellow "Buildchain parameters: $@"
echo_colour yellow "Extra compiler flags: $cxx_flags"
echo_colour yellow "Unit tests enabled: $enable_ut"
echo_colour yellow "Performance tests enabled: $enable_pt"
echo_colour yellow "UT log print on success: $print_ut_log"
echo_colour yellow "Python source package build: $build_python_tgz"
echo_colour yellow "Python package build: $build_python_pkg"
echo


# Clean build
test "$clean" = "yes" && rm -rf "$build_dir"

# Build
mkdir -p "$build_dir"
cd "$build_dir"
CXXFLAGS="$cxx_flags" cmake -DCMAKE_BUILD_TYPE="$build_type" "$source_dir"
eval cmake --build . -- "$@" $pager

test "$devel_mode" = "yes" && exit 0  # skip the rest in devel mode


# Unit testing
if test "$enable_ut" = "yes"; then
    echo
    make test || ut_failed="yes"
    test "$ut_failed" = "yes" -o "$print_ut_log" = "yes" && \
        cat "Testing/Temporary/LastTest.log"
    test "$ut_failed" = "yes" && exit 1
fi

if which pytest >/dev/null; then
    echo; echo_colour cyan "Running Python wrapper tests..."
    cd "$project_dir"
    PYTHONPATH="$PYTHONPATH:$project_dir/src" \
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$build_dir/libpysdcxx" \
    pytest --verbose --color=yes src/unit_test
else
    echo; echo_colour red "WARNING: Skipping Python wrapper tests, pytest not found"
fi


# Performance testing
if test "$enable_pt" = "yes"; then
    echo; echo_colour cyan "Running performance tests..."
    cd "$project_dir"

    echo "Bigram multiset operations performance:"
    PYTHONPATH="$PYTHONPATH:$project_dir/src" \
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$build_dir/libpysdcxx" \
    python src/perf_test/op_time.py -t src/perf_test/test.txt

    echo "Sequence matching performance:"
    PYTHONPATH="$PYTHONPATH:$project_dir/src" \
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$build_dir/libpysdcxx" \
    python src/perf_test/matching.py -T0.8 \
        -t src/perf_test/short.txt -s src/perf_test/sequences.txt
fi


# Python source tarball build
if test "$build_python_tgz" = "yes"; then
    echo; echo_colour cyan "Building Python source package..."
    cd "$project_dir"
    ./adoc2md.sh README.adoc README.md
    python ./setup.py sdist -d .
fi

# Python package build
if test "$build_python_pkg" = "yes"; then
    echo; echo_colour cyan "Building Python package..."
    cd "$project_dir"
    pip wheel .
fi


# All done
echo_colour green "
------------------
Built SUCCESSFULLY
------------------
"
