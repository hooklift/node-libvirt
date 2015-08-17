{
  'targets': [
    {
      'target_name': 'libvirt',
      'product_prefix': 'lib',
      'sources': [
        'src/domain.cc',
        'src/error.cc',
        'src/event_impl.cc',
        'src/hypervisor.cc',
        'src/interface.cc',
        'src/network.cc',
        'src/network_filter.cc',
        'src/nlv_async_worker.cc',
        'src/node_device.cc',
        'src/node_libvirt.cc',
        'src/secret.cc',
        'src/storage_pool.cc',
        'src/storage_volume.cc',
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
