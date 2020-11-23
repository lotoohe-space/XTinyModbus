#include "RTOS_mem.h"
#include "RTOS_task.h"

__align(MALLOC_BLOCK_SIZE) uint8 malloc_mem[MALLOC_MEM_SIZE]={0};
uint16 manager_table[MALLOC_MANAGER_TABLE]={0};

void InitMem(void){
	uint32 i;
	for(i=0;i<MALLOC_MANAGER_TABLE;i++){
		manager_table[i]=0;
	}
}
///申请内存
static void* _malloc(uint32 size){
	uint32 i_need_block_num;//需要的block数量
	float f_nedd_blcok_num;
	uint32 i;
	uint32 find_block_num=0;//找到的空的块
	uint32 temp;
	uint8 flag=0;

	f_nedd_blcok_num=size/(float)MALLOC_BLOCK_SIZE;
	i_need_block_num=size/MALLOC_BLOCK_SIZE;	
	if(f_nedd_blcok_num>i_need_block_num){//向上进位
		i_need_block_num+=1;
	}
	for(i=0;i<MALLOC_MANAGER_TABLE;){
		if(manager_table[i]==0){
			find_block_num++;
			if(find_block_num==i_need_block_num){
				flag=1;
				break;				
			}
			i++;
		}else{
			find_block_num=0;
			i+=manager_table[i];
		}
	}
	if(flag!=1||i>=MALLOC_MANAGER_TABLE){//没有找到，或者超出了
		return NULL;
	}
	i-=i_need_block_num-1;
	for(temp=i;temp<i+i_need_block_num;temp++){
		if(temp==i){
			manager_table[i]=i_need_block_num;
		}else{
			manager_table[temp]=1;
		}
	}
	return (void*)(&(malloc_mem[MALLOC_BLOCK_SIZE*i]));	
}
static void _free(void* mem_addr){
	if(mem_addr==NULL){return ;}
	uint32 free_size;
	uint32 i_mem_offset=(uint32)mem_addr-(uint32)malloc_mem;
	uint32 i_manager_offset=i_mem_offset/MALLOC_BLOCK_SIZE;
	uint32 i;
	if(i_manager_offset>MALLOC_MANAGER_TABLE){
		return ;
	}
	free_size=manager_table[i_manager_offset];
	for(i=i_manager_offset;i<free_size+i_manager_offset;i++){
		manager_table[i]=0;
	}
}

void *OSMalloc(uint32 size){
	return _malloc(size);
}
void OSFree(void* mem){
	_free(mem);
}


