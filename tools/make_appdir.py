import sys
import os
import urllib.request
import subprocess
import shutil
import re

def printCmdAndExit():
	print(
	    "python make-appdir.py <interface, server, ice-server> <cmake build dir>"
	)
	sys.exit(1)

def resolve(path1, path2="."):
	return os.path.abspath(os.path.join(path1, path2))

if len(sys.argv) < 3:
	printCmdAndExit()

program = sys.argv[1]
if program != "interface" and program != "server" and program != "ice-server":
	print("Invalid program")
	printCmdAndExit()

build_dir = resolve(sys.argv[2])
if os.path.isdir(build_dir) == False:
	print("Invalid cmake build dir")
	printCmdAndExit()

def run(cwd, command, env=os.environ.copy()):
	print(command)
	process = subprocess.Popen(
	    command,
	    stdout=sys.stdout,
	    stderr=sys.stderr,
	    cwd=cwd,
	    env=env,
	    shell=True
	)
	process.wait()

	if (0 != process.returncode):
		raise RuntimeError('Call to "{}" failed.\n'.format(command))

# find qt path

with open(resolve(build_dir, "qt.cmake"), "r") as file:
	qt_cmake = file.read()
	results = re.search(
	    'QT_CMAKE_PREFIX_PATH "([\s\S]*?)"', qt_cmake, re.IGNORECASE
	)
	if results == None:
		print("Qt path not found!")
		sys.exit(1)
	qt_path = resolve(results.group(1), "../..")

print("-- Found Qt: " + qt_path)

# download linuxdeployqt

linuxdeployqt_compile_from_source = True

if linuxdeployqt_compile_from_source:
	# debian
	# apt-get install -y git unzip cmake build-essential libdouble-conversion-dev libpcre2-dev libglib2.0-dev

	linuxdeployqt_url = "https://github.com/probonopd/linuxdeployqt"
	linuxdeployqt_dir = resolve(build_dir, "linuxdeployqt")
	linuxdeployqt_build_dir = resolve(linuxdeployqt_dir, "build")
	linuxdeployqt = resolve(
	    linuxdeployqt_build_dir, "tools/linuxdeployqt/linuxdeployqt"
	)

	if not os.path.isdir(linuxdeployqt_dir):
		print("-- Cloning linuxdeployqt...")
		run(
		    build_dir,
		    "git clone " + linuxdeployqt_url + " " + linuxdeployqt_dir
		)

	if not os.path.isfile(linuxdeployqt):
		print("-- Compiling linuxdeployqt...")
		run(linuxdeployqt_dir, "rm -rf build")
		run(linuxdeployqt_dir, "mkdir build")
		run(
		    linuxdeployqt_build_dir,
		    "cmake .. -DCMAKE_PREFIX_PATH=\"" + qt_path + "\""
		)
		run(linuxdeployqt_build_dir, "make")

else:
	# precompiled but there's no arm64
	sys.exit(1)

	# linuxdeployqt_url = "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
	# linuxdeployqt_appimage = resolve(build_dir, "linuxdeployqt.AppImage")
	# linuxdeployqt_appdir = resolve(build_dir, "linuxdeployqt")
	# linuxdeployqt = resolve(linuxdeployqt_appdir, "AppRun")

	# if not os.path.isfile(linuxdeployqt_appimage):
	# 	print("-- Downloading linuxdeployqt...")
	# 	urllib.request.urlretrieve(linuxdeployqt_url, linuxdeployqt_appimage)

	# if not os.path.isdir(linuxdeployqt_appdir):
	# 	print("-- Extracting linuxdeployqt...")
	# 	run(build_dir, "chmod +x linuxdeployqt.AppImage")
	# 	run(build_dir, "./linuxdeployqt.AppImage --appimage-extract")
	# 	run(build_dir, "mv squashfs-root " + linuxdeployqt_appdir)

# verify linuxdeployqt

if not os.path.isfile(linuxdeployqt):
	print("-- " + linuxdeployqt + " not found!")
	sys.exit(1)

print("-- Installed linuxdeployqt")

# make appdir

if program == "interface":

	# find all plugins

	interface_dir = resolve(build_dir, "interface")

	plugins = os.listdir(resolve(interface_dir, "plugins"))
	print("-- Found plugins: " + ", ".join(plugins))

	# recreate appdir

	print("-- Preparing interface.AppDir...")

	appdir = resolve(interface_dir, "interface.AppDir")

	if os.path.isdir(appdir):
		shutil.rmtree(appdir)

	run(interface_dir, "mkdir -p interface.AppDir/usr/bin")
	run(interface_dir, "cp interface interface.AppDir/usr/bin")
	run(interface_dir, "cp resources.rcc interface.AppDir/usr/bin")
	run(interface_dir, "cp -r jsdoc interface.AppDir/usr/bin")
	run(interface_dir, "cp -r plugins interface.AppDir/usr/bin")
	run(interface_dir, "cp -r resources interface.AppDir/usr/bin")
	run(interface_dir, "cp -r scripts interface.AppDir/usr/bin")

	# libgl is a bad name and is ignored by linuxdeployqt
	run(interface_dir, "mkdir -p interface.AppDir/usr/lib")
	run(interface_dir, "cp ../libraries/gl/libgl.so interface.AppDir/usr/lib")

	print("-- Creating interface.AppDir")

	deploy_args = [
	    linuxdeployqt,
	    "interface.AppDir/usr/bin/interface",
	    "-unsupported-allow-new-glibc",
	    "-qmake=" + qt_path + "/bin/qmake",
	    "-qmldir=" + qt_path + "/qml",
	    "-extra-plugins=webview,platformthemes/libqgtk3.so",
	    "-exclude-libs=libnss3.so,libnssutil3.so",
	    "-no-translations",
	    "-no-copy-copyright-files",
	    "-bundle-non-qt-libs",
	]

	for plugin in plugins:
		deploy_args.append(
		    "-executable=interface.AppDir/usr/bin/plugins/" + plugin
		),
	deploy_args.append("-executable=interface.AppDir/usr/lib/libgl.so")

	# run linuxdeployqt!
	env = os.environ.copy()
	env["LD_LIBRARY_PATH"] = build_dir
	run(interface_dir, " ".join(deploy_args), env)

	run(appdir, "ln -s usr/bin/interface interface")

	print("-- Done!")

elif program == "server":

	# recreate appdir

	print("-- Preparing server.AppDir...")

	appdir = resolve(build_dir, "server.AppDir")

	if os.path.isdir(appdir):
		shutil.rmtree(appdir)

	run(build_dir, "mkdir -p server.AppDir/usr/bin")
	run(build_dir, "cp domain-server/domain-server server.AppDir/usr/bin")
	run(
	    build_dir,
	    "cp assignment-client/assignment-client server.AppDir/usr/bin"
	)
	run(build_dir, "cp tools/oven/oven server.AppDir/usr/bin")
	run(build_dir, "cp -r domain-server/resources server.AppDir/usr/bin")

	# libgl is a bad name and is ignored by linuxdeployqt
	run(build_dir, "mkdir -p server.AppDir/usr/lib")
	run(build_dir, "cp libraries/gl/libgl.so server.AppDir/usr/lib")

	print("-- Creating server.AppDir")

	deploy_args = [
	    linuxdeployqt,
	    "server.AppDir/usr/bin/domain-server",
	    "-unsupported-allow-new-glibc",
	    "-executable=server.AppDir/usr/bin/assignment-client",
	    "-executable=server.AppDir/usr/bin/oven",
	    "-executable=server.AppDir/usr/lib/libgl.so",
	    "-unsupported-allow-new-glibc",
	    "-qmake=" + qt_path + "/bin/qmake",
	    "-qmldir=" + qt_path + "/qml",
	    "-no-translations",
	    "-no-copy-copyright-files",
	    "-bundle-non-qt-libs",
	]

	# run linuxdeployqt!

	run(build_dir, " ".join(deploy_args))

	run(appdir, "ln -s usr/bin/domain-server domain-server")
	run(appdir, "ln -s usr/bin/assignment-client assignment-client")
	run(appdir, "ln -s usr/bin/oven oven")

	print("-- Done!")

elif program == "ice-server":

	# recreate appdir

	print("-- Preparing ice-server.AppDir...")

	appdir = resolve(build_dir, "ice-server.AppDir")

	if os.path.isdir(appdir):
		shutil.rmtree(appdir)

	run(build_dir, "mkdir -p ice-server.AppDir/usr/bin")
	run(build_dir, "cp ice-server/ice-server ice-server.AppDir/usr/bin")

	# libgl is a bad name and is ignored by linuxdeployqt
	run(build_dir, "mkdir -p ice-server.AppDir/usr/lib")
	run(build_dir, "cp libraries/gl/libgl.so ice-server.AppDir/usr/lib")

	print("-- Creating ice-server.AppDir")

	deploy_args = [
	    linuxdeployqt,
	    "ice-server.AppDir/usr/bin/ice-server",
	    "-executable=ice-server.AppDir/usr/lib/libgl.so",
	    "-unsupported-allow-new-glibc",
	    "-qmake=" + qt_path + "/bin/qmake",
	    "-qmldir=" + qt_path + "/qml",
	    "-no-translations",
	    "-no-copy-copyright-files",
	    "-bundle-non-qt-libs",
	]

	# run linuxdeployqt!

	run(build_dir, " ".join(deploy_args))

	run(appdir, "ln -s usr/bin/ice-server ice-server")

	print("-- Done!")
