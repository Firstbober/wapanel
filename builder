#!/bin/python

from abc import ABC, abstractmethod
import os
import subprocess
import argparse
import json
import shutil
import glob
import signal

# Modify this if you need
# defaults will be enough for most people
BUILD_DIRECTORY = "build"
EXECUTABLE_OUTPUT = "output"


# builder.py
# Copyright 2020 Firstbober
#
# Permission to use, copy, modify,
# and/or distribute this software for any purpose with or without fee is hereby granted,
# provided that the above copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
# THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

__version__ = "1.6.0"

class BuildBackend(ABC):
    TriggerFile = ""

    def build(self, opts):
        pass

    def run(self, opts, args):
        pass

    def valgrind(self, opts, args):
        pass

    def test(self, opts):
        pass

    def clean(self):
        pass


class Meson(BuildBackend):
    TriggerFile = "meson.build"

    def build(self, opts):
        build_type = "plain"

        if len(opts) > 0:
            if opts[0] == "build_release":
                build_type = "release"
            elif opts[0] == "build_debug":
                build_type = "debug"
            elif opts[0] == "build_plain":
                build_type = "plain"

            if opts[0] != "build_force" and len(opts) > 1:
                if opts[1] == "build_force":
                    self.clean()
            elif opts[0] == "build_force":
                self.clean()

        meson_out = os.popen("meson introspect " +
                             BUILD_DIRECTORY + " --buildoptions").read()

        if meson_out.startswith("Cu"):
            os.system("meson setup " + BUILD_DIRECTORY + " . --buildtype " +
                      build_type + " --prefix " + os.getcwd() + "/" + EXECUTABLE_OUTPUT + "/" + build_type)

        if meson_out.startswith("["):
            meson_build_options = json.loads(meson_out)

            for option in meson_build_options:
                if option["name"] == "buildtype":
                    if option["value"] != build_type:
                        os.system("meson configure " + BUILD_DIRECTORY + " --buildtype " +
                                  build_type + " --prefix " + os.getcwd() + "/" + EXECUTABLE_OUTPUT + "/" + build_type)

        CURRENT_PROC = subprocess.Popen(["meson", "compile", "-C", BUILD_DIRECTORY])
        out, err = CURRENT_PROC.communicate()

        if CURRENT_PROC.returncode != 0:
            return False

        os.system("meson install --quiet -C " + BUILD_DIRECTORY)

        return True

    def run(self, opts, args):
        meson_out = os.popen("meson introspect " +
                             BUILD_DIRECTORY + " --targets").read()

        requested_binary_type = ""
        no_call = False

        if len(opts) > 0:
            if opts[0] == "build_release":
                requested_binary_type = "release"
            elif opts[0] == "build_debug":
                requested_binary_type = "debug"
            elif opts[0] == "build_plain":
                requested_binary_type = "plain"

            for opt in opts:
                if opt == "__no_call":
                    no_call = True

        if meson_out.startswith("["):
            meson_targets = json.loads(meson_out)

            for target in meson_targets:
                if target["type"] == "executable":
                    if requested_binary_type == "":
                        exec_path = [target["install_filename"][0]]

                        if len(args) > 0:
                            exec_path += args

                        if no_call:
                            return [exec_path, '/'.join(target["install_filename"][0].split("/")[:-1])]
                        else:
                            os.chdir('/'.join(target["install_filename"][0].split("/")[:-1]))
                            os.system(" ".join(exec_path) + ">&2")

                    else:
                        exec_path = [os.getcwd() + "/" + EXECUTABLE_OUTPUT + "/" +
                                     requested_binary_type + "/bin/" +
                                     target["install_filename"][0].split("/")[-1]]

                        if len(args) > 0:
                            exec_path += args

                        if no_call:
                            return [exec_path, os.getcwd() + "/" + EXECUTABLE_OUTPUT + "/" +
                                    requested_binary_type + "/bin/"]
                        else:
                            os.chdir(os.getcwd() + "/" + EXECUTABLE_OUTPUT + "/" + requested_binary_type + "/bin/")
                            os.system(" ".join(exec_path) + ">&2")
        else:
            print("Project is not compiled yet!")

        pass

    def valgrind(self, opts, args):
        opts.append("__no_call")
        exec_info = self.run(opts, args)

        os.chdir(exec_info[1])
        os.system("valgrind ./" + exec_info[0][0].split("/")[-1] + ">&2")
        pass

    def test(self, opts):
        os.system("meson test -C " + BUILD_DIRECTORY)
        pass

    def clean(self):
        if os.path.exists(BUILD_DIRECTORY) and os.path.isdir(BUILD_DIRECTORY):
            shutil.rmtree(BUILD_DIRECTORY)

        print("Removed build directory")


_build_backend_registry = [
    Meson()
]


def find_build_backend():
    for entry in os.scandir("."):
        for build_backend in _build_backend_registry:
            if entry.name == build_backend.TriggerFile:
                return build_backend
    return False


