import os

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
    PathVariable("hudsontest_output", "where to generate output for HUDSON",
                 "test_results.xml", PathVariable.PathAccept),
    BoolVariable("debug", "generate debug code", False),
    BoolVariable("build_java", "build java bindings", True),
                ("install_root", "installation directory",
                 "${prefix}/baltrad/db-${version}"),
    ("test_db_dsn", "database to use for testing", None)
)

env = Environment(tools=["default", "doxygen", "swig"],
                  toolpath=["scons_tools"],
                  variables=vars,
                  CCFLAGS=["-pedantic", "-Wall", "-Wno-long-long"],
                  CPPPATH=["#src"],
                  LIBPATH=["#lib"],
                  ENV={"PATH": os.environ["PATH"]})

env.Default(None)

env["version"] = "devel"

if vars.UnknownVariables():
    print "unknown variables:"
    for key, value in vars.UnknownVariables().iteritems():
        print " ", key, "=", value
    Exit(1)

vars.Save("variables.cache", env)

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
           build_dir="build/test", duplicate=0,
           exports={"env": testenv})

if env["build_java"]:
    SConscript("swig/SConscript",
               build_dir="build/swig", duplicate=0,
               exports={"env": env.Clone()})

env.Alias("install", [env["install_root"]])

test = testenv.Command("test_runner", "#test/runner",
                       "$SOURCE ")
env.Alias("test", test)

hudsontest = testenv.Command("hudsontest", "#test/runner",
                   "$SOURCE "
                   "--gtest_output=xml:${hudsontest_output}")

# vim:filetype=python:et:ts=4:sw=4:
