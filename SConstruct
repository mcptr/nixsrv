import sys
import os
from collections import OrderedDict

THIS_PLATFORM = os.uname()[0].lower()
PROJECT_NAME = "nix"
PROJECT_TARGET = "NIX"

AddOption("--compiler",
		  dest="compiler",
		  action="store",
		  choices=["clang++", "g++"],
		  default="clang++",
		  help="compiler to use")


AddOption("--enable-debug",
		  dest="debug_build",
		  action="store_true",
		  default=False,
		  help="debug build")

AddOption("--enable-profile",
		  dest="profile_build",
		  action="store_true",
		  default=False,
		  help="enable profiling")

AddOption("--modules",
		  dest="build_modules",
		  action="store_true",
		  default=False,
		  help="build modules")


def is_option_set(opt):
	v = ARGUMENTS.get(opt, "").lower()
	return (v not in ["", "no", "0"])


def extend_env(dest, src):
	entities = ["CXXFLAGS", "CPPPATH", "LIBS", "LIBPATH"]
	src = src if isinstance(src, list) else [src]
	for skel in src:
		for item in entities:
			if not dest.get(item):
				dest[item] = []
			items = skel.get(item)
			if items:
				dest[item].extend(items if isinstance(items, list) else [items])
	for item in entities:
		# using OrderedDict, as set reorders elements causing linking error...
		dest[item] = OrderedDict.fromkeys(dest[item]).keys()
	return dest

class Dirs(object):
	build = "./build"
	objects = "%s/objs" % build
	target = "%s/target" % build
	source = "./src"
	project_source = source #"%s/%s" % (source, PROJECT_NAME)
	install = ARGUMENTS.get("install_dir", "./bin")
	#exceptions = "%s/excpt" % project_source
	tests_source = "%s/test" % source
	tests_target = target


class Setup(object):
	base_cxxflags = Split(" ".join([
		"-std=c++11 -Wall -Wextra -pedantic -O2",
		"-pipe -pthread -finline-functions -Wreorder"
	]))
	base_libs = []
	base_libpath = ["/usr/lib", "/usr/local/lib"]
	base_linkflags = ["-pthread"]
	base_defines = []


	def __init__(self):
		self.setup_cpp_defines()
		if GetOption("debug_build"):
			self.base_cxxflags.append("-g")
		if GetOption("profile_build"):
			#self.base_cxxflags.append("--coverage")
			#self.base_linkflags.append("-profile")
			self.base_linkflags.append("-pg")
		if THIS_PLATFORM == "linux":
			self.base_libs.append("dl")


	def create_directories(self):
		for prop in Dirs.__dict__:
			d = Dirs.__dict__[prop]
			if not prop.startswith("_") and not callable(d):
				if not os.path.isdir(d):
					print("Creating directory: " + d)
					Mkdir(d)


	def setup_cpp_defines(self):
		sql_backend = "USE_SQL_BACKEND"
		opt2define = {
			"debug" : ["DEBUG_BUILD"],
			"mysql" : ["MYSQL_BACKEND", sql_backend],
			"postgresql" : ["PGSQL_BACKEND", sql_backend],
			"mongodb" : ["MONGODB_BACKEND"],
		}

		for opt in opt2define:
			if is_option_set(opt):
				for item in opt2define[opt]:
					self.base_defines.append(item)
# - class setup -


def resolve_include(path, **kwargs):
	fatal = kwargs.pop("fatal", True)
	ns = kwargs.pop("ns", "")
	# 2DO: read base include paths from compiler, system, or wherever
	# else to support arch libs, etc.
	__base = {
		"linux" : ["/usr/local", "/usr"],
		"freebsd" : ["/usr/local", "/usr"],
	}

	search_paths = __base.get(THIS_PLATFORM)
	for sp in search_paths:
		candidates = [os.path.join(sp, "include", path)]
		if ns:
			candidates.append("%s/include/%s/%s" % (sp, ns, path))
		for cand in candidates:
			if os.path.isdir(cand):
				return cand
	if fatal:
		raise Exception("Cannot find headers for: %s" % path)


def resolve_libpath(directory, fatal = True):
	# 2DO: read base include paths from compiler, system, or wherever
	# else to support arch libs, etc.
	# FIXME: add_option()s to scons, ubuntu is broken
	__base = {
		
		"linux" : [
			"/usr/lib64", "/usr/local/lib64",
			"/usr/lib", "/usr/local/lib",
		],
		"freebsd" : [
			"/usr/local/lib", "/usr/local/lib64",
			"/usr/lib", "/usr/lib64"
		],
	}

	custom_lib_paths = ["lib/external"]

	search_paths = __base[THIS_PLATFORM] + custom_lib_paths
	for sp in search_paths:
		candidate = "%s/%s" % (sp, directory)
		if os.path.isdir(candidate):
			return candidate
	if fatal:
		raise Exception("Cannot find libs for: %s" % directory)


def get_platform_default_inc():
	__base = {
		"linux" : ["/usr", "/usr/local"],
		"freebsd" : ["/usr/local", "/usr"],
	}
	return list(
		map(lambda x: "%s/include" % x, __base.get(THIS_PLATFORM))
	)

def get_platform_default_libs():
	libs = {
		"linux" : [],
		"freebsd" : ["execinfo"],
	}
	return libs.get(THIS_PLATFORM)


# ------------------------------------------------------------------------
# SETUP
# ------------------------------------------------------------------------

setup = Setup()
setup.create_directories()

# ------------------------------------------------------------------------
# BASE ENVIRONMENT
# ------------------------------------------------------------------------

baseenv = Environment(
	CXX = GetOption("compiler"),
	CXXFLAGS = setup.base_cxxflags,
	CPPDEFINES = setup.base_defines,
	CPPPATH = [Dirs.project_source] + get_platform_default_inc(),
	LIBS = setup.base_libs + get_platform_default_libs(),
	LIBPATH = setup.base_libpath,
	LINKFLAGS = setup.base_linkflags
)

baseenv["ENV"]["TERM"] = os.environ["TERM"]

extend_env(baseenv, {
	"CPPPATH" : ["src/external/include"], # resolve_include("jsoncpp")],
	"LIBS" : ["jsoncpp"],
	"LIBPATH" : [resolve_libpath("jsoncpp")]
})

# ------------------------------------------------------------------------
# BOOST ENVIRONMENT
# ------------------------------------------------------------------------

boostenv = baseenv.Clone()
extend_env(boostenv, {
	"LIBS" : [
		"boost_system",
		"boost_thread",
		"boost_program_options",
		#"boost_log",
		#"boost_filesystem",
		#"boost_regex",
	]
})
# ------------------------------------------------------------------------
# YAMI ENVIRONMENT
# ------------------------------------------------------------------------

yamienv = baseenv.Clone()

extend_env(yamienv, {
	"CPPPATH" : [
		resolve_include("yami4"),
		#resolve_include("yami4-cpp", ns="yami4"),
		#resolve_include("yami4-core", ns="yami4")
	],
	"LIBPATH" : [resolve_libpath("yami4")],
	"LIBS" : ["yami4cppd", "yami4cored"]
})

# ------------------------------------------------------------------------
# DB ENVIRONMENT
# ------------------------------------------------------------------------

db_backends = {
	"mysql" : {
		"include" : Split("mysql soci"),
		"libs" : Split("mysqlclient soci_mysql"),
		"libpath" : Split("mysql soci"),
	},
	"postgresql" : {
		"include" : Split("postgresql soci"),
		"libs" : Split("pq soci_postgresql"),
		"libpath" : Split("postgresql soci"),
	},
	"mongodb" : {
		"include" : Split("mongo"),
		"libs" : Split("mongoclient"),
		"libpath" : Split("mongo"),
	}
}

includes_db = [resolve_include("soci")]
libpath_db = [resolve_libpath("soci")]
libs_db = ["soci_core"]

for dbback in db_backends:
	if is_option_set(dbback):
		includes_db.extend(
			list(map(lambda p:
					 resolve_include(p),
					 db_backends[dbback]["include"])
			 )
		)

		libs_db.extend(db_backends[dbback]["libs"])

		libpath_db.extend(
			list(map(lambda d:
					 resolve_libpath(d),
					 db_backends[dbback]["libpath"]
				 )
			 )
		)

dbenv = baseenv.Clone()
extend_env(dbenv, {
	"CPPPATH" : includes_db,
	"LIBS" : libs_db,
	"LIBPATH" : libpath_db,
})

# ------------------------------------------------------------------------
# COMBINED ENVIRONMENT
# ------------------------------------------------------------------------

combinedenv = baseenv.Clone()
extend_env(combinedenv, [dbenv, yamienv, boostenv])


# ------------------------------------------------------------------------
# EXT_MODS ENVIRONMENT
# ------------------------------------------------------------------------

extmodsenv = baseenv.Clone()
extend_env(extmodsenv, [combinedenv])
extend_env(extmodsenv, {
	"CPPPATH" : [
		os.path.join(Dir(".").path, "../../"),
		os.path.join(Dir(".").path, "../../nix"),
	],
})

# ------------------------------------------------------------------------
# TESTS ENVIRONMENT
# ------------------------------------------------------------------------

testsenv = baseenv.Clone()
extend_env(testsenv, [dbenv, yamienv, boostenv, combinedenv])


# ------------------------------------------------------------------------
# TRANSLATION UNITS
# ------------------------------------------------------------------------

