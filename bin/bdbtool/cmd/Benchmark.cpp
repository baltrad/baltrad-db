/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#include <bdbtool/cmd/Benchmark.hpp>

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <boost/timer.hpp>

#include <brfc/DateTime.hpp>
#include <brfc/FileCatalog.hpp>
#include <brfc/StringList.hpp>
#include <brfc/TimeDelta.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/test/TempH5File.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {
namespace cmd {

Benchmark::Benchmark()
        : iterations_(1)
        , optdesc_("benchmark options") {
    optdesc_.add_options()
        ("input-file", po::value<std::string>(),
                "input file")
        ("iterations,n", po::value<int>(&iterations_),
                "number of iterations, default: 1")
        ("keep",
                "do not clean up imported files")
    ;
}

void
Benchmark::do_help(std::ostream& out) const {
    out << optdesc_ << std::endl;
}

int
Benchmark::do_execute(FileCatalog& fc,
                      const std::vector<std::string>& args) {
    // read args
    po::variables_map vm;
    po::store(po::command_line_parser(args).
              options(optdesc_).run(), vm);
    po::notify(vm);
    
    if (not vm.count("input-file")) {
        help(std::cout);
        return 1;
    }
    
    // read the "template" file
    shared_ptr<oh5::File> f = oh5::File::from_filesystem(vm["input-file"].as<std::string>());

    DateTime dt(Date(3000, 1, 1));
    TimeDelta delta;
    delta.add_days(1);
    StringList stored_paths;
    boost::timer timer;
    double elapsed = 0;
    boost::progress_display progress(iterations_);

    for (int i=0; i < iterations_; ++i) {
        test::TempH5File tmp;
        f->root()->attribute("what/date")->value(Variant(dt.date()));
        f->root()->attribute("what/time")->value(Variant(dt.time()));
        tmp.write(*f);
        timer.restart();
        stored_paths.append(fc.catalog(tmp.path())->path());
        elapsed += timer.elapsed();
        dt += delta;
        ++progress;
    }

    std::cout << "imported " << iterations_
              << " files in " << elapsed << " secs (avg. "
              << elapsed / iterations_ << " secs per file) "
              << std::endl;

    if (vm.count("keep") == 0) {
        std::cout << "cleaning up" << std::endl;
        BOOST_FOREACH(const String& path, stored_paths) {
            fc.remove(path);
        }
    }

    return 0;
}

} // namespace cmd
} // namespace tool
} // namespace brfc
