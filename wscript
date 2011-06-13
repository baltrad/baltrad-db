# Copyright 2010-2011 Estonian Meteorological and Hydrological Institute
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

import optparse
import os
import sys

from waflib import Build, Logs, Options, Utils

sys.path.append("./misc")
from build_helper import urlsplit

APPNAME = "baltrad-db"
VERSION = "devel"

top = "."
out = "build"

def _add_lib_path_options(opts, libname, default_root="/usr"):
    opts.add_option("--%s_dir" % libname,
                    action="store", metavar="DIR",
                    default=default_root,
                    help="%s install root [default: %%default]" % libname)
    opts.add_option("--%s_lib_dir" % libname,
                    action="store", metavar="DIR",
                    default="${%s_dir}/lib" % libname,
                    help="%s libraries directory [default: %%default]" % libname)
    opts.add_option("--%s_inc_dir" % libname,
                    action="store", metavar="DIR",
                    default="${%s_dir}/include" % libname,
                    help="%s includes directory [default: %%default]" % libname)

_dsn_library_map = {
    "postgresql": "PQXX",
}

def _validate_test_db_dsns(dsns, env):
    valid = []
    for dsn in dsns:
        dialect = dsn.split(":", 1)[0]
        library = _dsn_library_map.get(dialect, "")
        if env["LIB_%s" % library]:
            valid.append(dsn)
    return valid

def _store_bool(option, opt, value, parser):
    _true = ["True", "true", "yes", "on", "1"]
    _false = ["False", "false", "no", "off", "0"]
    if value in _true:
        value = True
    elif value in _false:
        value = False
    else:
        vals = ", ".join(_true + _false)
        raise optparse.OptionValueError("%s must be one of: %s" % (opt, vals))
    setattr(parser.values, option.dest, value)
            
def options(opt):
    grp = opt.add_option_group("project specific options")

    grp.add_option("--debug", action="callback",
                   callback=_store_bool, type="string", nargs=1,
                   default=False, metavar="BOOL",
                   help="compile with debugging information [default: %default]")
    grp.add_option("--build_java", action="callback",
                   callback=_store_bool, type="string", nargs=1,
                   default=True, metavar="BOOL",
                   help="build java wrappers [default: %default]")
    grp.add_option("--build_tests", action="callback",
                   callback=_store_bool, type="string", nargs=1,
                   default=True, metavar="BOOL",
                   help="build tests [default: %default]")
    grp.add_option("--build_bdbtool", action="callback",
                   callback=_store_bool, type="string", nargs=1,
                   default=True, metavar="BOOL",
                   help="build bdbtool [default: %default]")
    grp.add_option("--build_bdbfs", action="callback",
                   callback=_store_bool, type="string", nargs=1,
                   default=False, metavar="BOOL",
                   help="build with FUSE filesystem"),
    grp.add_option("--test_db_dsn", action="append",
                   dest="test_db_dsns", metavar="DSN",
                   default=[],
                   help="database to test against (specify multiple times "
                        "for different databases)")

    libnames = ("boost", "hdf5", "hlhdf", "fuse", "jdk", "pqxx", "pthread")
    for libname in libnames:
        _add_lib_path_options(grp, libname)
   
    opt.tool_options("ant", tooldir="misc/waf_tools")
    opt.tool_options("swig", tooldir="misc/waf_tools")
    opt.tool_options("compiler_cc")
    opt.tool_options("compiler_cxx")

def _opts_to_env(env, opts):
    for opt in opts:
        val = getattr(Options.options, opt)
        setattr(env, opt, (Utils.subst_vars(val, env)))

def _lib_path_opts_to_env(env, libname):
    exclude = ["/usr/lib", "/usr/include"]
    vars = [libname + suffix for suffix in ("_dir", "_lib_dir", "_inc_dir")]
    _opts_to_env(env, vars)
    for var in vars:
        if getattr(env, var) in exclude:
            setattr(env, var, None)

def configure(conf):
    conf.check_tool("compiler_cc")
    conf.check_tool("compiler_cxx")
#    conf.check_tool("misc")

    env = conf.env

    env.build_java = Options.options.build_java
    env.build_tests = Options.options.build_tests
    env.build_bdbtool = Options.options.build_bdbtool
    env.build_bdbfs = Options.options.build_bdbfs
    
    if env.build_java:
        conf.check_tool("ant", tooldir="misc/waf_tools")
        conf.check_tool("swig", tooldir="misc/waf_tools")
    
    # check for libpqxx
    _lib_path_opts_to_env(env, "pqxx")
    conf.check_cxx(
        header_name="pqxx/pqxx",
        lib="pqxx",
        includes=env.pqxx_inc_dir,
        libpath=env.pqxx_lib_dir,
        uselib_store="PQXX",
        mandatory=True,
    )

    # check for hdf5
    _lib_path_opts_to_env(env, "hdf5")
    conf.check_cc(
        header_name="hdf5.h",
        lib="hdf5",
        includes=env.hdf5_inc_dir,
        libpath=env.hdf5_lib_dir,
        uselib_store="HDF5",
        mandatory=True,
    )
    
    # check for libhlhdf
    _lib_path_opts_to_env(env, "hlhdf")
    conf.check_cc(
        header_name="hlhdf.h",
        lib="hlhdf",
        includes=env.hlhdf_inc_dir,
        libpath=env.hlhdf_lib_dir,
        uselib_store="HLHDF",
        uselib="HDF5",
        mandatory=True,
    )

    _lib_path_opts_to_env(env, "fuse")
    conf.check_cc(
        header_name="fuse.h",
        lib="fuse",
        defines=["_FILE_OFFSET_BITS=64", "FUSE_USE_VERSION=27"],
        includes=env.fuse_inc_dir,
        libpath=env.fuse_lib_dir,
        uselib_store="FUSE",
        mandatory=env.build_bdbfs,
    )
    
    _lib_path_opts_to_env(env, "pthread")
    conf.check_cc(
        header_name="pthread.h",
        lib="pthread",
        includes=env.pthread_inc_dir,
        libpath=env.pthread_lib_dir,
        uselib_store="PTHREAD",
        mandatory=True,
    )
    
    # check for boost
    _lib_path_opts_to_env(env, "boost")
    conf.check_cxx(
        header_name="boost/system/config.hpp",
        lib="boost_system",
        includes=env.boost_inc_dir,
        libpath=env.boost_lib_dir,
        uselib_store="BOOST_SYSTEM",
        mandatory=True,
    )

    conf.check_cxx(
        header_name="boost/filesystem.hpp",
        lib="boost_filesystem",
        uselib="BOOST_SYSTEM",
        includes=env.boost_inc_dir,
        libpath=env.boost_lib_dir,
        uselib_store="BOOST_FILESYSTEM",
        mandatory=True,
    )

    conf.check_cxx(
        header_name="boost/program_options.hpp",
        lib="boost_program_options",
        uselib="BOOST_PROGRAM_OPTIONS",
        includes=env.boost_inc_dir,
        libpath=env.boost_lib_dir,
        uselib_store="BOOST_PROGRAM_OPTIONS",
        mandatory=env.build_bdbtool,
    )

    conf.check_cxx(
        header_name="boost/thread.hpp",
        lib="boost_thread",
        uselib="BOOST_THREAD",
        includes=env.boost_inc_dir,
        libpath=env.boost_lib_dir,
        uselib_store="BOOST_THREAD",
        mandatory=True,
    )

    boost_headers = (
        "bimap.hpp",
        "foreach.hpp",
        "lexical_cast.hpp",
        "multi_index_container.hpp",
        "noncopyable.hpp",
        "scoped_ptr.hpp",
        "shared_ptr.hpp",
        "variant.hpp",
        "iterator/iterator_facade.hpp",
        "numeric/conversion/cast.hpp",
    )

    for header in boost_headers:
        conf.check_cxx(
            header_name="boost/" + header,
            includes=env.boost_inc_dir,
            uselib_store="BOOST",
            mandatory=True,
        )
    
    conf.check_cxx(
        msg="Checking for boost::spirit >= 2.2",
        fragment="""#include <boost/spirit/include/version.hpp>
                    #if SPIRIT_VERSION < 0x2020
                    #error
                    #endif
                    int main() { return 0; }""",
        includes=env.boost_inc_dir,
        mandatory=True,
    )
    
    if env.build_java:
        # check for jni
        _lib_path_opts_to_env(env, "jdk")
        conf.check_cc(
            header_name="jni.h",
            includes=[
                env.jdk_inc_dir,
                os.path.join(env.jdk_inc_dir, "linux")
            ],
            uselib_storage="JNI",
            mandatory=True,
        )

    # default flags for all variants
    env.append_unique("CXXFLAGS", ["-Wall", "-Wno-long-long", "-pedantic"])

    if Options.options.debug:
        env.append_unique("CXXFLAGS", ["-g", "-fno-inline-functions"])
    else:
        env.append_unique("CXXFLAGS", ["-O2", "-fno-strict-aliasing"])

    env.VERSION = VERSION
    env.test_db_dsns = _validate_test_db_dsns(Options.options.test_db_dsns, env)
    
    # set up environment for building tests
    testenv = env.copy()
    testenv.detach()
    # remove -pedantic flag (errors in gmock headers)
    testenv.CXXFLAGS.remove("-pedantic")
    # always add debugging information to tests
    testenv.append_unique("CXXFLAGS", "-g")
    conf.set_env_name("testenv", testenv)

    if env.build_bdbfs:
        conf.define("BDB_BUILD_BDBFS", 1);

    conf.write_config_header("src/brfc/buildconfig.h")

def build(bld):
    _build_shared_library(bld)

    if bld.env.build_bdbtool:
        _build_bdbtool(bld)

    if bld.env.build_java:
        _build_java_wrapper(bld)
    
    bld.install_files("${PREFIX}/share/baltrad-db/sql/postgresql",
                      bld.path.ant_glob("schema/postgresql/*.sql"))

def _build_shared_library(bld):
    sources = sorted(bld.path.ant_glob("src/brfc/**/*.cpp"))
    libs = [
        "BOOST", "BOOST_SYSTEM", "BOOST_FILESYSTEM", "BOOST_THREAD",
        "HDF5", "HLHDF",
        "PQXX",
        "PTHREAD",
    ]

    for src in bld.path.ant_glob("src/brfc/fuse/*.cpp"):
        sources.remove(src)
    for src in bld.path.ant_glob("src/brfc/tool/*.cpp"):
        sources.remove(src)

    bld.shlib(
        source=sources,
        target="brfc",
        includes="src",
        export_includes="src",
        use=libs,
        install_path="${PREFIX}/lib",
    )

def _build_bdbtool(bld):
    libs = ["brfc", "BOOST_PROGRAM_OPTIONS"]
    sources = bld.path.ant_glob("src/brfc/tool/*.cpp")

    if bld.env.build_bdbfs:
        sources.extend(bld.path.ant_glob("src/brfc/fuse/*.cpp"))
        libs.append("FUSE");
    else:
        sources.remove("src/brfc/tool/Mount.cpp")

    bld.shlib(
        source=sources,
        target="brfc-tool",
        includes="src",
        export_includes="src",
        use=libs,
        install_path="${PREFIX}/lib",
    )
    
    sources = sorted(bld.path.ant_glob("bin/bdbtool/**/*.cpp"))

    bld.program(
        source=sources,
        target="bdbtool",
        includes="bin src",
        use=["brfc", "brfc-tool",],
        install_path="${PREFIX}/bin",
    )

def _strlit(var):
    return "\"%s\"" % var

def _build_gtest_gmock_lib(bld):
    sources = bld.path.ant_glob("src/gmock-gtest-all/*.cc")

    bld.stlib(
        source=sources,
        target="gtest-gmock",
        env=bld.env_of_name("testenv").copy(),
        includes="src",
    )

def _build_gtest_tests(bld):
    sources = sorted(bld.path.ant_glob("test/brfc/**/*.cpp"))
    local_libs = ["brfc", "gtest-gmock"]
    
    if not bld.env.build_bdbtool:
        for src in bld.path.ant_glob("test/brfc/tool/*.cpp"):
            sources.remove(src)
        for src in bld.path.ant_glob("test/brfc/fuse/*.cpp"):
            sources.remove(src)
    else:
        if not bld.env.build_bdbfs:
            for src in bld.path.ant_glob("test/brfc/fuse/*.cpp"):
                sources.remove(src)
        local_libs.append("brfc-tool")

    bld.program(
        source=sources,
        target="test_runner",
        env=bld.env_of_name("testenv").copy(),
        includes="src test",
        use=local_libs,
        install_path=None,
    )

def _build_gtest_itests(bld):
    cfg_values = {
        "test_dsn_count": len(bld.env.test_db_dsns),
        "test_dsns": ",\n".join(map(_strlit, bld.env.test_db_dsns)),
        "test_schema_dir": _strlit(bld.path.find_dir("schema").abspath())
    }

    cfg_cpp = bld(
        features="subst",
        source = "itest/brfc/itest_config.cpp.in",
        target = "itest/brfc/itest_config.cpp",
        on_results=True,
    )

    cfg_cpp.dct = cfg_values

    cfg_hpp = bld(
        features="subst",
        source = "itest/brfc/itest_config.hpp.in",
        target = "itest/brfc/itest_config.hpp",
        on_results=True,
    )
    cfg_hpp.dct = cfg_values

    sources = sorted(bld.path.ant_glob("itest/brfc/**/*.cpp"))

    sources.insert(0, "test/brfc/test_common.cpp")
    sources.insert(0, "itest/brfc/itest_config.cpp")

    bld.program(
        source=sources,
        target="itest_runner",
        env=bld.env_of_name("testenv").copy(),
        includes="src test itest",
        use=[
            "BOOST", "BOOST_SYSTEM", "BOOST_FILESYSTEM",
            "HDF5", "HLHDF",
            "PQXX",
            "brfc", "gtest-gmock",
        ],
        install_path=None,
    )

def _build_java_wrapper(bld):
    bld.shlib(
        features="cxx cxxshlib",
        source="swig/fc.i",
        swig_flags="-c++ -java "
                   "-outdir swig/eu/baltrad/fc "
                   "-package eu.baltrad.fc",
        includes="src",
        use="brfc HLHDF BOOST JNI.H",
        target="brfc_java"
    )
        
    bld.add_group("compile_java_wrapper")
    
    bld(
        features="ant",
        buildfile="build.xml",
        properties={
            "waf.builddir": bld.path.get_bld().abspath(),
        },
        command="build",
        target="jbrfc.jar",
    )

    bld.install_files("${PREFIX}/share/baltrad-db/java", ["jbrfc.jar"])

    jbrfc = bld.path.find_or_declare("jbrfc.jar")
    
    bld.add_manual_dependency(jbrfc,
                              bld.path.find_resource("swig/fc.i"))
    
    for filename in bld.path.ant_glob("swig/java/**/*.java"):
        bld.add_manual_dependency(jbrfc, filename)

    bld.add_manual_dependency(jbrfc,
                              bld.path.find_resource("build.xml"))

