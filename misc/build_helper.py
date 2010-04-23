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
    oldlibs = ctx.AppendLIBS(["hlhdf", "hdf5"])
    result = ctx.TryLink("\n".join(src), ".c")
    ctx.Result(result)
    ctx.SetLIBS(oldlibs)

    return result

def CheckQt(ctx):
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
    result, _ = ctx.TryRun("\n".join(src), ".cpp")
    ctx.Result(result)
    ctx.SetLIBS(oldlibs)
    return result

def CheckQtSqlDrivers(ctx):
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
    result, drivers = ctx.TryRun("\n".join(src), ".cpp")
    ctx.Result(drivers)
    ctx.SetLIBS(oldlibs)
    return drivers.split(" ")

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
