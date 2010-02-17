import os
import sys

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
    PathVariable("junit_dir", "directory where junit4.jar is found",
                 "/usr/share/java", PathVariable.PathIsDir),
    PathVariable("ant_dir", "directory where ant libraries are found",
                 "/usr/share/java", PathVariable.PathIsDir),
    BoolVariable("debug", "generate debug code", False),
    BoolVariable("build_java", "build java bindings", True),
    PathVariable("install_root", "installation directory",
                 "${prefix}/baltrad/db-${version}", PathVariable.PathAccept),
                ("test_db_dsns", "comma separated dsns to test against", "")
)

env = Environment(tools=["default", "doxygen", "swig"],
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
           build_dir="build/test", duplicate=1,
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

run_java_tests = testenv.Command("run_java_tests", "#lib/jbrfc_test.jar",
    "ant "
    "-lib ./lib -lib ${junit_dir} -lib ${ant_dir} "
    "-f test/java/build.xml")

env.Alias("hudsontest", [run_gtest_tests, run_java_tests])

##
# configure
##

def CheckBoost(ctx, boost_include_dir):
    ctx.Message("Checking for Boost version >= 1.38... ")
    ctx.env.AppendUnique(CPPPATH=boost_include_dir)
    src = (
        "#include <boost/version.hpp>",
        "int main(int argc, char** argv) {",
        "    if (BOOST_VERSION / 100000 == 1 and BOOST_VERSION / 100 % 1000 >= 38)",
        "        return 0;",
        "    else",
        "        return 1;",
        "}"
    )

    result, _ = ctx.TryRun("\n".join(src), ".cpp")
    ctx.Result(result)

    if not result: return 0

    headers = (
        "enable_shared_from_this.hpp",
        "foreach.hpp",
        "lexical_cast.hpp",
        "noncopyable.hpp",
        "scoped_ptr.hpp",
        "shared_ptr.hpp",
        "variant.hpp",
        "algorithm/string/classification.hpp",
        "algorithm/string/erase.hpp",
        "algorithm/string/join.hpp",
        "algorithm/string/split.hpp",
        "iterator/iterator_facade.hpp",
        "lambda/bind.hpp",
        "lambda/lambda.hpp",
        "numeric/conversion/cast.hpp",
    )

    for header in headers:
        result = conf.CheckCXXHeader("/".join(("boost", header)))
        if not result:
            return 0
    return 1

def CheckHlhdf(ctx, hlhdf_include_dir, hlhdf_lib_dir):
    src = (
        "#include \"hlhdf.h\"",
        "int main() { return 0; }"
    )
    ctx.Message("Checking for HLHDF... ")
    oldlibs = ctx.AppendLIBS(["hlhdf", "hdf5"])
    ctx.env.AppendUnique(CPPPATH=hlhdf_include_dir, LIBPATH=hlhdf_lib_dir)
    result = ctx.TryLink("\n".join(src), ".c")
    ctx.Result(result)
    ctx.SetLIBS(oldlibs)

    return result

def CheckQt(ctx, qt_include_dir, qt_lib_dir):
    src = (
        "#include <QtCore/QtGlobal>",
        "int main(int argc, char** argv) {",
        "    if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))",
        "        return 0;",
        "    else",
        "        return 1;",
        "}"
    )
    ctx.Message("Checking for Qt >= 4.5... ")
    oldlibs = ctx.AppendLIBS(["QtCore"])
    ctx.env.AppendUnique(CPPPATH=qt_include_dir, LIBPATH=qt_lib_dir)
    ctx.env.AppendENVPath('LD_LIBRARY_PATH', env.Dir(qt_lib_dir).abspath)
    result, _ = ctx.TryRun("\n".join(src), ".cpp")
    ctx.Result(result)
    ctx.SetLIBS(oldlibs)
    return result

def CheckQtSqlDrivers(ctx, qt_include_dir, qt_lib_dir):
    src = (
        "#include <QtSql/QSqlDatabase>",
        "#include <QtCore/QCoreApplication>",
        "#include <QtCore/QStringList>",
        "#include <iostream>",
        "int main(int argc, char** argv) {",
        "   QCoreApplication app(argc, argv);",
        "   QStringList drivers = QSqlDatabase::drivers();",
        "   std::cout << drivers.join(\" \").toStdString();",
        "   return 0;",
        "}\n"
    )
    ctx.Message("Checking for available QtSql drivers... ")
    oldlibs = ctx.AppendLIBS(["QtSql"])
    ctx.env.AppendUnique(CPPPATH=qt_include_dir, LIBPATH=qt_lib_dir)
    ctx.env.AppendENVPath('LD_LIBRARY_PATH', env.Dir(qt_lib_dir).abspath)
    result, drivers = ctx.TryRun("\n".join(src), ".cpp")
    ctx.Result(drivers)
    ctx.SetLIBS(oldlibs)
    return drivers.split(" ")

conf = Configure(confenv,
                 custom_tests={
                    "CheckBoost": CheckBoost,
                    "CheckHlhdf": CheckHlhdf,
                    "CheckQt": CheckQt,
                    "CheckQtSqlDrivers": CheckQtSqlDrivers,
                 },
                 clean=False, help=False)

rets = [] # list of conf return values

_TARGET_STRS = map(str, BUILD_TARGETS)

if set(["shared-library", "java-wrapper", "test", "hudsontest"]) & set(_TARGET_STRS):
    rets.append(conf.CheckQt(env["qt_include_dir"], env["qt_lib_dir"]))
    rets.append(conf.CheckLibWithHeader("QtSql", "QtSql/QSqlDatabase", "c++"))
    drivers = conf.CheckQtSqlDrivers(env["qt_include_dir"], env["qt_lib_dir"])
    conf.env["qtsql_drivers"] = drivers
    rets.append(conf.CheckHlhdf(env["hlhdf_include_dir"], env["hlhdf_lib_dir"]))
    rets.append(conf.CheckBoost(env["boost_include_dir"]))

if set(["test", "hudsontest"]) & set(_TARGET_STRS):
    db_driver_map = {
        "sqlite": "QSQLITE",
        "postgresql": "QPSQL",
    }
    conf.env.AppendUnique(CPPPATH="${gtest_include_dir}",
                          LIBPATH="${gtest_lib_dir}")
    rets.append(conf.CheckLibWithHeader("gtest", "gtest/gtest.h", "c++"))
    rets.append(conf.CheckLibWithHeader("gmock", "gmock/gmock.h", "c++"))
    for dsn in env["test_db_dsns"]:
        dialect = dsn.split(":", 1)[0]
        if dialect not in db_driver_map:
            print "Invalid dialect specified in %r" % dsn
            rets.append(0)
        elif db_driver_map[dialect] not in conf.env["qtsql_drivers"]:
            print "Missing QtSql driver for %s" % dialect
            rets.append(0)
    #XXX: enable when fully supporting sqlite
    #sqlite_dsns = [dsn for dsn in env["test_db_dsns"]
    #               if dsn.startswith("sqlite")]
    #if not sqlite_dsns and "SQLITE" in drivers:
    #    env["test_db_dsns"].append("sqlite:///:memory:")

if "java-wrapper" in _TARGET_STRS:
    conf.env.AppendUnique(CPPPATH=["${jdk_include_dir}",
                                   "${jdk_include_dir}/linux"],
                          LIBPATH="${jdk_lib_dir}")
    rets.append(conf.CheckCHeader("jni.h"))

if 0 in rets:
    print >> sys.stderr, "\nMissing required dependencies."
    Exit(1)

env = conf.Finish()

# vim:filetype=python:et:ts=4:sw=4:
