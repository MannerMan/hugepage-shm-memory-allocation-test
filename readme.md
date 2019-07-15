## hugepage-memory-allocator-test:

This application is a hacky merge of these two with few changes;
* https://github.com/torvalds/linux/blob/master/tools/testing/selftests/vm/hugepage-shm.c
* https://gist.github.com/tuxdna/5146001

Essentially this application allocates a configurable amount of Huge Pages, by default 4gb
(see LENGTH). It then proceedes to allocate regular memory in 100mb chunks until it fails.
This can be used for testing when configuring hugepages manually in combination with
vm.overcommit_memory=2, to check that memory boundaries works as expected.

All credit goes to the creators of respective applications linked above.
