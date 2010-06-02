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

import optparse
import os
import sys

import Build
import Configure
import Options
import Scripting
import Utils

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
            
def set_options(opt):
    grp = opt.get_option_group("--prefix") # configuration options group

    grp.remove_option("--srcdir")
    grp.remove_option("--download")

    #XXX: this should probably be eliminated and just use PREFIX
    grp.add_option("--install_root", action="store",
                   default="${PREFIX}/baltrad/db-${VERSION}",
                   help="installation root [default: '%default']")
    grp.add_option("--debug", action="store_true",
                   default=False,
                   help="compile with debugging information")
    grp.add_option("--build_java", action="callback",
                   callback=_store_bool, type="string", nargs=1,
                   default=True, metavar="BOOL",
                   help="build java wrappers (default: %default)")
    grp.add_option("--test", action="callback",
                   callback=_store_bool, type="string", nargs=1,
                   default=True, metavar="BOOL",
                   help="build & run tests")
    grp.add_option("--test_db_dsn", action="append",
                   dest="test_db_dsns", metavar="DSN",
                   default=[],
                   help="database to test against (specify multiple times "
                        "for different databases)")

    libnames = ("boost", "hdf5", "hlhdf", "gmock", "gtest", "icu", "jdk", "pqxx")
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
    suffixes = ("_dir", "_lib_dir", "_inc_dir")
    _opts_to_env(env, [libname + suffix for suffix in suffixes])
        

def configure(conf):
    conf.check_tool("compiler_cc")
    conf.check_tool("compiler_cxx")
    conf.check_tool("misc")

    env = conf.env

    env.build_java = Options.options.build_java
    env.test = Options.options.test
    
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
    )

    # check for hdf5
    _lib_path_opts_to_env(env, "hdf5")
    conf.check_cc(
        header_name="hdf5.h",
        lib="hdf5",
        includes=env.hdf5_inc_dir,
        libpath=env.hdf5_lib_dir,
        uselib_store="HDF5",
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
    )
    
    # check for libicu
    _lib_path_opts_to_env(env, "icu")
    conf.check_cxx(
        header_name=["unicode/unistr.h", "unicode/regex.h"],
        lib=["icuuc", "icutu"],
        includes=env.icu_inc_dir,
        libpath=env.icu_lib_dir,
        uselib_store="ICU",
    )
    
    # check for boost
    _lib_path_opts_to_env(env, "boost")
    conf.check_cxx(
        header_name="boost/system/config.hpp",
        lib="boost_system",
        includes=env.boost_inc_dir,
        libpath=env.boost_lib_dir,
        uselib_store="BOOST_SYSTEM",
    )

    conf.check_cxx(
        header_name="boost/filesystem.hpp",
        lib="boost_filesystem",
        uselib="BOOST_SYSTEM",
        includes=env.boost_inc_dir,
        libpath=env.boost_lib_dir,
        uselib_store="BOOST_FILESYSTEM",
    )

    boost_headers = (
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

    for header in boost_headers:
        conf.check_cxx(
            header_name="boost/" + header,
            includes=env.boost_inc_dir,
            uselib_store="BOOST",
        )
    
    if env.build_java:
        # check for jni
        _lib_path_opts_to_env(env, "jdk")
        conf.check_cc(
            header_name="jni.h",
            includes=env.jdk_inc_dir,
            uselib_storage="JNI"
        )

    if env.test:    
        # check for gtest
        _lib_path_opts_to_env(env, "gtest")
        conf.check_cxx(
            header_name="gtest/gtest.h",
            lib="gtest",
            includes=env.gtest_inc_dir,
            libpath=env.gtest_lib_dir,
            uselib_store="GTEST",
        )
        
        # check for gmock
        _lib_path_opts_to_env(env, "gmock")
        conf.check_cxx(
            header_name="gmock/gmock.h",
            lib="gmock",
            uselib="GTEST",
            includes=env.gmock_inc_dir,
            libpath=env.gmock_lib_dir,
            uselib_store="GMOCK",
        )
    
    # default flags for all variants
    env.append_unique("CXXFLAGS", ["-Wall", "-Wno-long-long", "-pedantic"])

    if Options.options.debug:
        env.append_unique("CXXFLAGS", ["-g", "-fno-inline-functions"])
    else:
        env.append_unique("CXXFLAGS", ["-O2", "-fno-strict-aliasing"])

    env.VERSION = VERSION
    env.install_root = Utils.subst_vars(Options.options.install_root, env)
    env.test_db_dsns = _validate_test_db_dsns(Options.options.test_db_dsns, env)
    
    # set up environment for building tests
    testenv = env.copy()
    testenv.detach()
    # remove -pedantic flag (errors in gmock headers)
    testenv.CXXFLAGS.remove("-pedantic")
    # always add debugging information to tests
    testenv.append_unique("CXXFLAGS", "-g")
    conf.set_env_name("testenv", testenv)

def build(bld):
    _build_shared_library(bld)

    env = bld.env

    if env.build_java:
        _build_java_wrapper(bld)

    if env.test:
        _build_gtest_tests(bld)
        if env.build_java:
            _build_java_tests(bld)
        _run_tests(bld)
    
    bld.install_files("${install_root}/sql/postgresql",
                      bld.path.ant_glob("schema/postgresql/*.sql"))

def _build_shared_library(bld):
    bld.add_group("build_shared_library")

    sources = sorted(bld.path.ant_glob("src/brfc/**/*.cpp").split(" "))
    bld(
        features="cxx cshlib",
        source=sources,
        target="brfc",
        includes="src",
        uselib=[
            "BOOST", "BOOST_SYSTEM", "BOOST_FILESYSTEM",
            "HDF5", "HLHDF",
            "ICU",
            "PQXX",
        ],
        install_path="${install_root}/lib",
    )

def _strlit(var):
    return "\"%s\"" % var

def _build_gtest_tests(bld):
    bld.add_group("build_gtest_tests")
    bld.use_the_magic()

    cfg_values = {
        "test_dsn_count": len(bld.env.test_db_dsns),
        "test_dsns": ",\n".join(map(_strlit, bld.env.test_db_dsns)),
        "test_schema_dir": _strlit(bld.path.find_dir("schema").abspath())
    }

    cfg_cpp = bld(
        features="subst",
        source = "test/brfc/config.cpp.in",
        target = "test/brfc/config.cpp",
        on_results=True,
    )

    cfg_cpp.dict = cfg_values

    cfg_hpp = bld(
        features="subst",
        source = "test/brfc/config.hpp.in",
        target = "test/brfc/config.hpp",
        on_results=True,
    )
    cfg_hpp.dict = cfg_values

    sources = sorted(bld.path.ant_glob("test/brfc/**/*.cpp").split(" "))
    sources.insert(0, "test/brfc/config.cpp")
    lib = bld(
        features="cxx cprogram",
        source=sources,
        target="test_runner",
        env=bld.env_of_name("testenv").copy(),
        includes="src test/brfc",
        uselib=[
            "BOOST", "BOOST_SYSTEM", "BOOST_FILESYSTEM",
            "GMOCK", "GTEST",
            "HDF5", "HLHDF",
            "ICU",
            "PQXX",
        ],
        uselib_local="brfc",
        install_path=None,
    )

def _build_java_wrapper(bld):
    bld.add_group("build_java_wrapper")

    swig_files = [
        "fc.i",
        "fc_expr.i",
        "fc_oh5.i",
        "fc_test.i"
    ]
    
    swig_targets = []

    for filename in swig_files:
        name = filename.split(".")[0]
        elms = name.split("_")

        outdir = "swig/eu/baltrad/" + "/".join(elms)
        package = "eu.baltrad." + ".".join(elms)
        target = name + "_gen"
        swig_targets.append(target)

        bld(
            features="cxx",
            source="swig/" + filename,
            swig_flags="-c++ -java -outdir %s -package %s" % (outdir, package),
            includes="src",
            uselib="HLHDF BOOST JNI.H",
            target=target,
        )
        
    bld.add_group("link_java_wrapper")

    t = bld(
        features="cxx cshlib",
        target="brfc_java",
        add_objects=swig_targets,
        includes="src",
        uselib="BOOST JNI.H",
        uselib_local="brfc",
        install_path="${install_root}/lib",
    )

    bld.add_group("compile_java_wrapper")
    
    bld(
        features="ant",
        buildfile="build.xml",
        properties={
            "waf.builddir": bld.path,
        },
        command="build",
        target="jbrfc.jar",
    )

    bld.install_files("${install_root}/java", ["jbrfc.jar"])

    jbrfc = bld.path.find_or_declare("jbrfc.jar")
    
    #XXX: the correct dependency should be files produced by swig
    for filename in swig_files:
        bld.add_manual_dependency(jbrfc,
                                  bld.path.find_resource("swig/" + filename))
    
    for filename in bld.path.ant_glob("swig/java/**/*.java").split():
        bld.add_manual_dependency(jbrfc,
                                  bld.path.find_resource(filename))

    bld.add_manual_dependency(jbrfc,
                              bld.path.find_resource("build.xml"))

def _build_java_tests(bld):
    bld.add_group()

    bld(
        features="ant",
        properties={
            "waf.builddir": bld.path,
        },
        command="build-tests",
        target="jbrfc_test.jar",
    )

    jbrfc_test = bld.path.find_or_declare("jbrfc_test.jar")

    for filename in bld.path.ant_glob("test/java/**/*.java").split():
        bld.add_manual_dependency(jbrfc_test,
                                  bld.path.find_resource(filename))
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

def _run_tests(bld):
    # set up LD_LIBRARY_PATH for running tests

    env=bld.env_of_name("testenv").copy()

    bdir = os.path.join(bld.bdir, bld.path.variant(env))

    ld_path = os.environ.get("LD_LIBRARY_PATH", "").split(":")
    ld_path = [os.path.abspath(path) for path in ld_path if path]
    ld_path.insert(0, bdir)

    libs = [
       "BOOST_SYSTEM", "BOOST_FILESYSTEM",
       "GMOCK", "GTEST",
       "HDF5", "HLHDF",
       "ICU",
       "PQXX",
    ]

    for libname in libs:
        libpath = env["LIBPATH_%s" % libname][0]
        libpath = os.path.abspath(libpath)
        if libpath not in ("/lib", "/usr/lib") and libpath not in ld_path:
            ld_path.append(libpath)
    
    if not env["env"]:
        env.env = {}
    env.env["LD_LIBRARY_PATH"] = ":".join(ld_path)
     
    bld.add_group("run_gtest_tests")
    bld(
        rule="${SRC} --gtest_output=xml:test/reports/gtest.xml",
        source="test_runner",
        uselib_local="brfc",
        env=env,
        always=True,
    )

    if not env.build_java:
        return

    bld.add_group("run_simple_java_tests")

    props = {
        "waf.builddir": bld.path,
        "db.test_db_schema_dir": bld.path.get_dir("schema").abspath(),
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
