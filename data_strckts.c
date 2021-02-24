#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*-----------------------lable table--------------------------*/


/*constans*/
const int MAX_SYMBOL_SIZE = 80;
const int MAX_ATTRIBUTES_SIZE = 25;
int lable_table_size;

/*line structure in the table.*/
typedef struct {
    char symbol[80];
    int value;
    char attribute[80];
}lable_table_line;

/* this method enter a new line to the lable table*/
lable_table_line ** add_line_to_lable_table(lable_table_line ** current, int new_value, char new_symbol[], char new_attribute[],int * lable_table_size)
{
	/*increasing table line*/
	lable_table_line ** temp_table;
	/*if no lable table - creat one*/
	if (!*lable_table_size) 
		temp_table = malloc(sizeof(lable_table_line *));
	
	else
		temp_table = realloc(current,  sizeof(lable_table_line *) * ((*lable_table_size)+1));

	if(!temp_table)
		{
			printf("no space in disk");
			return 0;
		}	
	
	/*making space for new line struct*/
	lable_table_line * temp_line = malloc(sizeof(lable_table_line));
	
	/*insert value of new line*/
	temp_line->value = new_value;
	strcpy(temp_line->symbol,new_symbol);
	strcpy(temp_line->attribute,new_attribute);
	/*putting new line in table*/
	temp_table[(*lable_table_size)] = temp_line;

	(*lable_table_size)++;

    return temp_table;
}

void print_lable_table(lable_table_line ** table,int * lable_table_size)
{
    int i;
	if (!*lable_table_size)
	{
		printf("lable table is empty\n");	
		return;	
	}
	printf("symbol\tvalue\tattribute\n");
	for (i = 0; i < *lable_table_size; i++)
	{
		printf("%s\t\t%d\t\t%s\n",table[i]->symbol,table[i]->value,table[i]->attribute);
	}
	return;    
}
void erase_lable_table(lable_table_line ** table,int * lable_table_size)
{
	int i;
	for (i = 0; i < *lable_table_size; i++)
	{
		free(table[i]);
	}
	free(table);
	*lable_table_size = 0;
	printf("lable table erased sucsesfully\n");
}

int main()
{
	int lable_table_size = 0;
    lable_table_line ** table_table;
	table_table = add_line_to_lable_table(table_table,34,"aaa","bbb",&lable_table_size);
	print_lable_table(table_table, &lable_table_size);	

	table_table = add_line_to_lable_table(table_table,23,"xxx","ddd",&lable_table_size);
	print_lable_table(table_table, &lable_table_size);	
	erase_lable_table(table_table,&lable_table_size);
	print_lable_table(table_table, &lable_table_size);	

    return  0;
}

/*------------------something else--------------------------*/