translation_units = {
	"core/auth" : {
		"env": yamienv,
	},
	"core/client" : {
		"env": combinedenv,
	},
	"core/service_client" : {
		"env": combinedenv,
	},
	"core/resolver_client" : {
		"env": combinedenv,
	},
	# ------------------------
	"direct_handlers" : {
		"env": combinedenv,
	},
	# ------------------------
	"db/connection" : {},
	"db/instance_config" : {},
	"db/options" : {},
	"init/common" : {
		"env": combinedenv,
	},
	"init/daemon" : {
		"env": combinedenv,
	},
	"job" : {},
	"message" : {
		"env": combinedenv,
	},
	"message/incoming" : {
		"env": combinedenv,
	},
	"message/response" : {
		"env": combinedenv,
	},
	"message/value" : {
		"env": combinedenv,
	},
	"module" : {
		"env": combinedenv,
	},
	"module/api" : {},
	"module/builtin/debug" : {
		"env": combinedenv,
	},
	"module/builtin/debug/worker" : {
		"env": combinedenv,
	},
	"module/builtin/service/cache" : {
		"env": combinedenv,
	},
	"module/builtin/service/job_queue" : {
		"env": combinedenv,
	},
	"module/builtin/service/resolver" : {
		"env": combinedenv,
	},
	"module/instance" : {
		"env": combinedenv,
	},
	"module/manager" : {
		"env": combinedenv,
	},
	"options" : {},
	"program_options" : {
		"libs" : ["boost_program_options"],
		"libpath" : boostenv["LIBPATH"],
	},
	"queue/options" : {},
	"route" : {
		"env": combinedenv,
	},
	"server" : {
		"env": combinedenv,
	},
	"server/dispatcher" : {
		"env": combinedenv,
	},
	# "transport/options" : {},
	# "transport/yami" : {
	#	"env" : yamienv,
	# },
	"util/fs" : {},
	"util/pid" : {},
	"util/string" : {},
	"util/test" : {},
	"types" : {
		"env" : yamienv,
	},
	# ######################################################################
	# "core/auth/auth" : {
	#	"cpppath" : [resolve_include("yami4")],
	#	"env" : dbenv
	# },
	# "daemon/daemon" : {
	#	"env" : combinedenv,
	# },
	# "daemon/signal_handlers" : {
	#	"env" : combinedenv,
	# },
	# "util/version" : {},
	# "util/pid" : {},
	# "generic/uuid" : {},
}

main_target_objects = []

for tunit in sorted(translation_units):
	tunit_def = translation_units[tunit]
	tunit_env = tunit_def.get("env", baseenv).Clone()
	extend_env(tunit_env, {
		"CPPPATH" : tunit_def.get("cpppath", []),
		"LIBS" : tunit_def.get("libs", []),
		"LIBPATH" : tunit_def.get("libpath", []),
	})
	#print(tunit, tunit_env["CPPPATH"], tunit_env)
	obj = tunit_env.SharedObject(
		os.path.join(Dirs.objects, tunit) + ".o",
		os.path.join(Dirs.project_source, "nix", tunit) + ".cxx"
	)
	main_target_objects.append(obj)

# ------------------------------------------------------------------------
# BUILD TESTS - FIXME
# ------------------------------------------------------------------------

all_tests = []
for (root, dirs, files) in os.walk(Dirs.tests_source):
	for u in filter(lambda f: f.endswith(".cxx"), files):
		u = "%s/%s" % (root, u)
		basename = os.path.basename(u.replace(".cxx", ""))
		test_exe = "%s/%s.bin" % (Dirs.tests_target, basename)
		all_tests.append(test_exe)
		testsenv.Program(
			target = test_exe,
			source = [u, main_target_objects]
		)

testsenv.Alias("test", all_tests)

# ------------------------------------------------------------------------
# BUILD MAIN TARGET
# ------------------------------------------------------------------------

main_unit = "%s/main.cxx" % Dirs.source
target_executable = "%s/%s" % (Dirs.target, PROJECT_TARGET)
implenv = combinedenv.Clone()
implenv.Program(
	target = target_executable,
	source = [main_unit, main_target_objects]
)

implenv.Install(Dirs.install, target_executable)
implenv.Alias("install", Dirs.install)
implenv.Default(target_executable)

combinedenv.Alias("all", Dirs.install)

print("Building targets: %s\n" % "\n".join(list(map(str, BUILD_TARGETS))))

if GetOption("build_modules") and False:
	base_dir = Dir(".").path
	nix_objs_dir = os.path.join(base_dir, "../../..",	Dirs.objects)

	exported_objs = {
		"core" : [],
		"options" : [],
		"plugin" : [],
		"generic" : [],
		"runtime" : [],
	}

	core_objects = []
	for export_dir in exported_objs:
		for r, d, files in os.walk(os.path.join(base_dir, Dirs.objects, export_dir)):
			for f in files:
				if f.endswith(".o"):
					exported_objs[export_dir].append(
						os.path.join(r.replace(Dirs.objects + "/", ""), f))
		core_objects.extend(exported_objs[export_dir])
	core_objects = sorted(map(lambda o: os.path.join(nix_objs_dir, o), core_objects))

	destdir = os.path.join(base_dir, "../../../../lib")
	SConscript(
		"src/ext_mods/server_status/SConscript",
		exports=["core_objects", "extmodsenv", "destdir"],
		
	)
