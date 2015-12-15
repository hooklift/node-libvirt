'use strict';
var virt = require('../..'),
    fixture = require('../lib/helper').fixture;

// NOTE: you must run this manual test with `--expose-gc` to
//       trigger the bug!

// continuously run gc every 1ms
if (global.gc) setInterval(global.gc, 1);

var xml = fixture('storage_volume.xml');
var hv = new virt.Hypervisor('test:///default');
return hv.connectAsync()
  .then(function() { return hv.lookupStoragePoolByNameAsync('default-pool'); })
  .then(function(pool) { return pool.createVolumeAsync(xml); })
  .delay(2000)
  .then(function() { return hv.disconnectAsync(); })
  .then(function() { console.log('success'); })
  .catch(function(err) { console.log('error: ', err); });
