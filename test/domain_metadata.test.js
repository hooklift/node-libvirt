'use strict';

var libvirt = require('../lib'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    h = require('./lib/helper'),
    semver = require('semver'),
    expect = require('chai').expect,
    xpath = require('xpath'),
    Dom = require('xmldom').DOMParser;

var test = {};

function verifyMetadata(test, expectedFromApi, expectedFromXml, namespaceUri, done) {
    test.domain.getMetadata(libvirt.VIR_DOMAIN_METADATA_ELEMENT, namespaceUri, 0, function(err, xml) {
        if (expectedFromApi === null) {
            expect(err).to.exist;
            expect(xml).to.be.undefined;
        } else {
            expect(err).to.not.exist;
            xml = xml.replace(/"/g, "'");
            expect(xml).to.equal(expectedFromApi);
        }
        getMetadataFromXml(test, function(err, xml) {
            expect(err).to.not.exist;
            if (expectedFromXml === null) {
                expect(xml).to.be.undefined;
            } else {
                xml = xml.replace(/"/g, "'");
                expect(xml).to.equal(expectedFromXml);
            }
            done();
        });
    });
}

function getMetadataFromXml(test, callback) {
    test.domain.toXml(function(err, xml) {
        if (err) {
            callback(err);
        } else {
            var doc = new Dom().parseFromString(xml);
            var nodes = xpath.select("//metadata/*", doc);
            expect(nodes.length).to.be.below(2);
            xml = nodes.length === 1 ? nodes[0].toString() : undefined;
            callback(null, xml);
        }
    });
}

function testText(test, field, str, shouldPass, done) {

    var _without_this_str_is_null_when_passed_to_native_in_some_cases = 1;

    test.domain.setMetadata(field, str, null, null, 0, function(err) {
        if (err) {
            if (shouldPass) {
                expect(err).to.not.exist;
            } else {
                expect(err).to.exist;
            }
            done();
        } else {
            test.domain.getMetadata(field, null, 0, function(err, md) {
                // value not present is returned as "error" (and so undefined)
                if (str !== null) {
                    expect(err).to.not.exist;
                } else {
                    str = undefined;
                }
                expect(md).to.equal(str);
                done();
            });
        }
    });
}

describe('Domain', function() {
    before(function() {
        SegfaultHandler.registerHandler();
        return h.getLibVirtVersion()
        .then(function(version) {
            test.version = version;
        });
    });

    describe('metadata methods', function() {
        beforeEach(function(done) {
            test.hypervisor = new Hypervisor('test:///default');
            test.hypervisor.connect(function(err) {
                expect(err).to.not.exist;

                test.hypervisor.lookupDomainById(1, function(err, domain) {
                    expect(err).to.not.exist;
                    expect(domain).to.exist;
                    expect(domain._parent).to.exist;
                    test.domain = domain;
                    done();
                });
            });
        });

        afterEach(function(done) {
            test.hypervisor.disconnect(function(err) {
                expect(err).to.not.exist;
                done();
            });
        });

        it('should set domain element metadata', function(done) {
            if (semver.lt(test.version, '0.9.10')) { return done(); }
            var metadata1 = h.fixture("metadata1.xml");
            metadata1 = metadata1.trim();
            var metadata1_ns = h.fixture("metadata1_ns.xml");
            metadata1_ns = metadata1_ns.trim();
            test.domain.setMetadata(libvirt.VIR_DOMAIN_METADATA_ELEMENT, metadata1, "herp", "http://herp.derp/", 0,
                function(err) {
                    expect(err).to.not.exist;
                    verifyMetadata(test, metadata1, metadata1_ns, "http://herp.derp/", done);
                }
            );
        });

        it('should rewrite domain element metadata', function(done) {
            if (semver.lt(test.version, '0.9.10')) { return done(); }
            var metadata2 = h.fixture("metadata2.xml");
            metadata2 = metadata2.trim();
            var metadata2_ns = h.fixture("metadata2_ns.xml");
            metadata2_ns = metadata2_ns.trim();
            test.domain.setMetadata(libvirt.VIR_DOMAIN_METADATA_ELEMENT, metadata2, "blurb", "http://herp.derp/", 0,
                function(err) {
                    expect(err).to.not.exist;
                    verifyMetadata(test, metadata2, metadata2_ns, "http://herp.derp/", done);
                }
            );
        });

        it('should erase domain element metadata', function(done) {
            if (semver.lt(test.version, '0.9.10')) { return done(); }
            test.domain.setMetadata(libvirt.VIR_DOMAIN_METADATA_ELEMENT, null, "", "http://herp.derp/", 0,
                function(err) {
                    expect(err).to.not.exist;
                    verifyMetadata(test, null, null, "http://herp.derp/", done);
                }
            );
        });

        var titleTests = [
            [1, "qwert",		true  ]
            , [2, null, 		true  ]
            , [3, "blah", 		true  ]
            , [4, "qwe\nrt", 	false ]
            , [5, "", 			true  ]
            , [6, "qwert\n", 	false ]
            , [7, "\n",		false ]
        ];

        titleTests.forEach(function(tt) {
            it('should manipulate the title test#'+tt[0], function(done) {
                if (semver.lt(test.version, '0.9.10')) { return done(); }
                testText(test, libvirt.VIR_DOMAIN_METADATA_TITLE, tt[1], tt[2], done);
            });
        });

        var descriptionTests = [
            [1, "qwert\nqwert"]
            , [2, null]
            , [3, ""]
            , [4, "qwert"]
            , [5, "\n"]
            , [6, "qwert\n"]
            , [7, "\nqwert"]
        ];

        descriptionTests.forEach(function(tt) {
            it('should manipulate the description test#'+tt[0], function(done) {
                if (semver.lt(test.version, '0.9.10')) { return done(); }
                testText(test, libvirt.VIR_DOMAIN_METADATA_DESCRIPTION, tt[1], true, done);
            });
        });
    });
});
