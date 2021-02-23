/*------------------lable table--------------------------*/

/*constans*/
int MAX_SYMBOL_SIZE = 80;
int MAX_ATTRIBUTES_SIZE = 25

/*line structure in the table.*/
typedef struct {
    char symbol[MAX_SYMBOL_SIZE];
    int value;
    char attribute[MAX_ATTRIBUTES_SIZE];
}lable_table_line;

/* this method construct a new lable table. it shuold onlt be used once.*/

lable_table_line * make_label_table()
{
    return (lable_table_line *) malloc(sizeof(lable_table_line));      
}

/* this method enter a new line to the lable table*/
lable_table_line * make_new_label_table_line(lable_table_line * current)
{
    return (lable_table_line *) realloc(current,suzeof(current) + sizeof(lable_table_line))
}

/*------------------something else--------------------------*/




