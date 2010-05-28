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
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

import os
import sys

# XXX: this is not a nice way to access it
_mod_vars = sys.modules["SCons.Variables"]

Variables = _mod_vars.Variables
BoolVariable = _mod_vars.BoolVariable
PathVariable = _mod_vars.PathVariable

def CheckBoostVersion(ctx, version):
    ctx.Message("Checking for Boost version >= %s... " % version)

    # Boost versions are in format major.minor.subminor
    v_arr = version.split(".")
    version_n = 0
    if len(v_arr) > 0:
        version_n += int(v_arr[0])*100000
    if len(v_arr) > 1:
        version_n += int(v_arr[1])*100
    if len(v_arr) > 2:
        version_n += int(v_arr[2])

    src = (
        "#include <boost/version.hpp>",
        "#if BOOST_VERSION < %d" % version_n,
        "#error Installed boost is too old!",
        "#endif",
        "int main() { return 0; }"
    )

    result = ctx.TryCompile("\n".join(src), ".cpp")
    ctx.Result(result)
    return result

def CheckHlhdf(ctx):
    src = (
        "#include \"hlhdf.h\"",
        "int main() { return 0; }"
    )
    ctx.Message("Checking for HLHDF... ")
    oldlibs = ctx.AppendLIBS(["hlhdf"])
    result = ctx.TryLink("\n".join(src), ".c")
    ctx.Result(result)
    ctx.SetLIBS(oldlibs)

    return result

def CheckAnt(ctx, executable):
    ctx.Message("Checking for ant executable... ")
    result = os.path.exists(executable)
    ctx.Result(result)
    return result

class Config(object):
    _custom_tests = {
       "CheckBoostVersion": CheckBoostVersion,
       "CheckHlhdf": CheckHlhdf,
       "CheckAnt": CheckAnt
    }

    def __init__(self, env, cache=".configvars.cache"):
        self.env = env.Clone()
        self.cfg = self.env.Configure(custom_tests=self._custom_tests,
                                      clean=False, help=False)
        self.cache = cache

        self.vars = Variables(cache)

        self.vars.AddVariables(
            BoolVariable("have_pqxx", "found pqxx library", False),
            BoolVariable("have_hlhdf", "found hlhdf library", False),
            BoolVariable("have_icu", "found icu library", False),
            BoolVariable("have_pqxx", "found pqxx library", False),
            BoolVariable("have_boost", "found boost library", False),
            BoolVariable("have_gtest", "found gtest library", False),
            BoolVariable("have_gmock", "found gmock library", False),
            BoolVariable("have_jni", "found jni library", False),
            PathVariable("have_ant_executable", "found ant executable",
                         "", PathVariable.PathAccept)
        )

        self.vars.Update(self.env)
        self._ld_library_path = env["ENV"].get("LD_LIBRARY_PATH", "")


    def _cleanenv(self):
        self.env["LIBS"] = []
        self.env["CPPPATH"] = []
        self.env["LIBPATH"] = []
        self.env["ENV"]["LD_LIBRARY_PATH"] = self._ld_library_path

    def _wrap_check(self, act, *args, **kw):
        return lambda : act(*args, **kw)
    
    def _conf_one(self, var, check, clean=True):
        ret = check()
        if clean:
            self._cleanenv();
        self.env[var] = ret
    
    def configure(self):
        # XXX: hack to force rebuilding of all conf checks
        sys.modules["SCons.SConf"].SetCacheMode("force")

        cfg = self.cfg
        env = self.env
        
        env.AppendUnique(CPPPATH="${pqxx_include_dir}",
                         LIBPATH="${pqxx_lib_dir}")
        env.AppendENVPath("LD_LIBRARY_PATH",
                          env.Dir("${pqxx_lib_dir}").abspath)
        self._conf_one("have_pqxx",
                       self._wrap_check(cfg.CheckLibWithHeader,
                                        "pqxx", "pqxx/pqxx", "c++"))

        env.AppendUnique(CPPPATH="${hdf5_include_dir}",
                         LIBPATH="${hdf5_lib_dir}")
        env.AppendENVPath("LD_LIBRARY_PATH",
                          env.Dir("${hdf5_lib_dir}").abspath)
        self._conf_one("have_hdf5",
                       self._wrap_check(cfg.CheckLibWithHeader,
                                        "hdf5", "hdf5.h", "c"))

        env.AppendUnique(CPPPATH="${hlhdf_include_dir}",
                         LIBPATH="${hlhdf_lib_dir}")
        env.AppendENVPath("LD_LIBRARY_PATH",
                          env.Dir("${hlhdf_lib_dir}").abspath)
        self._conf_one("have_hlhdf",
                       self._wrap_check(cfg.CheckHlhdf))

        env.AppendUnique(CPPPATH="${icu_include_dir}",
                         LIBPATH="${icu_lib_dir}")
        env.AppendENVPath("LD_LIBRARY_PATH",
                          env.Dir("${icu_lib_dir}").abspath)
        self._conf_one("have_icu", self._wrap_check(self._check_icu))

        env.AppendUnique(CPPPATH="${boost_include_dir}",
                         LIBPATH="${boost_lib_dir}")
        env.AppendENVPath("LD_LIBRARY_PATH",
                          env.Dir("${boost_lib_dir}").abspath)
        self._conf_one("have_boost", self._wrap_check(self._check_boost))
       
        env.AppendUnique(CPPPATH="${gtest_include_dir}",
                         LIBPATH="${gtest_lib_dir}")
        env.AppendENVPath("LD_LIBRARY_PATH",
                          env.Dir("${gtest_lib_dir}").abspath)
        self._conf_one("have_gtest",
                       self._wrap_check(cfg.CheckLibWithHeader,
                                        "gtest", "gtest/gtest.h", "c++"),
                       clean=False)
        self._conf_one("have_gmock",
                       self._wrap_check(cfg.CheckLibWithHeader,
                                        "gmock", "gmock/gmock.h", "c++"))
        
        env.AppendUnique(CPPPATH=["${jdk_include_dir}",
                                  "${jdk_include_dir}/linux"],
                         LIBPATH="${jdk_lib_dir}")
        env.AppendENVPath("LD_LIBRARY_PATH",
                          env.Dir("${jdk_lib_dir}").abspath)
        self._conf_one("have_jni",
                       self._wrap_check(cfg.CheckHeader, "jni.h"))

        self._conf_one("have_ant_executable",
                       self._wrap_check(cfg.CheckAnt,
                                        env.File("${ant_executable}").abspath))
        
        cfg.Finish()
    
    def _check_boost(self):
        rets = []
        rets.append(self.cfg.CheckBoostVersion("1.38"))

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

        for header in headers:
            rets.append(self.cfg.CheckCXXHeader("/".join(("boost", header))))

        rets.append(self.cfg.CheckLibWithHeader("boost_filesystem",
                                                "boost/filesystem.hpp",
                                                "c++"))

        return False not in map(bool, rets)
 
    def _check_icu(self):
        rets = []

        rets.append(self.cfg.CheckLibWithHeader("icuuc",
                                                "unicode/unistr.h",
                                                "c++"))
        rets.append(self.cfg.CheckLibWithHeader("icutu",
                                                "unicode/regex.h",
                                                "c++"))

        return False not in map(bool, rets)
    
    def save(self):
        self.vars.Save(self.cache, self.env)

    def is_configured(self):
        return os.path.exists(self.cache)
    
    def has_minimal_deps(self):
        env = self.env
        return False not in map(bool, (env["have_boost"],
                                       env["have_icu"],
                                       env["have_pqxx"],
                                       env["have_hlhdf"],
                                       env["have_hdf5"]))
    
    def has_java_deps(self):
        env = self.env
        return False not in map(bool, (env["have_jni"],
                                       env["have_ant_executable"]))
    
    def has_test_deps(self):
        return bool(self.env["have_gtest"])
    
    def has_sql_dialect(self, dialect):
        if dialect == "postgresql" and self.env["have_pqxx"]:
            return True
        return False


