require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

describe 'Libvirt'
    shared_behaviors_for 'Domain'
        after_each
            conn.close();
        end
    
        it 'should define a Domain from its XML Description'
            var xml = fixture('domain1.xml')
            var domain = new libvirt.Domain(conn, xml);
        end        
    end

    describe 'Connection using test:///default'
        should_behave_like('Domain')

        before_each
            // local access, default config
            conn = new libvirt.Connection('test:///default')
        end
    end

    describe 'Connection using test+unix:///default via libvirt daemon'
        should_behave_like('Domain')

        before_each
            // local access, default config
            conn = new libvirt.Connection('test+unix:///default')
        end
    end

    describe 'Connection using qemu:///system'
        should_behave_like('Domain')

        before_each
            // local access, default config
            conn = new libvirt.Connection('qemu:///system')
        end
    end
    
end


