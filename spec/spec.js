require.paths.unshift('spec', './spec/lib', 'lib')
require('jspec')
require('spec.helper')
libvirt = require('libvirt')

JSpec
  .exec('spec/spec.js')
  .run({ reporter: JSpec.reporters.Terminal, fixturePath: 'spec/fixtures' })
  .report()