# from python2.5
class BaseResult(tuple):
    """Base class for the parsed result objects.

    This provides the attributes shared by the two derived result
    objects as read-only properties.  The derived classes are
    responsible for checking the right number of arguments were
    supplied to the constructor.

    """

    __slots__ = ()

    # Attributes that access the basic components of the URL:

    @property
    def scheme(self):
        return self[0]

    @property
    def netloc(self):
        return self[1]

    @property
    def path(self):
        return self[2]

    @property
    def query(self):
        return self[-2]

    @property
    def fragment(self):
        return self[-1]

    # Additional attributes that provide access to parsed-out portions
    # of the netloc:

    @property
    def username(self):
        netloc = self.netloc
        if "@" in netloc:
            userinfo = netloc.split("@", 1)[0]
            if ":" in userinfo:
                userinfo = userinfo.split(":", 1)[0]
            return userinfo
        return None

    @property
    def password(self):
        netloc = self.netloc
        if "@" in netloc:
            userinfo = netloc.split("@", 1)[0]
            if ":" in userinfo:
                return userinfo.split(":", 1)[1]
        return None

    @property
    def hostname(self):
        netloc = self.netloc
        if "@" in netloc:
            netloc = netloc.split("@", 1)[1]
        if ":" in netloc:
            netloc = netloc.split(":", 1)[0]
        return netloc.lower() or None

    @property
    def port(self):
        netloc = self.netloc
        if "@" in netloc:
            netloc = netloc.split("@", 1)[1]
        if ":" in netloc:
            port = netloc.split(":", 1)[1]
            return int(port, 10)
        return None

class SplitResult(BaseResult):

    __slots__ = ()

    def __new__(cls, scheme, netloc, path, query, fragment):
        return BaseResult.__new__(
            cls, (scheme, netloc, path, query, fragment))

    def geturl(self):
        return urlunsplit(self)
