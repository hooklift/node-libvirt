{
  'targets': [
    {
      'target_name': 'node-libvirt',
      'sources': [
        'src/node_libvirt.cc',
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
      'conditions': [
        ['OS!="win"', {
          'link_settings': {
            'libraries': [
              '<!@(pkg-config --libs-only-l libvirt)'
            ]
          }
        }]
      ]
    }
  ]
}
