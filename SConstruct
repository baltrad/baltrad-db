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
from build_helper import (CheckBoostVersion, CheckHlhdf, SplitResult)

EnsureSConsVersion(1, 2)

def convert_test_db_dsns(value):
    dsns = value.split(",")
    dsns = [dsn for dsn in dsns if dsn.strip()]
    return dsns

vars = Variables("variables.cache")

vars.AddVariables(
    PathVariable("prefix", "installation prefix",
                 "/usr/local", PathVariable.PathIsDir),
    PathVariable("pqxx_dir", "libpqxx install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("pqxx_lib_dir", "libpqxx libraries directory",
                 "${pqxx_dir}/lib", PathVariable.PathIsDir),
    PathVariable("pqxx_include_dir", "libpqxx include directory",
                 "${pqxx_dir}/include", PathVariable.PathIsDir),
    PathVariable("hdf5_dir", "HDF5 install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("hdf5_lib_dir", "HDF5 libraries directory",
                 "${hdf5_dir}/lib", PathVariable.PathIsDir),
    PathVariable("hdf5_include_dir", "HDF5 include directory",
                 "${hdf5_dir}/include", PathVariable.PathIsDir),
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
    PathVariable("ant_dir", "directory where ant libraries are found",
                 "/usr/share/java", PathVariable.PathIsDir),
    BoolVariable("debug", "generate debug code", False),
    BoolVariable("build_java", "build java bindings", True),
    PathVariable("install_root", "installation directory",
                 "${prefix}/baltrad/db-${version}", PathVariable.PathAccept),
                ("test_db_dsns", "comma separated dsns to test against", "")
)

##
# set up default environment
##

env = Environment(tools=["default", "doxygen", "swig", "textfile"],
                  toolpath=["misc/scons_tools"],
                  variables=vars,
                  CCFLAGS=["-pedantic", "-Wall", "-Wno-long-long"],
                  CPPPATH=["#src"],
                  LIBPATH=["#lib"],
                  ENV={"PATH": os.environ["PATH"]})
confenv = env.Clone()
confenv["CCFLAGS"].remove("-pedantic")

env.Default(env.Alias("shared-library"))

if env["build_java"]:
    env.Default(env.Alias("java-wrapper"))

env["version"] = "devel"

if vars.UnknownVariables():
    print "unknown variables:"
    for key, value in vars.UnknownVariables().iteritems():
        print " ", key, "=", value
    Exit(1)

vars.Save("variables.cache", env)

env["test_db_dsns"] = convert_test_db_dsns(env["test_db_dsns"])

Help(vars.GenerateHelpText(env))

if env["debug"]:
    env.AppendUnique(CCFLAGS=["-g", "-fno-inline-functions"])
else:
    env.AppendUnique(CCFLAGS=["-O2", "-fno-strict-aliasing"])

env.AppendUnique(CPPPATH=["${hdf5_include_dir}",
                          "${hlhdf_include_dir}",
                          "${pqxx_include_dir}",
                          "${icu_include_dir}",
                          "${boost_include_dir}"])
env.AppendUnique(LIBPATH=["${hdf5_lib_dir}",
                          "${hlhdf_lib_dir}",
                          "${pqxx_lib_dir}",
                          "${icu_lib_dir}",
                          "${boost_lib_dir}"])

##
# configure
##

