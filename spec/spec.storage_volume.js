require.paths.unshift('build/default/src');
sys = require('sys');
libvirt = require('node-libvirt');

describe 'Libvirt'
    before_each
        // local access, default config
        //conn = new libvirt.Connect('test:///default') 

        // local access, custom config
        // conn = new libvirt.Connect('test:///path/to/driver/config.xml')
     
        // local access, default config, via daemon
        // conn = new libvirt.Connect('test+unix:///default')

        // remote access, TLS/x509
        // conn = new libvirt.Connect('test://example.com/default')
        
        // remote access, SASl/Kerberos
        // conn = new libvirt.Connect('test+tcp://example.com/default')
        
        // remote access, SSH tunnelled
        // conn = new libvirt.Connect('test+ssh://root@example.com/default')    
    end
     
    describe 'Storage Volume'
        it 'test'
            true.should_be true
        end
    end
end
