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


