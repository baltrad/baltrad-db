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

SWIG_EXTS = ['.swig', '.i']

swig_str = '${SWIG} ${SWIGFLAGS} ${_CCINCFLAGS} ${_CXXINCFLAGS} ${_CCDEFFLAGS} ${_CXXDEFFLAGS} ${SRC}'
cls = Task.simple_task_type('swig', swig_str, color='BLUE', ext_in='.i .h', ext_out='.o .c .cxx', shell=False)

re_module = re.compile('%module(?:\s*\(.*\))?\s+(.+)', re.M)

re_1 = re.compile(r'^%module.*?\s+([\w]+)\s*?$', re.M)
re_2 = re.compile('%include "(.*)"', re.M)
re_3 = re.compile('#include "(.*)"', re.M)

def scan(self):
    "scan for swig dependencies, climb the .i files"

    env = self.env

    lst_src = []

    seen = []
    to_see = [self.inputs[0]]

    while to_see:
        node = to_see.pop(0)
        if node.id in seen:
            continue
        seen.append(node.id)
        lst_src.append(node)

        # read the file
        code = node.read(env)
        code = preproc.re_nl.sub('', code)
        code = preproc.re_cpp.sub(preproc.repl, code)

        # find .i files and project headers
        names = re_2.findall(code) + re_3.findall(code)
        for n in names:
            for d in self.generator.env.INC_PATHS + [node.parent]:
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

# provide additional language processing
swig_langs = {}
def swig(fun):
    swig_langs[fun.__name__.replace('swig_', '')] = fun

@swig
def swig_python(tsk):
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + '.py'))

@swig
def swig_ocaml(tsk):
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + '.ml'))
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + '.mli'))

@swig
def swig_java(tsk):
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + ".java"))
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + "JNI.java"))

@extension(SWIG_EXTS)
def i_file(self, node):
    tsk = self.create_task('swig')
    tsk.set_inputs(node)
    tsk.module = getattr(self, 'swig_module', None)

    if not tsk.module:
        # search the module name
        txt = node.read(self.env)
        m = re_module.search(txt)
        if not m:
            raise ValueError("could not find the swig module name")
        tsk.module = m.group(1)

    flags = self.to_list(getattr(self, 'swig_flags', []))

    if not '-outdir' in flags:
        flags.append('-outdir')
        flags.append('') # placeholder
        self.outdir_node = node.parent
    else:
        outdir = flags[flags.index('-outdir') + 1]
        self.outdir_node = self.path.ensure_dir_node_from_path(outdir)

    outdir = self.outdir_node.abspath(self.env) 
    idx = flags.index("-outdir") + 1
    flags[idx] = outdir
    Utils.check_dir(outdir)

    ext = '.swigwrap_%d.c' % self.idx
    if '-c++' in flags:
        ext += 'xx'
    out_node = node.parent.find_or_declare(tsk.module + ext)

    try:
        if '-c++' in flags:
            fun = self.cxx_hook
        else:
            fun = self.c_hook
    except AttributeError:
        raise Utils.WafError('No c%s compiler was found to process swig files' % ('-c++' in flags and '++' or ''))

    tsk.outputs.append(out_node)

    task = fun(out_node)
    task.set_run_after(tsk)

    if not '-o' in flags:
        flags.append('-o')
        flags.append(out_node.abspath(self.env))

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

def detect(conf):
    paths = [] # when left empty, uses SWIG, then searches on PATH
    if Options.options.swig:
        # force searching only in the user-supplied directory
        paths.append(os.path.dirname(Options.options.swig))
    conf.find_program('swig', var='SWIG', path_list=paths, mandatory=True)

def set_options(opt):
    grp = opt.get_option_group("--prefix") # configuration options group
    grp.add_option("--swig", action="store",
                   default=None,
                   help="SWIG executable")
