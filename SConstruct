import os
import sys
import urlparse


from build_helper import (CheckBoostVersion, CheckHlhdf, CheckQt,
                          CheckQtSqlDrivers, SplitResult)

EnsureSConsVersion(1, 2)

def convert_test_db_dsns(value):
    dsns = value.split(",")
    dsns = [dsn for dsn in dsns if dsn.strip()]
    return dsns

vars = Variables("variables.cache")

vars.AddVariables(
    PathVariable("prefix", "installation prefix",
                 "/usr/local", PathVariable.PathIsDir),
    PathVariable("qt_dir", "Qt4 install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("qt_lib_dir", "Qt4 libraries directory",
                 "${qt_dir}/lib", PathVariable.PathIsDir),
    PathVariable("qt_include_dir", "Qt4 include directory",
                 "${qt_dir}/include", PathVariable.PathIsDir),
    PathVariable("hlhdf_dir", "HLHDF install root",
                 "/usr", PathVariable.PathIsDir),
    PathVariable("hlhdf_lib_dir", "HLHDF libraries directory",
                 "${hlhdf_dir}/lib", PathVariable.PathIsDir),
    PathVariable("hlhdf_include_dir", "HLHDF include directory",
                 "${hlhdf_dir}/include", PathVariable.PathIsDir),
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
                  toolpath=["scons_tools"],
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

env.AppendUnique(CPPPATH=["${qt_include_dir}",
                          "${hlhdf_include_dir}",
                          "${boost_include_dir}"])
env.AppendUnique(LIBPATH=["${qt_lib_dir}",
                          "${hlhdf_lib_dir}",
                          "${boost_lib_dir}"])

##
# configure
##

class Config(object):
    _custom_tests = {
       "CheckBoostVersion": CheckBoostVersion,
       "CheckHlhdf": CheckHlhdf,
       "CheckQt": CheckQt,
       "CheckQtSqlDrivers": CheckQtSqlDrivers,
    }

    def __init__(self, env):
        self.cfg = env.Configure(custom_tests=self._custom_tests,
                                 clean=False, help=False)
    
    def check(self):
        cfg = self.cfg

        cfg.env.AppendUnique(CPPPATH=env["qt_include_dir"],
                             LIBPATH=env["qt_lib_dir"])
        cfg.env.AppendENVPath("LD_LIBRARY_PATH",
                              env.Dir(env["qt_lib_dir"]).abspath)
        self.qt = cfg.CheckQt()
        self.qtsql = cfg.CheckLibWithHeader("QtSql", "QtSql/QSqlDatabase", "c++")
        self.qtsql_drivers = cfg.CheckQtSqlDrivers()

        cfg.env.AppendUnique(CPPPATH=env["hlhdf_include_dir"],
                             LIBPATH=env["hlhdf_lib_dir"])
        self.hlhdf = cfg.CheckHlhdf()

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
        db_driver_map = {
            "sqlite": "QSQLITE",
            "postgresql": "QPSQL",
        }
        try:
            driver = db_driver_map[dialect]
        except KeyError:
            return False
        else:
            return driver in self.qtsql_drivers

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

for path in ("#lib", "${gtest_lib_dir}", "${hlhdf_lib_dir}", "${qt_lib_dir}"):
    testenv.AppendENVPath('LD_LIBRARY_PATH', env.Dir(path).abspath)

SConscript("test/SConscript",
           build_dir="build/test", duplicate=0,
           exports={"env": testenv})

jtestenv = env.Clone()
SConscript("test/java/SConscript",
           build_dir="build/test/java", duplicate=0,
           exports={"env": jtestenv})



SConscript("swig/SConscript",
           build_dir="build/swig", duplicate=0,
           exports={"env": env.Clone()})

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
                "-Ddb.password=%s" % url.password]
    return []

run_java_tests = testenv.Command("run_java_tests", "#lib/jbrfc_test.jar",
    "ant "
    "-lib ./lib -lib ./deplib -lib ${ant_dir} " +
    " ".join(ant_testdb_properties()) + 
    " -f test/java/build.xml")

env.Alias("hudsontest", [run_gtest_tests, run_java_tests])


# vim:filetype=python:et:ts=4:sw=4:
