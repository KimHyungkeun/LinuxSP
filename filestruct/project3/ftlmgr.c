// 주의사항
// 1. blkmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blkmap.h에 정의되어 있지 않을 경우 본인이 만들어서 사용하면 됨

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"
// 필요한 경우 헤더 파일을 추가하시오.

int dd_read(int, char*);
int dd_write(int, char*);
int dd_erase(int);

extern FILE *devicefp;


int sparebuf[PAGES_PER_BLOCK * BLOCKS_PER_DEVICE]; //4 * 4 
char freeblock = DATABLKS_PER_DEVICE;


const int lsn_count = PAGES_PER_BLOCK * DATABLKS_PER_DEVICE; //4 * 3 
const int psn_count = PAGES_PER_BLOCK * DATABLKS_PER_DEVICE; //4 * 3 
const int lbn_count = DATABLKS_PER_DEVICE; //3 
const int pbn_count = BLOCKS_PER_DEVICE; //4 

int lbn_table[DATABLKS_PER_DEVICE];

int lbn;
int pbn;
int ppn;
int offset;
int tmp;


//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// main()함수에서 반드시 먼저 호출이 되어야 한다.
//


// address mapping table 생성 및 초기화 등을 진행
// mapping table에서 lbn과 pbn의 수는 blkmap.h에 정의되어 있는 DATABLKS_PER_DEVICE
// 수와 같아야 하겠지요? 나머지 free block 하나는 overwrite 시에 사용하면 됩니다.
// pbn 초기화의 경우, 첫 번째 write가 발생하기 전을 가정하므로 예를 들면, -1로 설정을
// 하고, 그 이후 필요할 때마다 block을 하나씩 할당을 해 주면 됩니다. 어떤 순서대로 할당하는지는
// 각자 알아서 판단하면 되는데, free block들을 어떻게 유지 관리할 지는 스스로 생각해 보기
// 바랍니다.
void ftl_open()
{
	

	for(int pbn = 0 ; pbn < lbn_count ; pbn++){
		lbn_table[pbn] = pbn; //number of lbn
		
	}

	
	for(int ppn = 0 ; ppn < (PAGES_PER_BLOCK * BLOCKS_PER_DEVICE) ; ppn++) {
		sparebuf[ppn] = -1;
	}


	return;
}

//
// file system이 ftl_write()를 호출하면 FTL은 flash memory에서 주어진 lsn과 관련있는
// 최신의 데이터(512B)를 읽어서 sectorbuf가 가리키는 곳에 저장한다.
// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
//
void ftl_read(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	lbn = lsn / PAGES_PER_BLOCK;
	pbn = lbn_table[lbn];
	offset = (lsn % PAGES_PER_BLOCK);
	ppn = offset + (PAGES_PER_BLOCK * pbn) ; //pure offset
	
	dd_read(ppn, pagebuf);
	strncpy(sectorbuf, pagebuf, SECTOR_SIZE);

	printf("%s, spare : %d, ppn : %d\n",sectorbuf,pagebuf[SECTOR_SIZE],ppn);	
	return;
}


//
// file system이 ftl_write()를 호출하면 FTL은 flash memory에 sectorbuf가 가리키는 512B
// 데이터를 저장한다. 당연히 flash memory의 어떤 주소에 저장할 것인지는 block mapping 기법을
// 따라야한다.
//

void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	lbn = lsn / PAGES_PER_BLOCK;
	pbn = lbn_table[lbn];
	offset = (lsn % PAGES_PER_BLOCK);
	ppn = offset + (PAGES_PER_BLOCK * pbn) ; //pure offset

	
	
	dd_read(ppn, pagebuf);


			if(pagebuf[SECTOR_SIZE] == -1) { //if user input data new
				//printf("SECTOR SIZE BUFFER : %d\n", pagebuf[SECTOR_SIZE]);	
				strncpy(pagebuf,sectorbuf, SECTOR_SIZE);		
				pagebuf[SECTOR_SIZE] = 0; //input spare area
				sparebuf[ppn] = 0;
				dd_write(ppn, pagebuf);
				memset(pagebuf,0,PAGE_SIZE);
			}

			else if(pagebuf[SECTOR_SIZE] == 0){ //if user overwrite data	
			//printf("Check\n");
			
				for(int i=0; i < PAGES_PER_BLOCK; i++) {
					if(i == offset){
						strncpy(pagebuf,sectorbuf, SECTOR_SIZE);
						pagebuf[SECTOR_SIZE] = 0;
						sparebuf[PAGES_PER_BLOCK * freeblock + i] = 0;
						dd_write(PAGES_PER_BLOCK * freeblock + i, pagebuf);
						sparebuf[pbn * PAGES_PER_BLOCK + i] = -1;
						
					}
					
					else {					
						dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf);
						dd_write(PAGES_PER_BLOCK * freeblock + i, pagebuf);
						sparebuf[PAGES_PER_BLOCK * freeblock + i] = sparebuf[pbn * PAGES_PER_BLOCK + i];
						sparebuf[pbn * PAGES_PER_BLOCK + i] = -1;
						
					}

				}

				dd_erase(lbn_table[lbn]);
				lbn_table[lbn] = freeblock;
				//printf("lbn_table[%d] : %d\n", lbn, lbn_table[lbn]);
				freeblock = pbn;
				//printf("Changed freeblock : %d\n", freeblock);
				
			}
	
	printf("freeblock : %d\n", freeblock);
	for(int idx = 0 ; idx < (PAGES_PER_BLOCK * BLOCKS_PER_DEVICE) ; idx++) {
		printf("%d ", sparebuf[idx]);
	}
	printf("\n\n");
	
	return;
}
