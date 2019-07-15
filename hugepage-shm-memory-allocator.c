// SPDX-License-Identifier: GPL-2.0
/*
 * hugepage-memory-allocator-test:
 *
 * This application is a hacky merge of these two with few changes;
 * https://github.com/torvalds/linux/blob/master/tools/testing/selftests/vm/hugepage-shm.c
 * https://gist.github.com/tuxdna/5146001
 *
 * Essentially this application allocates a configurable amount of Huge Pages, by default 4gb
 * (see LENGTH). It then proceedes to allocate regular memory in 100mb chunks until it fails.
 * This can be used for testing when configuring hugepages manually in combination with
 * vm.overcommit_memory=2, to check that memory boundaries works as expected.
 * All credit goes to the creators of respective applications linked above
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#ifndef SHM_HUGETLB
#define SHM_HUGETLB 04000
#endif

#define LENGTH (4096UL*1024*1024)

#define dprintf(x)  printf(x)

/* Only ia64 requires this */
#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define SHMAT_FLAGS (SHM_RND)
#else
#define ADDR (void *)(0x0UL)
#define SHMAT_FLAGS (0)
#endif

int main(void)
{
	int shmid;
	unsigned long i;
	char *shmaddr;

	/* added for mem commit */
	const int KB = 1024; /* bytes */
	const int MB = 1024 * KB; /* bytes */
	const int GB = 1024 * MB; /* bytes */
	const long TB = 1024 * GB; /* bytes */
	long size = 0;
	void *p = NULL;

	int ctr;
	int j;
	int times = 100000;
	long sum = 0;
	/* end hack */

	shmid = shmget(2, LENGTH, SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W);
	if (shmid < 0) {
		perror("shmget");
		exit(1);
	}
	printf("shmid: 0x%x\n", shmid);

	shmaddr = shmat(shmid, ADDR, SHMAT_FLAGS);
	if (shmaddr == (char *)-1) {
		perror("Shared memory attach failure");
		shmctl(shmid, IPC_RMID, NULL);
		exit(2);
	}
	printf("shmaddr: %p\n", shmaddr);

	dprintf("Starting the writes:\n");
	for (i = 0; i < LENGTH; i++) {
		shmaddr[i] = (char)(i);
		if (!(i % (1024 * 1024)))
			dprintf(".");
	}
	dprintf("\n");

	dprintf("Starting the Check...");
	for (i = 0; i < LENGTH; i++)
		if (shmaddr[i] != (char)i) {
			printf("\nIndex %lu mismatched\n", i);
			exit(3);
		}
	dprintf("Done allocating HUGE PAGES.\n");
	dprintf("Begin allocation regular memory.\n");
	for(ctr = 0; ctr < times; ctr ++ ) {
	    size = 100 * MB;
	    //printf("Allocating %d bytes memory \n", size);
	    p = calloc(1, size);
	    //p = malloc(1, size);
	    if(p != NULL) {
	      //printf("SUCCESS.\n");
	      sum += size;

	      for(j=0; j<size; j++) {
		((char*)p)[j] = 1;
	      }
	      //printf("sum %ld \n", sum);
	      if(sum < (1*GB)) {
		printf("Total allocated so far: %ld MB\n", sum/MB);
	      } else {
		printf("Total allocated so far: %ld MB\n", sum/MB);
	      }
		//printf("begin new loop \n");
	    } else {
	      printf("FAIL.\n");
	      break;
	    }
	  }


        sleep(5);

	if (shmdt((const void *)shmaddr) != 0) {
		perror("Detach failure");
		shmctl(shmid, IPC_RMID, NULL);
		exit(4);
	}

	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}