def _build_java_tests(bld):
    bld.add_group()

    bld(
        features="ant",
        properties={
            "waf.builddir": bld.path.get_bld().abspath(),
        },
        command="build-tests",
        target="jbrfc_test.jar",
    )

    jbrfc_test = bld.path.find_or_declare("jbrfc_test.jar")

    for node in bld.path.ant_glob("test/java/**/*.java"):
        bld.add_manual_dependency(jbrfc_test, node)
    bld.add_manual_dependency(jbrfc_test,
                              bld.path.find_resource("build.xml"))
    
def _ant_testdb_properties(dsns):
    # pick out postgresql dsn if present
    for dsn in dsns:
        url = urlsplit(dsn)
        if url.scheme != "postgresql":
            continue
        jdbc_url = "".join(["jdbc:", url.scheme, "://", url.hostname, url.path])
        return {
            "db.url": jdbc_url,
            "db.username": url.username,
            "db.password": url.password,
            "db.test_db_dsn": dsn,
        }
    return {}

def _build_tests(bld):
    if not bld.env.build_tests:
        Logs.error("tests are not set to be built, "
                   "(re-)configure with '--build_tests=yes'")
        sys.exit(1)
    
    build(bld)
    _build_gtest_gmock_lib(bld)
    _build_gtest_tests(bld)
    _build_gtest_itests(bld)
    if bld.env.build_java:
        _build_java_tests(bld)

class test(Build.BuildContext):
    cmd = 'test'
    fun = '_test'
    help = "build & run tests"

def _test(bld):
    _build_tests(bld)
    _run_tests(bld)

class run_tests(Build.BuildContext):
    cmd = 'run_tests'
    fun = '_run_tests'

def _run_tests(bld):
    # set up LD_LIBRARY_PATH for running tests

    env=bld.env_of_name("testenv").copy()
    
    bdir = bld.path.get_bld().abspath()

    ld_path = os.environ.get("LD_LIBRARY_PATH", "").split(":")
    ld_path = [os.path.abspath(path) for path in ld_path if path]
    ld_path.insert(0, bdir)


    libs = [
       "BOOST_SYSTEM", "BOOST_FILESYSTEM",
       "HDF5", "HLHDF",
       "PQXX",
    ]

    for libname in libs:
        try:
            libpath = env["LIBPATH_%s" % libname][0]
        except LookupError:
            continue
        libpath = os.path.abspath(libpath)
        if libpath not in ld_path:
            ld_path.append(libpath)
    
    if not env["env"]:
        env.env = os.environ.copy()
    env.env["LD_LIBRARY_PATH"] = ":".join(ld_path)
     
    bld.add_group("run_gtest_tests")
    bld(
        rule="${SRC[0].abspath()} --gmock_catch_leaked_mocks=1 --gmock_verbose=error --gtest_output=xml:%s" % os.path.join(bdir, "test/reports/gtest.xml"),
        source="test_runner",
        use="brfc",
        env=env,
        always=True,
    )

    bld.add_group("run_gtest_itests")
    bld(
        rule="${SRC[0].abspath()} --gmock_catch_leaked_mocks=1 --gmock_verbose=error --gtest_output=xml:%s" % os.path.join(bdir, "test/reports/gitest.xml"),
        source="itest_runner",
        use="brfc",
        env=env,
        always=True,
    )

    if not env.build_java:
        return

    bld.add_group("run_simple_java_tests")

    props = {
        "waf.builddir": bld.path.get_bld().abspath(),
        "db.test_db_schema_dir": bld.path.find_dir("schema").abspath(),
    }

    bld(
        features="ant",
        command="execute-simple-tests",
        properties=props,
        env=env,
        always=True,
    )

    bld.add_group("run_database_java_tests")

    testdb_props = _ant_testdb_properties(bld.env.test_db_dsns)
    if testdb_props:
        props.update(testdb_props)
        bld(
            features="ant",
            command="execute-database-tests",
            properties=props,
            env=env,
            always=True,
        )


def doc(bld):
    Utils.exec_command("doxygen", cwd="doc")

# vim:filetype=python:et:ts=4:sw=4:
