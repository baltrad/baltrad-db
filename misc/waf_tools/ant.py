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
import Constants
import Options
import Node
import Task
from TaskGen import feature

cls = Task.simple_task_type("exec_ant", "${ANT} -f ${ANT_BUILDFILE} ${ANT_PROPERTIES} ${ANT_COMMAND}", shell=True)

def _transform_properties(props, env):
    propstrs = []
    for k, v in props.iteritems():
        if isinstance(v, Node.Node):
            vstr = v.abspath(env)
        else:
            vstr = str(v)
        propstrs.append("-D%s=%s" % (k, vstr))
    return " ".join(propstrs)

@feature("ant")
def ant(self):
    """
    recognized attributes:
      command - ant target (default: "")
      target - files expected to be produced by the command
               (default: [])
      properties - dictionary to be transformed to java property
                   definitions (-Dkey=value). If value is a Node,
                   its absolute path is used. (default: {})
      buildfile - ant buildfile (default: build.xml from the top dir)
      always - should this task always run (default: False)
    """

    self.env = self.env.copy()

    self.env.append_value("ANT_COMMAND", getattr(self, "command", ""))
    
    propstr = _transform_properties(getattr(self, "properties", {}), self.env)
    self.env.append_value("ANT_PROPERTIES", propstr)

    buildfile = self.path.get_file(getattr(self, "buildfile", "build.xml"))
    self.env.append_value("ANT_BUILDFILE", buildfile.abspath())

    tsk = self.create_task("exec_ant")
    if getattr(self, "always", False):
        always_run = lambda s: Constants.RUN_ME
        new_rs = type(Task.TaskBase.run)(always_run, tsk, tsk.__class__)
        tsk.runnable_status = new_rs
    
    targets = self.to_list(getattr(self, "target", []))
    for target in targets:
        tsk.outputs.append(self.path.find_or_declare(target))

def set_options(opt):
    grp = opt.get_option_group("--prefix") # configuration options group
    grp.add_option("--ant", action="store",
                   help="ant executable [default: search from $PATH]")

def detect(conf):
    paths = [] # when left empty, uses SWIG, then searches on PATH
    if Options.options.ant:
        # force searching only in the user-supplied directory
        paths.append(os.path.dirname(Options.options.ant))
    conf.find_program("ant", var="ANT", path_list=paths, mandatory=True)
