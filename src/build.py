import os, sys, logging

SOURCES = ["board.cpp", "engine.cpp", "cli.cpp", "util.cpp", "main.cpp"]
OBJECTS = [source.replace("cpp", "o") for source in SOURCES]
CXXFLAGS = ["-std=c++20", "-O3", "-march=native", "-mtune=native", "-Wno-non-c-typedef-for-linkage"]
LDFLAGS = []
CXX = "clang++"
LD = "clang++"
TARGET = "../iskelt-corax-v0.4.1"

class CompileError (Exception):
	pass

def determinePlatform ():
	if os.name == "nt":
		global TARGET

		TARGET += ".exe"
		CXXFLAGS.append("--target=x86_64-pc-windows-msvc")
		LDFLAGS.append("--target=x86_64-pc-windows-msvc")

def compileSources (sources: list[str]):
	for i in range(len(SOURCES)):
		logging.info(f"Compiling {SOURCES[i]}")

		if os.system(f"{CXX} {" ".join(CXXFLAGS)} -c -o {OBJECTS[i]} {SOURCES[i]}"):
			raise CompileError(f"Failed to compile source: {SOURCES[i]}")
		
		logging.info(f"Compiled {SOURCES[i]}")

def linkObjects (objects: list[str]):
	logging.info("Linking")

	if os.system(f"{LD} {" ".join(LDFLAGS)} -o {TARGET} {" ".join(OBJECTS)}"):
		raise CompileError("Failed to link")
	
	logging.info("Linked")

def build ():
	determinePlatform()
	compileSources(SOURCES)
	linkObjects(OBJECTS)
	logging.info("Done")

def clean ():
	for object in OBJECTS:
		try:
			logging.info(f"Removing {object}")
			os.remove(object)
		except:
			pass

def main ():
	logging.basicConfig(level=logging.INFO)

	if len(sys.argv) == 2:
		if sys.argv[1] == "build":
			try:
				build()
			except CompileError as error:
				logging.error(str(error))
		elif sys.argv[1] == "clean":
			clean()
		else:
			logging.error(f"Invalid command: {sys.argv[1]}")
	elif len(sys.argv) == 1:
		try:
			build()
		except CompileError as error:
			logging.error(str(error))
	else:
		logging.error(f"Invalid usage")

if __name__ == "__main__":
	main()