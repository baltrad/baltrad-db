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

#include <brfc/expr/AttributePrototypes.hpp>

#include <boost/foreach.hpp>
#include <boost/assign/list_inserter.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace expr {

AttributePrototypes
AttributePrototypes::default_odim_h5() {
    typedef std::map<String, Attribute::Type> NameTypeMap;
    NameTypeMap defns;

    boost::assign::insert(defns)
        ("Conventions",       Attribute::STRING)
        /***
         * Table 1
         * Mandatory top-level what header Attributes for all weather
         * radar files
         */
        ("what/object",       Attribute::STRING)
        ("what/version",      Attribute::STRING)
        ("what/date",         Attribute::DATE)
        ("what/time",         Attribute::TIME)
        ("what/source",       Attribute::STRING)
        /***
         * Table 4
         * where Attributes for polar data objects
         */
        ("where/lon",         Attribute::DOUBLE)
        ("where/lat",         Attribute::DOUBLE)
        ("where/height",      Attribute::DOUBLE)
        // Dataset specific
        ("where/elangle",     Attribute::DOUBLE)
        ("where/nbins",       Attribute::INT64)
        ("where/rstart",      Attribute::DOUBLE)
        ("where/rscale",      Attribute::DOUBLE)
        ("where/nrays",       Attribute::INT64)
        ("where/a1gate",      Attribute::INT64)
        // Sector specific
        ("where/startaz",     Attribute::DOUBLE)
        ("where/stopaz",      Attribute::DOUBLE)
        /***
         * Table 5
         * where Attributes for geographical image data Groups
         */
        ("where/projdef",     Attribute::STRING)
        ("where/xsize",       Attribute::INT64)
        ("where/ysize",       Attribute::INT64)
        ("where/xscale",      Attribute::DOUBLE)
        ("where/yscale",      Attribute::DOUBLE)
        ("where/LL_lon",      Attribute::DOUBLE)
        ("where/LL_lat",      Attribute::DOUBLE)
        ("where/UL_lon",      Attribute::DOUBLE)
        ("where/UL_lat",      Attribute::DOUBLE)
        ("where/UR_lon",      Attribute::DOUBLE)
        ("where/UR_lat",      Attribute::DOUBLE)
        ("where/LR_lon",      Attribute::DOUBLE)
        ("where/LR_lat",      Attribute::DOUBLE)
        /***
         * Table 6
         * where for cross-section data Group
         */
        // Common Attributes
        //("where/xsize",       Attribute::INT64) // dup
        //("where/ysize",       Attribute::INT64) // dup
        //("where/xscale",      Attribute::DOUBLE) // dup
        //("where/yscale",      Attribute::DOUBLE) // dup
        ("where/minheight",   Attribute::DOUBLE)
        ("where/maxheight",   Attribute::DOUBLE)
        // RHI specific
        //("where/lon",         Attribute::DOUBLE) // dup
        //("where/lat",         Attribute::DOUBLE) // dup
        ("where/az_angle",    Attribute::DOUBLE)
        ("where/angles",      Attribute::SEQUENCE)
        ("where/range",       Attribute::DOUBLE)
        // Cross section and side panel specific
        ("where/start_lon",   Attribute::DOUBLE)
        ("where/start_lat",   Attribute::DOUBLE)
        ("where/stop_lon",    Attribute::DOUBLE)
        ("where/stop_lat",    Attribute::DOUBLE)
        /***
         * Table 7
         * where for vertical profiles
         */
        //("where/lon",         Attribute::DOUBLE) // dup
        //("where/lat",         Attribute::DOUBLE) // dup
        //("where/height",      Attribute::DOUBLE) // dup
        ("where/levels",      Attribute::INT64)
        ("where/interval",    Attribute::DOUBLE)
        //("where/minheight",   Attribute::DOUBLE) // dup
        //("where/maxheight",   Attribute::DOUBLE) // dup
        /***
         * Table 8
         * how attributes for all objects
         */
        ("how/task",          Attribute::STRING)
        ("how/startepochs",   Attribute::INT64)
        ("how/endepochs",     Attribute::INT64)
        ("how/system",        Attribute::STRING)
        ("how/software",      Attribute::STRING)
        ("how/sw_version",    Attribute::STRING)
        ("how/zr_a",          Attribute::DOUBLE)
        ("how/zr_b",          Attribute::DOUBLE)
        ("how/kr_a",          Attribute::DOUBLE)
        ("how/kr_b",          Attribute::DOUBLE)
        ("how/simulated",     Attribute::BOOL)
        // Data from individual radars
        ("how/beamwidth",     Attribute::DOUBLE)
        ("how/wavelength",    Attribute::DOUBLE)
        ("how/rpm",           Attribute::DOUBLE)
        ("how/pulsewidth",    Attribute::DOUBLE)
        ("how/lowprf",        Attribute::INT64)
        ("how/highprf",       Attribute::INT64)
        // Polar data
        ("how/azmethod",      Attribute::STRING)
        ("how/binmethod",     Attribute::STRING)
        ("how/azangles",      Attribute::SEQUENCE)
        ("how/elangles",      Attribute::SEQUENCE)
        ("how/aztimes",       Attribute::SEQUENCE)
        // Cartesian images including composites
        ("how/angles",        Attribute::SEQUENCE)
        ("how/arotation",     Attribute::SEQUENCE)
        ("how/camethod",      Attribute::STRING)
        ("how/nodes",         Attribute::SEQUENCE)
        ("how/ACCnum",        Attribute::INT64)
        // Vertical profile specific
        ("how/minrange",      Attribute::DOUBLE)
        ("how/maxrange",      Attribute::DOUBLE)
        ("how/NI",            Attribute::DOUBLE)
        ("how/dealiased",     Attribute::INT64)
        // Quality
        ("how/pointaccEL",    Attribute::DOUBLE)
        ("how/pointaccAZ",    Attribute::DOUBLE)
        ("how/malfunc",       Attribute::BOOL)
        ("how/radar_msg",     Attribute::STRING)
        ("how/radhoriz",      Attribute::DOUBLE)
        ("how/MDS",           Attribute::DOUBLE)
        ("how/OUR",           Attribute::DOUBLE)
        ("how/Dclutter",      Attribute::SEQUENCE)
        ("how/comment",       Attribute::STRING)
        ("how/SQI",           Attribute::DOUBLE)
        ("how/CSR",           Attribute::DOUBLE)
        ("how/LOG",           Attribute::DOUBLE)
        ("how/VPRCorr",       Attribute::BOOL)
        ("how/freeze",        Attribute::DOUBLE)
        ("how/min",           Attribute::DOUBLE)
        ("how/max",           Attribute::DOUBLE)
        ("how/step",          Attribute::DOUBLE)
        ("how/levels",        Attribute::INT64)
        ("how/peakpwr",       Attribute::DOUBLE)
        ("how/avgpwr",        Attribute::DOUBLE)
        ("how/dynrange",      Attribute::DOUBLE)
        ("how/RAC",           Attribute::DOUBLE)
        ("how/BBC",           Attribute::BOOL)
        ("how/PAC",           Attribute::DOUBLE)
        ("how/S2N",           Attribute::DOUBLE)
        ("how/polarization",  Attribute::STRING)
        /***
         * Table 13
         * what Group for Dataset Objects
         */
        ("what/product",      Attribute::STRING)
        // XXX: wtf, this has varying type (double or sequence)
        //("what/prodpar",      Attribute::SEQUENCE)
        ("what/quantity",     Attribute::STRING)
        ("what/startdate",    Attribute::DATE)
        ("what/starttime",    Attribute::TIME)
        ("what/enddate",      Attribute::DATE)
        ("what/endtime",      Attribute::TIME)
        ("what/gain",         Attribute::DOUBLE)
        ("what/offset",       Attribute::DOUBLE)
        ("what/nodata",       Attribute::DOUBLE)
        ("what/undetect",     Attribute::DOUBLE)
        /***
         * Table 17
         * Eight-bit Image attributes.
         */
        ("CLASS",             Attribute::STRING)
        ("IMAGE_VERSION",     Attribute::STRING)
        
        /***
         * "virtual" attributes
         */
        ("file:uuid",         Attribute::STRING)
        ("file:hash",         Attribute::STRING)
        ("file:stored_at",    Attribute::DATETIME)
        ("what/source:name",  Attribute::STRING)
        ("what/source:node",  Attribute::STRING)
        ("what/source:WMO",   Attribute::STRING)
        ("what/source:RAD",   Attribute::STRING)
        ("what/source:ORG",   Attribute::STRING)
        ("what/source:PLC",   Attribute::STRING)
        ("what/source:CTY",   Attribute::STRING)
        ("what/source:CMT",   Attribute::STRING)
    ;

    
    AttributePrototypes prototypes;
    
    BOOST_FOREACH(const NameTypeMap::value_type& defn, defns) {
        prototypes.add(Attribute::create(defn.first, defn.second));
    }

    return prototypes;
}

bool
AttributePrototypes::has(const String& name) const {
    return prototypes_.find(name) != prototypes_.end();
}

void
AttributePrototypes::add(AttributePtr prototype) {
    if (has(prototype->name()))
        throw duplicate_entry(prototype->name().to_utf8());
    prototypes_.insert(std::make_pair(prototype->name(), prototype));

}

AttributePtr
AttributePrototypes::get(const String& name) const {
    PrototypeMap::const_iterator i = prototypes_.find(name);
    if (i == prototypes_.end())
        throw lookup_error("no prototype for attribute: "
                           + name.to_std_string());
    return Attribute::create(i->first, i->second->type());
}

void
AttributePrototypes::clear() {
    prototypes_.clear();
}

} // namespace expr
} // namespace brfc
