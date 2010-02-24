srcdir = "."
blddir = "build"
APPNAME = "node-libvirt"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_cfg(package='libvirt', args='--cflags --libs', atleast_version='0.7.0')
  
def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "node-libvirt"
  obj.source = "src/node_libvirt.cc"
  obj.uselib = "LIBVIRT"
