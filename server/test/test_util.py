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
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

def called_with(mock, *args, **kw):
    """has mock been called with these arguments
    """
    return (args, kw) in mock.call_args_list

def call_count_with(mock, args, kw):
    """number of times the mock was called with these arguments
    """
    return mock.call_args_list.count((args, kw))

def called_once_with(mock, *args, **kw):
    """mock is called exactly once with specified arguments
    """
    return call_count_with(mock, args, kw) == 1

def assert_called_once_with(mock, *args, **kw):
    """mock is called exactly once with specified arguments
    """
    call_count = call_count_with(mock ,args, kw)
    assert call_count == 1, (
        "expected %s to be called once with %s,\n"
        "called %s times" % (mock, (args, kw), call_count)
    )

def called_once(mock):
    """mock is called exactly once (with any arguments)
    """
    return mock.call_count == 1
