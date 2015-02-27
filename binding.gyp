{
  'targets': [
    {
      'target_name': 'libvirt',
      'product_prefix': 'lib',
      'sources': [
        'src/worker.cc',
        'src/node_libvirt.cc',
        'src/event_impl.cc',
        'src/domain.cc',
        'src/error.cc',
        'src/hypervisor.cc',
        'src/interface.cc',
        'src/network.cc',
        'src/network_filter.cc',
        'src/node_device.cc',
        'src/secret.cc',
        'src/storage_pool.cc',
        'src/storage_volume.cc',
        'src/stream.cc',
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")"
      ],
      'conditions': [
        ['OS!="win"', {
          'link_settings': {
            'libraries': [
              '<!@(pkg-config --libs libvirt)'
            ]
          },
	  'cflags': [
	    '<!@(pkg-config --cflags libvirt)'
	  ],
        }]
      ]
    }
  ]
}
