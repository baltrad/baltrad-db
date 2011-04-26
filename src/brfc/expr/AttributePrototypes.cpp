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

#include <brfc/expr/AttributePrototypes.hpp>

#include <boost/foreach.hpp>
#include <boost/assign/list_inserter.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace expr {

AttributePrototypes
AttributePrototypes::default_odim_h5() {
    PrototypeMap defns;

    boost::assign::insert(defns)
        ("Conventions",       STRING)
        /***
         * Table 1
         * Mandatory top-level what header Attributes for all weather
         * radar files
         */
        ("what/object",       STRING)
        ("what/version",      STRING)
        ("what/date",         DATE)
        ("what/time",         TIME)
        ("what/source",       STRING)
        /***
         * Table 4
         * where Attributes for polar data objects
         */
        ("where/lon",         DOUBLE)
        ("where/lat",         DOUBLE)
        ("where/height",      DOUBLE)
        // Dataset specific
        ("where/elangle",     DOUBLE)
        ("where/nbins",       INT64)
        ("where/rstart",      DOUBLE)
        ("where/rscale",      DOUBLE)
        ("where/nrays",       INT64)
        ("where/a1gate",      INT64)
        // Sector specific
        ("where/startaz",     DOUBLE)
        ("where/stopaz",      DOUBLE)
        /***
         * Table 5
         * where Attributes for geographical image data Groups
         */
        ("where/projdef",     STRING)
        ("where/xsize",       INT64)
        ("where/ysize",       INT64)
        ("where/xscale",      DOUBLE)
        ("where/yscale",      DOUBLE)
        ("where/LL_lon",      DOUBLE)
        ("where/LL_lat",      DOUBLE)
        ("where/UL_lon",      DOUBLE)
        ("where/UL_lat",      DOUBLE)
        ("where/UR_lon",      DOUBLE)
        ("where/UR_lat",      DOUBLE)
        ("where/LR_lon",      DOUBLE)
        ("where/LR_lat",      DOUBLE)
        /***
         * Table 6
         * where for cross-section data Group
         */
        // Common Attributes
        //("where/xsize",       INT64) // dup
        //("where/ysize",       INT64) // dup
        //("where/xscale",      DOUBLE) // dup
        //("where/yscale",      DOUBLE) // dup
        ("where/minheight",   DOUBLE)
        ("where/maxheight",   DOUBLE)
        // RHI specific
        //("where/lon",         DOUBLE) // dup
        //("where/lat",         DOUBLE) // dup
        ("where/az_angle",    DOUBLE)
        ("where/angles",      SEQUENCE)
        ("where/range",       DOUBLE)
        // Cross section and side panel specific
        ("where/start_lon",   DOUBLE)
        ("where/start_lat",   DOUBLE)
        ("where/stop_lon",    DOUBLE)
        ("where/stop_lat",    DOUBLE)
        /***
         * Table 7
         * where for vertical profiles
         */
        //("where/lon",         DOUBLE) // dup
        //("where/lat",         DOUBLE) // dup
        //("where/height",      DOUBLE) // dup
        ("where/levels",      INT64)
        ("where/interval",    DOUBLE)
        //("where/minheight",   DOUBLE) // dup
        //("where/maxheight",   DOUBLE) // dup
        /***
         * Table 8
         * how attributes for all objects
         */
        ("how/task",          STRING)
        ("how/startepochs",   INT64)
        ("how/endepochs",     INT64)
        ("how/system",        STRING)
        ("how/software",      STRING)
        ("how/sw_version",    STRING)
        ("how/zr_a",          DOUBLE)
        ("how/zr_b",          DOUBLE)
        ("how/kr_a",          DOUBLE)
        ("how/kr_b",          DOUBLE)
        ("how/simulated",     BOOL)
        // Data from individual radars
        ("how/beamwidth",     DOUBLE)
        ("how/wavelength",    DOUBLE)
        ("how/rpm",           DOUBLE)
        ("how/pulsewidth",    DOUBLE)
        ("how/lowprf",        INT64)
        ("how/highprf",       INT64)
        // Polar data
        ("how/azmethod",      STRING)
        ("how/binmethod",     STRING)
        ("how/azangles",      SEQUENCE)
        ("how/elangles",      SEQUENCE)
        ("how/aztimes",       SEQUENCE)
        // Cartesian images including composites
        ("how/angles",        SEQUENCE)
        ("how/arotation",     SEQUENCE)
        ("how/camethod",      STRING)
        ("how/nodes",         SEQUENCE)
        ("how/ACCnum",        INT64)
        // Vertical profile specific
        ("how/minrange",      DOUBLE)
        ("how/maxrange",      DOUBLE)
        ("how/NI",            DOUBLE)
        ("how/dealiased",     INT64)
        // Quality
        ("how/pointaccEL",    DOUBLE)
        ("how/pointaccAZ",    DOUBLE)
        ("how/malfunc",       BOOL)
        ("how/radar_msg",     STRING)
        ("how/radhoriz",      DOUBLE)
        ("how/MDS",           DOUBLE)
        ("how/OUR",           DOUBLE)
        ("how/Dclutter",      SEQUENCE)
        ("how/comment",       STRING)
        ("how/SQI",           DOUBLE)
        ("how/CSR",           DOUBLE)
        ("how/LOG",           DOUBLE)
        ("how/VPRCorr",       BOOL)
        ("how/freeze",        DOUBLE)
        ("how/min",           DOUBLE)
        ("how/max",           DOUBLE)
        ("how/step",          DOUBLE)
        ("how/levels",        INT64)
        ("how/peakpwr",       DOUBLE)
        ("how/avgpwr",        DOUBLE)
        ("how/dynrange",      DOUBLE)
        ("how/RAC",           DOUBLE)
        ("how/BBC",           BOOL)
        ("how/PAC",           DOUBLE)
        ("how/S2N",           DOUBLE)
        ("how/polarization",  STRING)
        /***
         * Table 13
         * what Group for Dataset Objects
         */
        ("what/product",      STRING)
        // XXX: wtf, this has varying type (double or sequence)
        //("what/prodpar",      SEQUENCE)
        ("what/quantity",     STRING)
        ("what/startdate",    DATE)
        ("what/starttime",    TIME)
        ("what/enddate",      DATE)
        ("what/endtime",      TIME)
        ("what/gain",         DOUBLE)
        ("what/offset",       DOUBLE)
        ("what/nodata",       DOUBLE)
        ("what/undetect",     DOUBLE)
        /***
         * Table 17
         * Eight-bit Image attributes.
         */
        ("CLASS",             STRING)
        ("IMAGE_VERSION",     STRING)
        
        /***
         * "virtual" attributes
         */
        ("file:uuid",         STRING)
        ("file:hash",         STRING)
        ("file:stored_at",    DATETIME)
        ("what/source:_name",  STRING)
        ("what/source:WMO",   STRING)
        ("what/source:RAD",   STRING)
        ("what/source:ORG",   STRING)
        ("what/source:PLC",   STRING)
        ("what/source:CTY",   STRING)
        ("what/source:CMT",   STRING)
    ;
    
    AttributePrototypes prototypes;
    
    BOOST_FOREACH(const PrototypeMap::value_type& defn, defns) {
        prototypes.add(defn.first, defn.second);
    }

    return prototypes;
}

bool
AttributePrototypes::has(const std::string& name) const {
    return prototypes_.find(name) != prototypes_.end();
}

void
AttributePrototypes::add(const std::string& name, Type type) {
    if (has(name))
        throw duplicate_entry("duplicate prototype: " + name);
    prototypes_.insert(std::make_pair(name, type));

}

std::string
AttributePrototypes::typename_(const std::string& name) const {
    PrototypeMap::const_iterator i = prototypes_.find(name);
    if (i == prototypes_.end())
        throw lookup_error("no prototype for attribute: " + name);
    return typename_(i->second);
}

std::string
AttributePrototypes::typename_(Type type) const {
    switch (type) {
        case STRING:
            return "string";
        case INT64:
            return "int64";
        case DOUBLE:
            return "double";
        case BOOL:
            return "bool";
        case DATE:
            return "date";
        case TIME:
            return "time";
        case DATETIME:
            return "datetime";
        default:
            throw std::runtime_error("invalid attribute type");
    }
}

void
AttributePrototypes::clear() {
    prototypes_.clear();
}

} // namespace expr
} // namespace brfc
