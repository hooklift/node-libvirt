<a name="1.2.1"></a>
## [1.2.1](https://github.com/hooklift/node-libvirt/compare/v0.1.3...v1.2.1) (2017-01-09)


### Bug Fixes

* **build:** fix build on OSX ([775b114](https://github.com/hooklift/node-libvirt/commit/775b114))
* **copy-pasta:** Fix minor copy-paste issues ([7f711a1](https://github.com/hooklift/node-libvirt/commit/7f711a1))
* **domain:** update virt version check for domain block jobs ([f5ba188](https://github.com/hooklift/node-libvirt/commit/f5ba188))
* **domain:** use reference instead of copy ([09a9ff9](https://github.com/hooklift/node-libvirt/commit/09a9ff9))
* **error:** add proper context to error messages ([c9f159e](https://github.com/hooklift/node-libvirt/commit/c9f159e))
* **error:** keep copy of error object for later references ([57e20f4](https://github.com/hooklift/node-libvirt/commit/57e20f4))
* **errors:** avoid crashing if the error message is NULL ([526d924](https://github.com/hooklift/node-libvirt/commit/526d924))
* **errors:** include new error for 1.2.21 on osx, update tests ([0341bff](https://github.com/hooklift/node-libvirt/commit/0341bff))
* **GetAllDomainStats:** add missing HandleScope ([cd11ea8](https://github.com/hooklift/node-libvirt/commit/cd11ea8))
* **Hypervisor:** close connection on disconnect ([c843ed1](https://github.com/hooklift/node-libvirt/commit/c843ed1))
* **interface:** bug in response for Undefine ([23267e4](https://github.com/hooklift/node-libvirt/commit/23267e4))
* **invocation:** resolve issues discovered on a clang build ([00c634d](https://github.com/hooklift/node-libvirt/commit/00c634d))
* **migration:** support flags defined as an integer ([c81e718](https://github.com/hooklift/node-libvirt/commit/c81e718)), closes [#86](https://github.com/hooklift/node-libvirt/issues/86)
* **NLVObject:** fix tests and compiler warnings ([705cd6d](https://github.com/hooklift/node-libvirt/commit/705cd6d))
* **osx:** fix build on OSX ([7a94b69](https://github.com/hooklift/node-libvirt/commit/7a94b69))
* **test:** linting error in domain_metadata tests ([e9210fb](https://github.com/hooklift/node-libvirt/commit/e9210fb))
* **tests:** add clearing of children, reenable tests ([5e8f684](https://github.com/hooklift/node-libvirt/commit/5e8f684))
* **tests:** temporarily disable tests that cause segfaults ([13ad949](https://github.com/hooklift/node-libvirt/commit/13ad949))


### Features

* **agentLifecycle:** add Domain agent lifecycle event emission ([d7576a8](https://github.com/hooklift/node-libvirt/commit/d7576a8))
* **block:** added blockCommit, blockJobInfo, and blockJobAbort ([b2fdc02](https://github.com/hooklift/node-libvirt/commit/b2fdc02))
* **GetAllDomainStats:** add support for virGetAllDomainStats ([edae05f](https://github.com/hooklift/node-libvirt/commit/edae05f))
* **libvirt-handler:** refactored LibVirtHandler into own class ([910ba3c](https://github.com/hooklift/node-libvirt/commit/910ba3c))
* **NLVAsyncWorker:** create new async worker, migrate (dis)connect ([87d2036](https://github.com/hooklift/node-libvirt/commit/87d2036))
* **NLVObject:** make all class inherit NLVObject ([9b09ea6](https://github.com/hooklift/node-libvirt/commit/9b09ea6))
* **promises:** Export promisifed objects and add createHypervisor ([451a034](https://github.com/hooklift/node-libvirt/commit/451a034))
* **semver:** add checks for libvirt version in tests ([d035f6b](https://github.com/hooklift/node-libvirt/commit/d035f6b))
* **StoragePool:** implement lookupStoragePoolByVolume ([2abb5a9](https://github.com/hooklift/node-libvirt/commit/2abb5a9))
* **worker:** Added LookupInstanceByValueWorker class ([fc1b615](https://github.com/hooklift/node-libvirt/commit/fc1b615))



<a name="1.2.0"></a>
# [1.2.0](https://github.com/hooklift/node-libvirt/compare/v0.1.3...v1.2.0) (2016-08-23)


### Bug Fixes

* **build:** fix build on OSX ([775b114](https://github.com/hooklift/node-libvirt/commit/775b114))
* **copy-pasta:** Fix minor copy-paste issues ([7f711a1](https://github.com/hooklift/node-libvirt/commit/7f711a1))
* **domain:** use reference instead of copy ([09a9ff9](https://github.com/hooklift/node-libvirt/commit/09a9ff9))
* **error:** keep copy of error object for later references ([57e20f4](https://github.com/hooklift/node-libvirt/commit/57e20f4))
* **errors:** avoid crashing if the error message is NULL ([526d924](https://github.com/hooklift/node-libvirt/commit/526d924))
* **errors:** include new error for 1.2.21 on osx, update tests ([0341bff](https://github.com/hooklift/node-libvirt/commit/0341bff))
* **Hypervisor:** close connection on disconnect ([c843ed1](https://github.com/hooklift/node-libvirt/commit/c843ed1))
* **interface:** bug in response for Undefine ([23267e4](https://github.com/hooklift/node-libvirt/commit/23267e4))
* **osx:** fix build on OSX ([7a94b69](https://github.com/hooklift/node-libvirt/commit/7a94b69))
* **test:** linting error in domain_metadata tests ([e9210fb](https://github.com/hooklift/node-libvirt/commit/e9210fb))
* **tests:** add clearing of children, reenable tests ([5e8f684](https://github.com/hooklift/node-libvirt/commit/5e8f684))
* **tests:** temporarily disable tests that cause segfaults ([13ad949](https://github.com/hooklift/node-libvirt/commit/13ad949))


### Features

* **block:** added blockCommit, blockJobInfo, and blockJobAbort ([b2fdc02](https://github.com/hooklift/node-libvirt/commit/b2fdc02))
* **libvirt-handler:** refactored LibVirtHandler into own class ([910ba3c](https://github.com/hooklift/node-libvirt/commit/910ba3c))
* **NLVAsyncWorker:** create new async worker, migrate (dis)connect ([87d2036](https://github.com/hooklift/node-libvirt/commit/87d2036))
* **NLVObject:** make all class inherit NLVObject ([9b09ea6](https://github.com/hooklift/node-libvirt/commit/9b09ea6))
* **promises:** Export promisifed objects and add createHypervisor ([451a034](https://github.com/hooklift/node-libvirt/commit/451a034))
* **semver:** add checks for libvirt version in tests ([d035f6b](https://github.com/hooklift/node-libvirt/commit/d035f6b))
* **worker:** Added LookupInstanceByValueWorker class ([fc1b615](https://github.com/hooklift/node-libvirt/commit/fc1b615))



<a name="0.1.3"></a>
## 0.1.3 (2014-05-06)



