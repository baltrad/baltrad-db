#! /usr/bin/env python
# encoding: UTF-8
# Petar Forai
# Thomas Nagy 2008
#
# modified for baltrad-db by Sander Ernes

import os
import re
import Options, Task, Utils, Logs
from TaskGen import extension
from Configure import conf
import preproc

swig_str = '${SWIG} ${SWIGFLAGS} ${SWIGPATH_ST:INCPATHS} ${SRC}'
cls = Task.simple_task_type('swig', swig_str, color='BLUE', ext_in='.i .h', ext_out='.o .c .cxx', shell=False)

re_module = re.compile('%module(?:\s*\(.*\))?\s+(.+)', re.M)

re_1 = re.compile(r'^%module.*?\s+([\w]+)\s*?$', re.M)
re_2 = re.compile('%include ["<](.*)[">]', re.M)
re_3 = re.compile('#include ["<](.*)[">]', re.M)

def scan(self):
    "scan for swig dependencies, climb the .i files"
    env = self.env

    lst_src = []

    seen = []
    to_see = [self.inputs[0]]

    while to_see:
        node = to_see.pop(0)
        if node in seen:
            continue
        seen.append(node)
        lst_src.append(node)

        # read the file
        code = node.read()
        code = preproc.re_nl.sub('', code)
        code = preproc.re_cpp.sub(preproc.repl, code)

        # find .i files and project headers
        names = re_2.findall(code) + re_3.findall(code)
        for n in names:
            for d in self.generator.includes_nodes + [node.parent]:
                u = d.find_resource(n)
                if u:
                    to_see.append(u)
                    break
            else:
                Logs.warn('could not find %r' % n)

    # list of nodes this one depends on, and module name if present
    if Logs.verbose:
        Logs.debug('deps: deps for %s: %s' % (str(self), str(lst_src)))
    return (lst_src, [])
cls.scan = scan

@extension(".swig", ".i")
def i_file(self, node):
    gen_tsk = self.create_task('swig')
    gen_tsk.set_inputs(node)
    gen_tsk.module = getattr(self, 'swig_module', None)

    if not gen_tsk.module:
        # search the module name
        txt = node.read()
        m = re_module.search(txt)
        if not m:
            raise ValueError("could not find the swig module name")
        gen_tsk.module = m.group(1)

    flags = self.to_list(getattr(self, 'swig_flags', []))
    
    if '-outdir' in flags:
        idx = flags.index("-outdir")
        flags.pop(idx)
        outdir = self.path.get_bld().make_node(flags.pop(idx)).abspath()
    else:
        outdir = node.parent.get_bld().abspath()
    flags.append("-outdir")
    flags.append(outdir)
    Utils.check_dir(outdir)
    
    ext = '.swigwrap_%d.c' % self.idx
    if '-c++' in flags:
        ext += 'xx'
    out_node = node.parent.find_or_declare(gen_tsk.module + ext)

    try:
        if '-c++' in flags:
            compile_fun = self.cxx_hook
        else:
            compile_fun = self.c_hook
    except AttributeError:
        raise Utils.WafError('No c%s compiler was found to process swig files' % ('-c++' in flags and '++' or ''))

    gen_tsk.outputs.append(out_node)

    compile_tsk = compile_fun(out_node)
    compile_tsk.set_run_after(gen_tsk)

    if not '-o' in flags:
        flags.append('-o')
        flags.append(out_node.abspath())

    self.env.append_value('SWIGFLAGS', flags)

@conf
def check_swig_version(conf, minver=None):
    """Check for a minimum swig version like conf.check_swig_version('1.3.28')
    or conf.check_swig_version((1,3,28)) """
    reg_swig = re.compile(r'SWIG Version\s(.*)', re.M)

    swig_out = Utils.cmd_output('%s -version' % conf.env['SWIG'])

    swigver = [int(s) for s in reg_swig.findall(swig_out)[0].split('.')]
    if isinstance(minver, basestring):
        minver = [int(s) for s in minver.split(".")]
    if isinstance(minver, tuple):
        minver = [int(s) for s in minver]
    result = (minver is None) or (minver[:3] <= swigver[:3])
    swigver_full = '.'.join(map(str, swigver))
    if result:
        conf.env['SWIG_VERSION'] = swigver_full
    minver_str = '.'.join(map(str, minver))
    if minver is None:
        conf.check_message_custom('swig version', '', swigver_full)
    else:
        conf.check_message('swig version', '>= %s' % (minver_str,), result, option=swigver_full)
    return result

def configure(conf):
    paths = [] # when left empty, uses SWIG, then searches on PATH
    if Options.options.swig:
        # force searching only in the user-supplied directory
        paths.append(os.path.dirname(Options.options.swig))
    conf.find_program('swig', var='SWIG', path_list=paths, mandatory=True)
    conf.env.SWIGPATH_ST="-I%s"

def options(opt):
    grp = opt.add_option_group("SWIG options") # configuration options group
    grp.add_option("--swig", action="store",
                   default=None,
                   help="SWIG executable")
