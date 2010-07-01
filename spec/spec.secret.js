require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

Hypervisor = libvirt.Hypervisor;

//test driver does not provide mechanisms to test Secret

describe 'Secret'
    before
        hypervisor = new Hypervisor('test:///default');
    end

    before_each
        //secret = hypervisor.lookupSecretByUUID('8dbf92e0-657f-ad16-7ba9-861574f78941');
    end

    after_each

    end

    after
        hypervisor.closeConnection();
    end

    it 'should be located through its uuid'
        try {
            var secret = hypervisor.lookupSecretByUUID('8dbf92e0-657f-ad16-7ba9-861574f78941');
            secret.getValue().should_not_be undefined
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should be located through its usage'
        try {
            var secret = hypervisor.lookupSecretByUsage(hypervisor.VIR_SECRET_USAGE_TYPE_VOLUME,
                                                    'usage-id');
            secret.getValue().should_not_be undefined
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should define the secret using its xml description'
        try {
            var xml = fixture('secret.xml');
            hypervisor.defineSecret(xml).should_be true
        } catch(error) {
            error.code.should_not_be undefined
            error.code.should_be error.VIR_ERR_NO_SUPPORT
        }
    end

    it 'should return its uuid'
        try {
            var uuid = secret.getUUID();
            uuid.should_not_be undefined
            uuid.should_not_be null
        } catch(error) {
            error.should_include 'ReferenceError'
        }

    end

    it 'should return its usage id'
        try {
            var usage_id = secret.getUsageId();
            usage_id.should_not_be undefined
            usage_id.should_not_be null
        } catch(error) {
            error.should_include 'ReferenceError'
        }
    end

    it 'should return its usage type'
        try {
            var usage_type = secret.getUsageType();
            usage_type.should_not_be undefined
            usage_type.should_not_be null
        } catch(error) {
            error.should_include 'ReferenceError'
        }
    end

    it 'should return its value'
        try {
            secret.getValue().should_not_be undefined
        } catch(error) {
            error.should_include 'ReferenceError'
        }
    end

    it 'should set a value'
        try {
            secret.setValue('secret-value').should_be true
        } catch(error) {
            error.should_include 'ReferenceError'
        }
    end

    it 'should return its xml description'
        try {
            secret.toXml().should_match 'secret'
        } catch(error) {
            error.should_include 'ReferenceError'
        }
    end

    it 'should undefine the secret'
        try {
            secret.undefine().should_be true
        } catch(error) {
            error.should_include 'ReferenceError'
        }
    end
end

