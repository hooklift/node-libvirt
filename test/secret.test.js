'use strict';

var libvirt = require('../lib'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Secret', function() {
  before(function() {
    SegfaultHandler.registerHandler();
  });

  describe('hypervisor methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;
        done();
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        done();
      });
    });

    it('should be located through its uuid', function(done)  {
      var uuid = '8dbf92e0-657f-ad16-7ba9-861574f78941';
      test.hypervisor.lookupSecretByUUID(uuid, function(err, secret) {
        expect(err).to.exist;
        done();

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // secret.getValue(function(err, value) {
        //   expect(err).to.not.exist;
        //   expect(value).to.exist;
        //   done();
        // });
      });
    });

    it('should be located through its usage', function(done)  {
      // var usageType = test.hypervisor.VIR_SECRET_USAGE_TYPE_VOLUME;
      test.hypervisor.lookupSecretByUsage(1, 'usage-id', function(err, secret) {
        expect(err).to.exist;
        done();

        // NOTE: not supported by test driver
        // expect(err).to.not.exist;
        // secret.getValue(function(err, value) {
        //   expect(err).to.not.exist;
        //   expect(value).to.exist;
        //   done();
        // });
      });
    });

    it('should define the secret using its xml description', function(done)  {
      var xml = fixture('secret.xml');
      test.hypervisor.defineSecret(xml, function(err, secret) {
        expect(err).to.exist;
        done();

        // NOTE: not supported in test driver
        // expect(err).to.not.exist;
        // expect(secret).to.exist;
        // done();
      });
    });
  });

  /*
    NOTE: none of this will be supported by the test driver, port when can test

  describe('methods', function() {
    beforeEach(function(done) {
      test.hypervisor = new Hypervisor('test:///default');
      test.hypervisor.connect(function(err) {
        expect(err).to.not.exist;

        var xml = fixture('secret.xml');
        test.hypervisor.defineSecret(xml, function(err, secret) {
          expect(err).to.not.exist;
          expect(secret).to.exist;
          test.secret = secret;
          done();
        });

        done();
      });
    });

    afterEach(function(done) {
      test.hypervisor.disconnect(function(err) {
        expect(err).to.not.exist;
        test.secret = undefined;
        done();
      });
    });

    it('should return its uuid', function()  {
      try {
        var uuid = test.secret.getUUID();
        expect(uuid).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should return its usage id', function()  {
      try {
        var usage_id = test.secret.getUsageId();
        expect(usage_id).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should return its usage type', function()  {
      try {
        var usage_type = test.secret.getUsageType();
        expect(usage_type).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should return its value', function()  {
      try {
        expect(test.secret.getValue()).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should set a value', function()  {
      try {
        expect(test.secret.setValue('secret-value')).to.be.ok;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should return its xml description', function()  {
      try {
        expect(test.secret.toXml()).to.match(/secret/);
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should undefine the secret', function()  {
      try {
        expect(test.secret.undefine()).to.be.ok;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });
  });
  */

});

