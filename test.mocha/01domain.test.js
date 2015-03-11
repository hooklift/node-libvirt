var should = require('should');
var fixture = require('../test/lib/helper').fixture;
var libvirt = require('../build/Release/libvirt');

describe('Domain', function() {
    var hypervisor = new libvirt.Hypervisor('test:///default');

    before(function(done) {
        hypervisor.connect(function(err) {
            if (err) return done(err);
            done();
        });
    });

    it('should create a persistent Domain from its XML Description', function(done) {
        var xml = fixture('domain.xml');

        hypervisor.createDomain(xml, function(err, dom) {
            if (err) return done(err);
            dom.should.be.Object;
            dom.getName(function(err, name) {
                if (err) return done(err);
                name.should.be.equal("nodejs-test");
                done();
            });
        });
    });

    it('should lookup a domain by id', function(done) {
        hypervisor.lookupDomainById(1, function(err, dom) {
            if (err) return done(err);
            dom.should.be.Object;
            done();
        });
    });

    it('should lookup a domain by name', function(done) {
        hypervisor.lookupDomainByName("test", function(err, dom) {
            if (err) return done(err);
            dom.should.be.Object;
            done();
        });
    });

    describe('Methods', function() {
        var domain;

        // Get instance of domain #1
        before(function(done) {
            hypervisor.lookupDomainById(1, function(err, res) {
                if (err) return done(err);
                domain = res;
                done();
            });
        });

        // Start the domain if it has been shutdowned by a test.
        afterEach(function(done) {
            domain.isActive(function(err, res) {
                if (err) return done(err);
                if (!res) {
                    domain.start(function(err) {
                        if (err) return done(err);
                        done();
                    });
                } else {
                    done();
                }
            });
        });

        it('should return the id', function(done) {
            domain.getId(function(err, res) {
                if (err) return done(err);
                res.should.be.equal(1);
                done();
            });
        });

        it('should return the domain information', function(done) {
            domain.getInfo(function(err, res) {
                if (err) return done(err);

                res.state.should.be.equal(domain.VIR_DOMAIN_RUNNING);
                res.max_memory.should.be.equal(8388608);
                res.memory.should.be.equal(2097152);
                res.vcpus_number.should.be.equal(2);
                should.exist(res.cpu_time);
                should.ok(res.cpu_time);
                done();
            });
        });

        it('should return the name', function(done) {
            domain.getName(function(err, res) {
                if (err) return done(err);
                res.should.be.equal("test");
                done();
            });
        });

        it('should return the uuid', function(done) {
            domain.getUUID(function(err, res) {
                if (err) return done(err);
                should.exist(res);
                done();
            });
        });

        it('should indicate if autostart is enable', function(done) {
            domain.getAutostart(function(err, res) {
                if (err) return done(err);
                res.should.be.false;
                done();
            });
        });

        it('should enable or disable autostart', function(done) {
            domain.setAutostart(true, function(err) {
                if (err) return done(err);
                domain.getAutostart(function(err, res) {
                    if (err) return done(err);
                    res.should.be.true;

                    domain.setAutostart(false, function(err) {
                        if (err) return done(err);
                        domain.getAutostart(function(err, res) {
                            res.should.be.false;
                            done();
                        });
                    });
                });
            });
        });

        it('should return the operating system type', function(done) {
            domain.getOsType(function(err, res) {
                if (err) return done(err);
                res.should.be.equal("linux");
                done();
            });
        });

        it('should return maximum amount of physical memory allocated to a domain', function(done) {
            domain.getMaxMemory(function(err, res) {
                if (err) return done(err);
                res.should.be.equal(8388608);
                done();
            });
        });

        it('should change the maximum amount of physical memory allocated to a domain', function(done) {
            domain.setMaxMemory(512000, function(err) {
                if (err) return done(err);
                domain.getMaxMemory(function(err, res) {
                    if (err) return done(err);
                    res.should.be.equal(512000);
                    done();
                });
            });
        });

        it('should dynamically change the runtime amount of memory allocated to a domain', function(done) {
            domain.setMemory(256000, function(err) {
                if (err) return done(err);
                domain.getInfo(function(err, res) {
                    if (err) return done(err);
                    res.memory.should.be.equal(256000);
                    done();
                });
            });
        });

        it('should return the maximum number of virtual CPUs supported for the guest VM', function(done) {
            domain.getMaxVcpus(function(err, res) {
                if (err) return done(err);
                res.should.be.equal(2);
                done();
            });
        });

        it('should indicate whether the domain is active', function(done) {
            domain.isActive(function(err, res) {
                if (err) return done(err);
                res.should.be.true;
                done();
            });
        });

        it('should indicate whether the domain is persistent', function(done) {
            domain.isPersistent(function(err, res) {
                if (err) return done(err);
                res.should.be.true;
                done();
            });
        });

        it('should indicate whether the domain is updated', function(done) {
            domain.isUpdated(function(err, res) {
                if (err) return done(err);
                res.should.be.false;
                done();
            });
        });

        it('should reboot the domain', function(done) {
            domain.reboot(function(err) {
                if (err) return done(err);
                done();
            });
        });

        // Not supported by libvirt test driver
        /*it('should reset the domain', function(done) {
            domain.reset(function(err) {
                if (err) return done(err);
                done();
            });
        });*/

        it('should save and restore the domain', function(done) {
            var path = "/tmp/node-libvirt-saved.img";
            domain.save(path, function(err) {
                if (err) return done(err);
                hypervisor.restoreDomain(path, function(err) {
                    if (err) return done(err);
                    done();
                });
            });
        });

        it('should suspend and resume the domain', function(done) {
            domain.suspend(function(err) {
                if (err) return done(err);
                domain.resume(function(err) {
                    if (err) return done(err);
                    done();
                });
            });
        });

        it('should shutdown the domain', function(done) {
            domain.shutdown(function(err) {
                if (err) return done(err);
                done();
            });
        });


        it('should get information about vcpus', function(done) {
            domain.getVcpus(function(err, res) {
                if (err) return done(err);

                res.should.be.instanceof(Array);
                should.exists(res[0].number);
                should.exists(res[0].state);
                should.exists(res[0].cpu_time);
                should.exists(res[0].cpu);
                should.exists(res[0].affinity);

                var affinity = res[0].affinity;
                var real_cpu = 0; //pedagogical purpose
                should.exists(affinity[real_cpu].usable);

                done();
            });
        });

        it('should dynamically change the number of virtual CPUs used by the domain', function(done) {
            domain.setVcpus(1, function(err) {
                if (err) return done(err);
                done();
            });
        });

        it('should allow to change real CPUs, which can be allocated to a virtual CPU', function(done) {
            domain.getVcpus(function(err, res) {
                if (err) return done(err);

                var affinity = res[0].affinity;
                affinity[0].usable = false;
                affinity[1].usable = false;

                domain.pinVcpu(res[0].number, affinity, function(err) {
                    if (err) return done(err);
                    done();
                });
            });
        });

        it('should attach a device', function(done) {
            var xml = fixture('device.xml');

            //no supported by test driver
            domain.attachDevice(xml, function(err) {
                should.exists(err);
                err.code.should.be.equal(err.VIR_ERR_NO_SUPPORT)
                done();
            });
        });

        it('should detach a device', function(done) {
            var xml = fixture('device.xml');

            //no supported by test driver
            domain.detachDevice(xml, function(err) {
                should.exists(err);
                err.code.should.be.equal(err.VIR_ERR_NO_SUPPORT)
                done();
            });
        });

        it('should update a device', function(done) {
            var xml = fixture('device_update.xml');
            var flags = [libvirt.VIR_DOMAIN_DEVICE_MODIFY_CONFIG];

            //no supported by test driver
            domain.updateDevice(xml, flags, function(err) {
                should.exists(err);
                err.code.should.be.equal(err.VIR_ERR_NO_SUPPORT)
                done();
            });
        });

    });
});
