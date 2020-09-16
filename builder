#!/bin/bash

BUILD_FILES_DIRECTORY="$(pwd)"/build
DOCS_DIRECTORY="$(pwd)"/docs
RUNTIME_OUTPUT_DIRECTORY="$(pwd)"/bin

export PKG_CONFIG_PATH=$(pwd)/bin/lib/pkgconfig

function build {
	if [ "$1" == '-f' ] || [ "$1" == "--force" ]; then
		del_build && cd ../ ;
	fi

    BUILD_MODE="plain"
	BUILD_TESTS=false

	if [ "$1" == '-r' ] || [ "$1" == "--release" ]; then
		BUILD_MODE="release"
		meson setup "$BUILD_FILES_DIRECTORY" --wipe;
		meson "$BUILD_FILES_DIRECTORY" --buildtype=$BUILD_MODE;
	elif [ "$1" == "-d" ] || [ "$1" == "--debug" ]; then
		BUILD_MODE="debug"
		meson setup "$BUILD_FILES_DIRECTORY" --wipe;
		meson "$BUILD_FILES_DIRECTORY" --buildtype=$BUILD_MODE;
	fi

	if [ "$1" == '-t' ] || [ "$1" == "--tests" ]; then
		BUILD_TESTS=true
	fi

	if [ ! -f "$BUILD_FILES_DIRECTORY"/build.ninja ]; then
		meson ""$BUILD_FILES_DIRECTORY"" --buildtype=$BUILD_MODE;
		meson configure "$BUILD_FILES_DIRECTORY" -Dprefix=$RUNTIME_OUTPUT_DIRECTORY;
	fi

	cd "$BUILD_FILES_DIRECTORY" && ninja && ninja install ;
}

function del_build { cd "$BUILD_FILES_DIRECTORY" && rm -rf * ; }

function format_files {
	hash clang-format 2>/dev/null || { echo >&2 "'clang-format' is required but not found"; exit 1; }
	clang-format -i $(find ./src -type f \( -name '*.hpp' -o -name '*.cpp' \));
}

function run () {
	if [ -z $1 ]; then
		exef=$(meson introspect "$BUILD_FILES_DIRECTORY" --targets | grep -Po '"name":.*?[^\\]",' | awk -F':' '{print $2}' | cut -d "\"" -f 2 | head -n 3 | tail -1);
	else exef=$1; fi
    cd "$RUNTIME_OUTPUT_DIRECTORY/bin" && ./"$exef" "${@:2}"
}

function run_tests { cd "$BUILD_FILES_DIRECTORY" && ninja test; }

function run_valgrind () {
	if [ -z $1 ]; then
		exef=$(meson introspect "$BUILD_FILES_DIRECTORY" --targets | grep -Po '"name":.*?[^\\]",' | awk -F':' '{print $2}' | cut -d "\"" -f 2 | head -n 3 | tail -1);
	else exef=$1; fi
	echo $exef
    cd "$RUNTIME_OUTPUT_DIRECTORY/bin" && valgrind --leak-check=full -v ./"$exef" "${@:2}";
}

function show_help {
	printf "builder.sh version 1.5.0
Usage:
	%s [COMMAND] [FLAGS | FILE]

Commands:
    build          Build entire source code
    run            Run compiled executable
    test           Execute project tests
    valgrind       Run valgrind on project executable

    clean          Clean project build files and generated documentation
    fmt            Format all project source code using 'clang-format'

    br             Build and run
    brd            Create debug build and execute it
    brf            Force rebuild project
    brr            Create release build and execute it
    bv             Build project and run valgrind on generated executable

Flags:
    -r, --release             Release switch for build command
    -d, --debug               Debug switch for build command
" $0
}

case "$1" in
	"build")    build "$2" ;;
	"run")      run "${@:2}" ;;
	"valgrind") run_valgrind "${@:2}" ;;
	"test")     build "-t" && run_tests ;;

	"fmt")      format_files ;;
	"clean")    del_build ;;

	"br")       build && run "${@:2}" ;;
	"bv")       build && run_valgrind "${@:2}" ;;
	"brd")      build "-d" && run "${@:2}" ;;
	"brr")      build "-r" && run "${@:2}" ;;
	"brf")      build "-f" && run "${@:2}" ;;

	"help") show_help ;;

	*) echo "Invalid argument! Use help!" ;;
esac
