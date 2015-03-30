'use strict';

var libvirt = require('../build/Release/libvirt'),
    Hypervisor = libvirt.Hypervisor,
    SegfaultHandler = require('segfault-handler'),
    fixture = require('./lib/helper').fixture,
    expect = require('chai').expect;

var test = {};
describe('Secret', function() {
  before(function() {
    SegfaultHandler.registerHandler();
  });

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

  describe('hypervisor methods', function() {
    it('should be located through its uuid', function()  {
      try {
        var secret = test.hypervisor.lookupSecretByUUID('8dbf92e0-657f-ad16-7ba9-861574f78941');
        expect(secret.getValue()).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should be located through its usage', function()  {
      try {
        var secret = test.hypervisor.lookupSecretByUsage(test.hypervisor.VIR_SECRET_USAGE_TYPE_VOLUME, 'usage-id');
        expect(secret.getValue()).to.exist;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });

    it('should define the secret using its xml description', function()  {
      try {
        var xml = fixture('secret.xml');
        expect(test.hypervisor.defineSecret(xml)).to.be.ok;
      } catch (error) {
        expect(error.code).to.equal(error.VIR_ERR_NO_SUPPORT);
      }
    });
  });

  describe('methods', function() {
    beforeEach(function() {
      var xml = fixture('secret.xml');
      expect(function() {
       test.secret = test.hypervisor.defineSecret(xml);
      }).to.throw([Error]);
    });

    afterEach(function() {
      test.secret = undefined;
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
});

