// Copyright 2010, Camilo Aguilar. Cloudescape, LLC.
//require.paths.unshift('./spec/lib', './lib', './build/default/src');
require.paths.unshift('./spec/lib', './lib');
var sys     = require("sys");
var fs      = require("fs");
var path    = require("path");
var libvirt = require('libvirt');
require('jspec');

//Need it for jspec know how to read files
readFile = function(path) {
  var result;
  try {
    result = fs.readFileSync(path);
  } catch (e) {
    throw e;
  }
  return result;
}

;(function(spec) {
    if(spec) {
        var file = 'spec/spec.' + spec + '.js';
        path.exists(file, function(exists){
            if(!exists) {
                sys.puts(file + ": Spec file does not exists");        
            } else {
                JSpec.exec(file).run({ reporter: JSpec.reporters.Terminal }).report();
            }
        });
    } else {
        fs.readdir('spec/', function(error, files){
            for(file in files) {
                file = files[file];    
                if(file.match('^spec')) {
                    JSpec.exec('spec/'+file).run({ reporter: JSpec.reporters.Terminal }).report();
                } 
            }
        });    
    }
})(process.argv[2]);