class Config(object):
    _custom_tests = {
       "CheckBoostVersion": CheckBoostVersion,
       "CheckHlhdf": CheckHlhdf,
    }

    def __init__(self, env):
        self.cfg = env.Configure(custom_tests=self._custom_tests,
                                 clean=False, help=False)
    
    def check(self):
        cfg = self.cfg
        
        cfg.env.AppendUnique(CPPPATH="${pqxx_include_dir}",
                             LIBPATH="${pqxx_lib_dir}")
        self.pqxx = cfg.CheckLibWithHeader("pqxx", "pqxx/pqxx", "c++")

        cfg.env.AppendUnique(CPPPATH=env["hdf5_include_dir"],
                             LIBPATH=env["hdf5_lib_dir"])
        self.hdf5 = cfg.CheckLibWithHeader("hdf5", "hdf5.h", "c++")

        cfg.env.AppendUnique(CPPPATH=env["hlhdf_include_dir"],
                             LIBPATH=env["hlhdf_lib_dir"])
        self.hlhdf = cfg.CheckHlhdf()
        
        cfg.env.AppendUnique(CPPPATH="${icu_include_dir}",
                             LIBPATH="${icu_lib_dir}")
        self.icu_uc = cfg.CheckLibWithHeader("icuuc", "unicode/unistr.h", "c++")
        self.icu_tu = cfg.CheckLibWithHeader("icutu", "unicode/regex.h", "c++")

        cfg.env.AppendUnique(CPPPATH=env["boost_include_dir"])
        self.boost = 0 not in [cfg.CheckBoostVersion("1.38"),
                               self._check_boost_headers()]

        cfg.env.AppendUnique(CPPPATH="${gtest_include_dir}",
                             LIBPATH="${gtest_lib_dir}")
        self.gtest = cfg.CheckLibWithHeader("gtest", "gtest/gtest.h", "c++")
        self.gmock = cfg.CheckLibWithHeader("gmock", "gmock/gmock.h", "c++")

        cfg.env.AppendUnique(CPPPATH=["${jdk_include_dir}",
                                       "${jdk_include_dir}/linux"],
                              LIBPATH="${jdk_lib_dir}")
        self.jni = cfg.CheckCHeader("jni.h")
        cfg.Finish()
   
    def _check_boost_headers(self):
        headers = (
            "enable_shared_from_this.hpp",
            "foreach.hpp",
            "lexical_cast.hpp",
            "noncopyable.hpp",
            "scoped_ptr.hpp",
            "shared_ptr.hpp",
            "variant.hpp",
            "iterator/iterator_facade.hpp",
            "numeric/conversion/cast.hpp",
        )

        rets = []
        for header in headers:
            rets.append(self.cfg.CheckCXXHeader("/".join(("boost", header))))
        return 0 not in rets
    
    def has_sql_dialect(self, dialect):
        if dialect == "postgresql" and self.pqxx:
            return True
        return False

conf = Config(confenv)
conf.check()


def conf_obj(value):
    return Value(getattr(conf, value))

def conf_has(target, source, env):
    if not isinstance(source, list):
        source = [source]
    missing = []
    for src in source:
        if not src.value:
            missing.append(str(src))
    if missing:
        print >> sys.stderr, "missing dependencies: %s" % " ".join(missing)
        return 1

env.Append(BUILDERS={"ConfHas": env.Builder(action=conf_has, source_factory=conf_obj)})


# remove dsns with unavailable driver
for dsn in env["test_db_dsns"]:
    dialect = dsn.split(":", 1)[0]
    if not conf.has_sql_dialect(dialect):
        env["test_db_dsns"].remove(dsn)
        print "removed %r from test_db_dsns: dialect unavailable" % dsn

##
# set up targets
##

doc = env.Doxygen("doc/Doxyfile")
env.Alias("doc", doc)

SConscript("src/brfc/SConscript",
           build_dir="build/brfc", duplicate=0,
           exports={"env": env.Clone()})

testenv = env.Clone()
testenv.AppendUnique(CPPPATH=["${gtest_include_dir}"])
testenv.AppendUnique(LIBPATH=["${gtest_lib_dir}"])

for path in ("#lib", "${gtest_lib_dir}", "${hlhdf_lib_dir}", "${boost_lib_dir}"):
    abspath = env.Dir(path).abspath
    if abspath not in ("/lib", "/usr/lib"): # default ld lookups
        testenv.AppendENVPath('LD_LIBRARY_PATH', abspath)

SConscript("test/brfc/SConscript",
           build_dir="build/test/brfc", duplicate=0,
           exports={"env": testenv})

jtestenv = env.Clone()
SConscript("test/java/SConscript",
           build_dir="build/test/java", duplicate=0,
           exports={"env": jtestenv})



SConscript("swig/SConscript",
           build_dir="build/swig", duplicate=0,
           exports={"env": env.Clone()})

env.Install("${install_root}/sql/postgresql",
            ["#schema/postgresql/create.sql", "#schema/postgresql/drop.sql"])
env.Alias("install", [env["install_root"]])

run_gtest_tests = testenv.Command("run_gtest_tests", "#test/runner",
                                  "$SOURCE "
                                  "--gtest_output=xml:test/reports/gtest.xml")
env.Alias("test", run_gtest_tests)

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

run_java_tests = testenv.Command("run_java_tests", "#lib/jbrfc_test.jar",
    "ant "
    "-lib ./lib -lib ./deplib -lib ${ant_dir} " +
    " ".join(ant_testdb_properties()) + 
    " -f test/java/build.xml")

env.Alias("hudsontest", [run_gtest_tests, run_java_tests])


# vim:filetype=python:et:ts=4:sw=4:
