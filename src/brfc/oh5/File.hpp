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

#ifndef BRFC_FILE_H
#define BRFC_FILE_H

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

#include <QtCore/QString>

class QDate;
class QTime;

namespace brfc {

namespace oh5 {

class Attribute;
class AttributeSpecs;
class Group;
class RootGroup;
class Source;

/**
 * @brief a HDF5 file conforming to ODIM_H5/V2_0 specification
 */
class File : public boost::noncopyable,
             public boost::enable_shared_from_this<File> {
  public:
    typedef std::vector<shared_ptr<Attribute> > AttributeVector;
    typedef std::vector<shared_ptr<const Attribute> > ConstAttributeVector;
    typedef std::vector<QString> StringVector;

    /**
     * @brief construct an empty File
     *
     * this is mainly useful for testing purposes
     */
    static shared_ptr<File> create();

    /**
     * @brief construct from physical file
     * @param path absolute path to the file
     * @param specs AttributeSpecs instance used for converter lookups
     * @throw fs_error if file can not be opened
     *
     * full paths of ignored attributes (not defined in specs) are stored
     * and accessible through ignored_attributes();
     */
    static shared_ptr<File>
    from_filesystem(const QString& path, const AttributeSpecs& specs);
    
    /**
     * @brief construct with mandatory attributes present
     * @param object /what/object
     * @param date /what/date
     * @param time /what/time
     * @param source /what/source
     * @param version /what/version
     *
     * this is the minimal "correct" file, given that parameters are
     * correctly formed.
     */
    static shared_ptr<File>
    minimal(const QString& object,
            const QDate& date,
            const QTime& time,
            const QString& source,
            const QString& version=QString::fromUtf8("H5rad 2.0"));

    /**
     * @brief destructor
     */
    ~File();

    /**
     * @brief get hold of Group at path
     * @param path path to Group, where elements are separated by '/'
     * @return pointer to requested Group if found
     *
     * group("path/to/group")
     * @{
     */
    shared_ptr<const Group> group(const QString& path) const;

    shared_ptr<Group> group(const QString& path);
    ///@}
    
    /**
     * @brief get the root group
     * @{
     */
    shared_ptr<const RootGroup> root() const { return root_; }

    shared_ptr<RootGroup> root() { return root_; }
    ///@}
    
    /**
     * @brief get the Source definiton
     * @{
     */
    shared_ptr<const Source> source() const {
        return source_;
    }

    shared_ptr<Source> source() {
        return source_;
    }
    ///@}

    void source(shared_ptr<Source> src);

    /**
     * @brief get full paths of invalid attributes (have no value)
     */
    StringVector invalid_attribute_paths() const;
    
    /**
     * @brief get invalid attributes attributes (have no value)
     * @{
     */
    AttributeVector invalid_attributes();

    ConstAttributeVector invalid_attributes() const;
    //@}

    /**
     * @{
     * @brief absolute file path
     */
    const QString& path() const {
        return path_;
    }

    void path(const QString& path) {
        path_ = path;
    }
    ///@}
     
    /**
     * @brief filename part of the file path
     */
    QString name() const;
    
    /**
     * @name mandatory attribute access shorthands
     * @{
     */

    /**
     * @brief access /what/object
     *
     * equivalent to
     * @c root()->child_attribute("what/object")->value().string()
     */
    QString what_object() const;

    /**
     * @brief access /what/date
     *
     * equivalent to
     * @c root()->child_attribute("what/date")->value().date()
     */
    QDate what_date() const;

    /**
     * @brief access /what/time
     *
     * equivalent to
     * @c root()->child_attribute("what/time")->value().time()
     */
    QTime what_time() const;

    /**
     * @brief access /what/source
     *
     * equivalent to
     * @c root()->child_attribute("what/source")->value().string()
     */
    QString what_source() const;
    ///@}

  protected:
    File();
    
  private:

    shared_ptr<RootGroup> root_;
    QString path_;
    shared_ptr<Source> source_;
};

} // namespace oh5
} // namespace brfc
#endif // BRFC_FILE_H
