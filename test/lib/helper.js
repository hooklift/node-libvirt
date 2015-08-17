'use strict';

var Promise = require('bluebird'),
    fs = require('fs'),
    cp = require('child_process');

module.exports = {
  fixture: function(file) {
    var fixture = fs.readFileSync(__dirname + '/../fixtures/' + file, 'utf8');
    return fixture;
  },
  getLibVirtVersion: function() {
    return new Promise(function(resolve, reject) {
      cp.exec('pkg-config --modversion libvirt', function(err, stdout, stderr) {
        if (!!err) return reject(err);
        resolve(stdout.trim());
      });
    });
  }
};

