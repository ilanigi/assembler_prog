#include "structs.h"
#include "clean.h"

void full_clean(label_table_line **label_table,label_table_line** extern_table, label_table_line** entry_table, memory_table_line** instruction_table,memory_table_line** data_table, func_node** hash_func_table, node** label_name_hash, int* label_table_size, int* data_table_size, int* instruction_table_size, int* extern_table_size,int* entry_table_size)
{
    
    erase_label_table(label_table,label_table_size);
	erase_label_table(extern_table,extern_table_size);
	erase_label_table(entry_table,entry_table_size);
    erase_memory_table(instruction_table,instruction_table_size);
	erase_memory_table(data_table,data_table_size);
	erase_func_hash_table(hash_func_table,FUNCS_AMOUNT);
	erase_hash_table(label_name_hash,HASH_TABLE_SIZE);
	return;

}

void semi_clean(node ** label_name_hash, int* label_table_size, int* data_table_size, int* instruction_table_size, int* extern_table_size, int* entry_table_size)
{
	*label_table_size = 0;
	*extern_table_size = 0;
	*instruction_table_size = 0;
	*data_table_size = 0;
	*entry_table_size = 0;
	erase_hash_table(label_name_hash,HASH_TABLE_SIZE);
	return;
}
