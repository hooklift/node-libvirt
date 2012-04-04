require.paths.unshift('build/default/src');
var sys = require('sys');
var libvirt = require('libvirt');
var fixture = require('./lib/helper').fixture;

var Hypervisor = libvirt.Hypervisor;

var hypervisor = new Hypervisor('test:///default');

module.exports = {
    'should be located through its uuid': function(beforeExit, assert) {
        try {
            var secret = hypervisor.lookupSecretByUUID('8dbf92e0-657f-ad16-7ba9-861574f78941');
            assert.isDefined(secret.getValue());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should be located through its usage': function(beforeExit, assert) {
        try {
            var secret = hypervisor.lookupSecretByUsage(hypervisor.VIR_SECRET_USAGE_TYPE_VOLUME, 'usage-id');
            assert.isDefined(secret.getValue());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should define the secret using its xml description': function(beforeExit, assert) {
        try {
            var xml = fixture('secret.xml');
            assert.ok(hypervisor.defineSecret(xml));
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return its uuid': function(beforeExit, assert) {
        try {
            var uuid = secret.getUUID();
            assert.isDefined(uuid);
         } catch(error) {
           assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return its usage id': function(beforeExit, assert) {
        try {
            var usage_id = secret.getUsageId();
            assert.isDefined(usage_id);
            assert.isNotNull(usage_id);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return its usage type': function(beforeExit, assert) {
       try {
            var usage_type = secret.getUsageType();
            asssert.isDefined(usage_type);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return its value': function(beforeExit, assert) {
        try {
            assert.isDefined(secret.getValue());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should set a value': function(beforeExit, assert) {
        try {
            assert.ok(secret.setValue('secret-value'));
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should return its xml description': function(beforeExit, assert) {
        try {
            assert.match(secret.toXml(), /secret/);
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    },

    'should undefine the secret': function(beforeExit, assert) {
        try {
            assert.ok(secret.undefine());
        } catch(error) {
            assert.eql(error.code, error.VIR_ERR_NO_SUPPORT);
        }
    }
};

