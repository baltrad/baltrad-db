/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <brfc/tool/Benchmark.hpp>

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <boost/timer.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DateTime.hpp>
#include <brfc/TimeDelta.hpp>

#include <brfc/db/Database.hpp>
#include <brfc/db/FileEntry.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/hl/HlFile.hpp>

namespace po = boost::program_options;

namespace brfc {
namespace tool {

Benchmark::Benchmark()
        : iterations_(1)
        , keep_(false)
        , infile_()
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

std::string
Benchmark::do_description() const {
    return "benchmark importing file(s) to the database";
}

void
Benchmark::do_help(std::ostream& out) const {
    out << optdesc_;
}

void
Benchmark::do_parse_args(const ArgVector& args) {
    // read args
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(args).
                  options(optdesc_).run(), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        throw value_error(e.what());
    }

    if (not vm.count("input-file")) {
        throw value_error("input-file not specified");
    }

    infile_ = vm["input-file"].as<std::string>();

    keep_ = vm.count("keep") > 0;
}

int
Benchmark::do_execute(db::Database& db) {
    boost::timer timer;

    // read the "template" file
    oh5::hl::HlFile f(infile_);
    double load_secs = timer.elapsed();

    DateTime dt(Date(3000, 1, 1));
    TimeDelta delta;
    delta.add_days(1);
    double store_secs = 0;
    boost::ptr_vector<db::FileEntry> stored_files;
    boost::progress_display progress(iterations_);

    for (int i=0; i < iterations_; ++i) {
        f.root().attribute("what/date")->value(oh5::Scalar(dt.date()));
        f.root().attribute("what/time")->value(oh5::Scalar(dt.time()));

        timer.restart();
        stored_files.push_back(db.store(f));
        store_secs += timer.elapsed();

        dt += delta;
        ++progress;
    }

    std::cout << "imported " << iterations_
              << " files in " << store_secs << " secs " << std::endl
              << "average: " << store_secs / iterations_ << " (store) + "
              << load_secs << " (load) = "
              << store_secs / iterations_ + load_secs
              << " (total) secs per file" << std::endl;

    if (keep_) {
        std::cout << "cleaning up" << std::endl;
        BOOST_FOREACH(const db::FileEntry& entry, stored_files) {
            db.remove(entry);
        }
    }

    return 0;
}

} // namespace tool
} // namespace brfc
