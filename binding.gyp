{
  'targets': [
    {
      'target_name': 'libvirt',
      'product_prefix': 'lib',
      'sources': [
        'src/node_libvirt.cc',
        'src/hypervisor.cc',
        'src/error.cc',
        'src/worker.cc',
        'src/interface.cc',
        'src/network.cc',
        'src/network_filter.cc',
        'src/node_device.cc',
        'src/secret.cc',
        'src/storage_pool.cc',
        'src/storage_volume.cc',
        'src/domain.cc',
        'src/libvirt_handle.cc',
        'src/event_impl.cc'
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
            '-std=c++11',
            '<!@(pkg-config --cflags libvirt)'
          ],
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_RTTI': 'YES'
          }
        }]
      ]
    }
  ]
}
