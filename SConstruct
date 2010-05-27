# Copyright 2010 Estonian Meteorological and Hydrological Institute
# 
# This file is part of baltrad-db.
# 
# baltrad-db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# baltrad-db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.

import os
import sys
import urlparse

sys.path.append("./misc")
from build_helper import (Config, SplitResult)

EnsureSConsVersion(1, 2)

def convert_test_db_dsns(value):
    dsns = value.split(",")
    dsns = [dsn for dsn in dsns if dsn.strip()]
    return dsns

vars = Variables(".vars.cache")

vars.AddVariables(
    PathVariable("prefix", "installation prefix",
                 "/usr/local", PathVariable.PathIsDir),
    PathVariable("pqxx_dir", "libpqxx install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("pqxx_lib_dir", "libpqxx libraries directory",
                 "${pqxx_dir}/lib", PathVariable.PathIsDir),
    PathVariable("pqxx_include_dir", "libpqxx include directory",
                 "${pqxx_dir}/include", PathVariable.PathIsDir),
    PathVariable("hlhdf_dir", "HLHDF install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("hlhdf_lib_dir", "HLHDF libraries directory",
                 "${hlhdf_dir}/lib", PathVariable.PathIsDir),
    PathVariable("hlhdf_include_dir", "HLHDF include directory",
                 "${hlhdf_dir}/include", PathVariable.PathIsDir),
    PathVariable("icu_dir", "ICU install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("icu_lib_dir", "ICU libraries directory",
                 "${icu_dir}/lib", PathVariable.PathIsDir),
    PathVariable("icu_include_dir", "ICU include directory",
                 "${icu_dir}/include", PathVariable.PathIsDir),
    PathVariable("jdk_dir", "JDK install root",
                 "/usr/lib/jvm/java-6-openjdk", PathVariable.PathIsDir),
    PathVariable("jdk_lib_dir", "JDK libraries directory",
                 "${jdk_dir}/lib", PathVariable.PathIsDir),
    PathVariable("jdk_include_dir", "JDK include directory",
                 "${jdk_dir}/include", PathVariable.PathIsDir),
    PathVariable("boost_dir", "Boost install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("boost_lib_dir", "Boost libraries directory",
                 "${boost_dir}/lib", PathVariable.PathIsDir),
    PathVariable("boost_include_dir", "Boost include directory",
                 "${boost_dir}/include", PathVariable.PathIsDir),
    PathVariable("gtest_dir", "googletest install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("gtest_lib_dir", "googletest libraries directory",
                 "${gtest_dir}/lib", PathVariable.PathIsDir),
    PathVariable("gtest_include_dir", "googletest include directory",
                 "${gtest_dir}/include", PathVariable.PathIsDir),
    PathVariable("ant_dir", "directory of ant installation",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("ant_executable", "Ant executable",
                 "${ant_dir}/bin/ant", PathVariable.PathIsFile),
    BoolVariable("debug", "generate debug code", False),
    BoolVariable("build_java", "build java bindings", True),
    PathVariable("install_root", "installation directory",
                 "${prefix}/baltrad/db-${version}", PathVariable.PathAccept),
                ("test_db_dsns", "comma separated dsns to test against", "")
)

##
# create default environment
##

env = Environment(tools=["default", "doxygen", "swig", "textfile"],
                  toolpath=["misc/scons_tools"],
                  variables=vars,
                  ENV=os.environ)


if vars.UnknownVariables():
    print "unknown variables:"
    for key, value in vars.UnknownVariables().iteritems():
        print " ", key, "=", value
    Exit(1)

def my_format(env, key, help, default, actual, aliases):
    fmt = "  %s:\n      %s\n      value: %s\n"
    return fmt % (key, help, actual)
vars.FormatVariableHelpText = my_format

Help("VARIABLES:\n")
Help(vars.GenerateHelpText(env))
Help("""
TARGETS:
  configure
      configure the build environment and check for dependencies
  doc
      generate Doxygen documentation to doc/
      (can be called unconfigured)
  build-shared-library
      build shared library to lib/brfc.so
  build-java-wrapper
      build Java wrapper to lib/jbrfc.so, lib/libbrfc_java.so and jbrfc.jar
  build-gtest-tests
      build test/runner (tests written in C++)
  run-gtest-tests
      execute test/runner with xml output to test/reports/gtest.xml
  build-java-tests
      build tests written in Java
  run-java-tests
      execute tests written in Java
  run-all-tests
      execute both C++ and Java tests
  install
      install built libraries
  distclean
      clean all files (more than `scons -c`)
""")

##
# configure
##
conf = Config(env)

env.Action("#configure", None) #dummy target
if "configure" in COMMAND_LINE_TARGETS:
    # variables are saved here because configuration
    # validity depends upon them
    vars.Save(".vars.cache", env)

    conf.configure()

    if conf.has_minimal_deps():
        conf.save()
    else:
        print "\nMinimal dependencies not met. Fix and reconfigure."
    Exit() # don't proceed to build default targets


if GetOption("clean"):
    pass # clean may pass unconfigured
if GetOption("help"):
    print sys.modules["SCons.Script"].help_text
    Exit()
elif len(COMMAND_LINE_TARGETS) == 1 and "doc" in COMMAND_LINE_TARGETS:
    pass # doc may pass unconfigured
elif not conf.is_configured():
    print "\nHave no configuration, run 'scons configure'"
    #Exit()
elif not conf.has_minimal_deps():
    print "\nMinimal dependencies not met. Reconfigure"
    Exit()


##
# set up default environment for building
#
env.AppendUnique(CCFLAGS=["-pedantic", "-Wall", "-Wno-long-long"])

if env["debug"]:
    env.AppendUnique(CCFLAGS=["-g", "-fno-inline-functions"])
else:
    env.AppendUnique(CCFLAGS=["-O2", "-fno-strict-aliasing"])

env.AppendUnique(CPPPATH=["#src",
                          "${hlhdf_include_dir}",
                          "${pqxx_include_dir}",
                          "${icu_include_dir}",
                          "${boost_include_dir}"])

libdirs = [
    "${hlhdf_lib_dir}",
    "${pqxx_lib_dir}",
    "${icu_lib_dir}",
    "${boost_lib_dir}"
]

env.AppendUnique(LIBPATH="#lib")
env.AppendUnique(LIBPATH=libdirs)

ld_path = env["ENV"].get("LD_LIBRARY_PATH", "").split(":")
ld_path = [os.path.abspath(path) for path in ld_path if path]
ld_path.insert(0, env.Dir("#lib").abspath)
for path in libdirs:
    abspath = env.Dir(path).abspath
    if abspath not in ("/lib", "/usr/lib") and abspath not in ld_path:
        ld_path.append(abspath)

env["ENV"]["LD_LIBRARY_PATH"] = ":".join(ld_path)

env["version"] = "devel"

env["test_db_dsns"] = convert_test_db_dsns(env["test_db_dsns"])

# remove dsns with unavailable driver
for dsn in env["test_db_dsns"]:
    dialect = dsn.split(":", 1)[0]
    if not conf.has_sql_dialect(dialect):
        env["test_db_dsns"].remove(dsn)
        print "removed %r from test_db_dsns: dialect unavailable" % dsn

##
# set up targets
##

# set default target to 'build-shared-library' and 'build-java-wrapper'
env.Default(env.Alias("build-shared-library"))
if env["build_java"]:
    env.Default(env.Alias("build-java-wrapper"))

# add 'doc' target
doc = env.Doxygen("doc/Doxyfile")
env.Alias("doc", doc)

# add 'distclean' target
env.Clean("distclean", ["#.sconsign.dblite"
                        "#/.sconf_temp",
                        "#config.log",
                        "#test/reports",
                        "#.configvars.cache",
                        ".vars.cache",
                        "#build"])

# add 'build-shared-library' target
SConscript(
    "src/brfc/SConscript",
    build_dir="build/brfc", duplicate=0,
    exports={
        "env": env.Clone(),
        "conf": conf,
    }
)

# add 'build-gtest' target
SConscript(
    "test/brfc/SConscript",
    build_dir="build/test/brfc", duplicate=0,
    exports={
        "env": env.Clone(),
        "conf": conf
    }
)

# add 'build-java-wrapper' target
SConscript(
    "swig/SConscript",
    build_dir="build/swig", duplicate=0,
    exports={
        "env": env.Clone(),
        "conf": conf
    }
)

# add 'build-java-tests' target
cmd = env.Command("#lib/jbrfc_test.jar", ["build-java-wrapper"],
                  "${ant_executable} "
                  "-f build.xml build-tests")
env.Alias("build-java-tests", cmd)

urlparse.uses_netloc.append("postgresql")

def ant_testdb_properties():
    # pick out postgresql dsn if present
    for dsn in env["test_db_dsns"]:
        url = SplitResult(*urlparse.urlsplit(dsn))
        if url.scheme != "postgresql":
            continue
        jdbcurl = "".join(["jdbc:", url.scheme, "://", url.hostname, url.path])
        return ["-Ddb.url=%s" % jdbcurl,
                "-Ddb.username=%s" % url.username,
                "-Ddb.password=%s" % url.password,
                "-Ddb.test_db_dsn=%s" % dsn,
                "-Ddb.test_db_schema_dir=%s" % env.Dir("#schema").abspath]
    return []

# add 'execute-java-tests' target
cmd = env.Command("#run-java-tests", ["build-java-tests"],
                  "${ant_executable} " +
                  " ".join(ant_testdb_properties()) +
                  " -f build.xml execute-tests")

#

# add sql schema as 'install' target
env.Install("${install_root}/sql/postgresql",
            ["#schema/postgresql/create.sql", "#schema/postgresql/drop.sql"])

# alias install for everything going under ${install_root}
env.Alias("install", [env["install_root"]])

 
env.Alias("run-all-tests", ["run-gtest-tests", "run-java-tests"])

# vim:filetype=python:et:ts=4:sw=4:
