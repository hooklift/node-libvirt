var should = require('should');
var libvirt = require('../build/Release/libvirt');

describe('Hypervisor', function() {

    var Hypervisor = libvirt.Hypervisor;

    describe('Connection', function() {
        it('should open a connection', function(done) {
            var hypervisor = new Hypervisor('test:///default');
            hypervisor.connect(function(err) {
                should.not.exist(err);
                done();
            });
        });

        it('should open a readonly connection', function(done) {
            var hypervisor = new Hypervisor('test:///default', true);
            hypervisor.connect(function(err) {
                should.not.exist(err);
                done();
            });
        });
    });

    describe('Methods', function() {
        var hypervisor = new Hypervisor('test:///default');

        before(function(done) {
            hypervisor.connect(function(err) {
                if (err) return done(err);
                done();
            });
        });

        it('should return hypervisor capabilities', function(done) {
            hypervisor.getCapabilities(function(err, capabilities) {
                if (err) return done(err);
                should.exist(capabilities);
                done();
            });
        });
    });
});