def format_files(directory, extensions):
    files = []

    for extension in extensions:
        for name in glob.glob(directory + "/**/*." + extension, recursive=True):
            files.append(name)

    os.system("clang-format -i " + ' '.join(files))

    pass

def keyboard_interrupt_handler(sig, frame):
    exit(0)

RETURN_CODES = [
    "0",
    "1 (SIGHUP) [Hangup detected on controlling terminal or death of controlling process]",
    "2 (SIGINT) [Interrupt from keyboard]",
    "3 (SIGQUIT)[Quit from keyboard]",
    "4 (SIGILL) [Illegal Instruction]",
    "5 (SIGTRAP)[]",
    "6 (SIGABRT)[Abort signal from abort(3)]",
    "7 (SIGBUS) []",
    "8 (SIGFPE) [Floating point exception]",
    "9 (SIGKILL)[Kill signal]",
    "10 (SIGUSR1) []",
    "11 (SIGEGV)  [Invalid memory reference]",
    "12 (SIGUSR2) []",
    "13 (SIGPIPE) [Broken pipe: write to pipe with no readers]",
    "14 (SIGALRM) [Timer signal from alarm(2)]",
    "15 (SIGTERM) [Termination signal]",
]

if __name__ == "__main__":
    signal.signal(signal.SIGINT, keyboard_interrupt_handler)

    build_backend = find_build_backend()

    if build_backend == False:
        print("Cannot find supported build backend!")

    parser = argparse.ArgumentParser(
        description="builder, version " + __version__)

    parser.add_argument(
        "-b", "--build", help="build entire source code", action="store_true")
    parser.add_argument(
        "-r", "--run", help="run compiled executable", action="store_true")
    parser.add_argument(
        "-t", "--test", help="execute project tests", action="store_true")
    parser.add_argument(
        "-vg", "--valgrind", help="run valgrind on project executable", action="store_true")

    parser.add_argument(
        "-c", "--clean", help="clean project build files and generated documentation", action="store_true")

    parser.add_argument(
        "-f", "--format", help="format all project source code using 'clang-format'", action="store_true")

    parser.add_argument(
        "-br", "--buildrun", help="build and run", action="store_true")
    parser.add_argument(
        "-bv", "--buildvalgrind", help="build project and run valgrind on generated executable", action="store_true")

    parser.add_argument(
        "--release", help="release switch for building", action="store_true")
    parser.add_argument(
        "--debug", help="debug switch for building", action="store_true")
    parser.add_argument(
        "--plain", help="plain switch for building", action="store_true")
    parser.add_argument(
        "--force", help="force rebuilding", action="store_true")

    parser.add_argument(
        "--args", help="specify args for executable to be passed", nargs=argparse.REMAINDER)

    parser.add_argument(
        "--directory", help="directory to format by clang-format")
    parser.add_argument(
        "--extensions", help="list of extensions to be formatted")

    parser.add_argument('-v', "--version",
                        action='version', version=__version__)

    parsed_opts = parser.parse_args()

    if (parsed_opts.build == False
        and parsed_opts.buildrun == False
        and parsed_opts.buildvalgrind == False
        and parsed_opts.clean == False
        and parsed_opts.format == False
        and parsed_opts.run == False
        and parsed_opts.test == False
            and parsed_opts.valgrind == False):

        print("Use --help to see available actions")
        exit(0)

    build_system_opts = []
    addional_args = []

    if parsed_opts.release == True:
        build_system_opts.append("build_release")
    elif parsed_opts.debug == True:
        build_system_opts.append("build_debug")
    elif parsed_opts.plain == True:
        build_system_opts.append("build_plain")

    if parsed_opts.force == True:
        build_system_opts.append("build_force")

    if parsed_opts.args != None:
        addional_args = parsed_opts.args

    if parsed_opts.build == True:
        build_backend.build(build_system_opts)
    elif parsed_opts.run == True:
        build_backend.run(build_system_opts, addional_args)
    elif parsed_opts.test == True:
        build_backend.test(build_system_opts)
    elif parsed_opts.valgrind == True:
        build_backend.valgrind(build_system_opts, addional_args)
    elif parsed_opts.clean == True:
        build_backend.clean()
    elif parsed_opts.format == True:
        directory = "."
        extensions = []

        if parsed_opts.directory:
            directory = parsed_opts.directory

        if parsed_opts.extensions:
            extensions = parsed_opts.extensions.split(",")
        else:
            extensions = ["cc", "hh", "cpp", "hpp", "c", "h"]

        format_files(directory, extensions)

    elif parsed_opts.buildrun == True:
        if build_backend.build(build_system_opts):
            build_backend.run(build_system_opts, addional_args)
    elif parsed_opts.buildvalgrind == True:
        if build_backend.build(build_system_opts):
            build_backend.valgrind(build_system_opts, addional_args)