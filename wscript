srcdir = "."
blddir = "build"
APPNAME = "node-libvirt"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "node-libvirt"
  obj.source = "node-libvirt.cc"
