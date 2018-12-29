#include "hashTable.h"
#include "GEDCOMUtilities.h"
/*
 Author:Dhruv Lad	
 Date: Feb 2 2018
 This code was implemented in my Data structure class 
*/


//Create HNode/Initialize Hnode
HNode* createNode(char *key, void * data)
{
	HNode *ptr = NULL;
	ptr = malloc(sizeof(HNode));
	ptr->key = malloc(sizeof(char) * strlen(key)+1);
	strcpy(ptr->key,key);
	ptr->data = data;
	ptr->next = NULL;
    return ptr;
}

//Create table
HTable *createTable(size_t size, int (*hashFunction)(size_t tableSize, char *key),void (*destroyData)(void *data),void (*printData)(void *toBePrinted))
{
	HTable *hashtable = NULL;
	hashtable = malloc(sizeof(HTable));
	hashtable->size = size;
	hashtable->table = malloc(sizeof(HNode*) * size);
	for (int i = 0; i < size; i++){
		hashtable->table[i]=NULL;
	}

	return hashtable;

}

//Destroy all values in hash table
void destroyTable(HTable *hashTable)
{

	if(hashTable == NULL)
	{
		return;
	}

    //algo to delete everything in hash table
	else
	{
		int i;
		for(i = 0; i <  hashTable -> size; i++)
		{
			if (hashTable->table[i]){
				HNode * tempHNode;
				
				while(hashTable->table[i] != NULL){
					tempHNode = hashTable->table[i];
					hashTable->table[i] = hashTable->table[i]->next;
					free(tempHNode->key);
					free(tempHNode);
				}
				hashTable->table[i] = NULL;
			}

		}
		if (hashTable->table != NULL){
			free(hashTable->table);
		}
		if (hashTable != NULL){
			free(hashTable);
		}
	}
	
	
}

void insertData(HTable *hashTable, char *key, void *data)
{
	
	int index = hashFunction(hashTable->size,key);
	HNode *word = (HNode*) hashTable->table[index];

	if (word != NULL){
		while (word != NULL){
			if (strcmp((char*)word->key,key) == 0){
				return;
			}

		}
		word = word->next;
	}

	if (hashTable->size < index){
		return;
	}
	HNode *next = NULL;
	HNode *prevs = NULL;
	HNode *nodet = hashTable->table[index];

	while (nodet != NULL && nodet->key != NULL && strcmp(key,nodet->key) > 0){
		prevs = nodet;
		nodet = nodet->next;
	}

	if (nodet != NULL && nodet ->key != NULL && strcmp(key,nodet->key) == 0){
		
		free(nodet->data);
		nodet->data = NULL;
	}
	else {
		next = createNode(key,data);
		if (nodet == hashTable->table[index]){
			next ->next = nodet;
			hashTable->table[index] = next;
		}
		else if (nodet == NULL){
			
			prevs->next = next;
		}
		else{
			next->next = nodet;
			prevs ->next = next;
		}
	}
}

//remove data algorithm
void removeData(HTable *hashTable,char *key)
{
     if(hashTable != NULL)
     {
     	int index = hashTable -> hashFunction(hashTable -> size, key);

     	//HNode *temp = (HNode*) hashTable ->table[index]; 
     	
     	if(hashTable -> table[index] != NULL)
     	{
     		HNode * tempHNode = hashTable -> table[index];
     		HNode * prev = NULL;

     		while(tempHNode -> key != key)
     		{
     			prev = tempHNode;
     			tempHNode = tempHNode -> next;
     		}

     		if(tempHNode -> key == key)
     		{
     			if(prev == NULL)
     			{
     				hashTable -> table[index] -> next = tempHNode -> next;
     				free(tempHNode);

     				if(hashTable -> table[index] == NULL)
     				{
     					free(hashTable);
     				}

     				else
     				{
     					hashTable -> table[index] = hashTable -> table[index] -> next;

     				}
     			}

     			else
     			{
     				prev -> next = tempHNode -> next;
     				free(tempHNode);
     			}
     		}
     	}
     }
}

//hash function to generate position
int hashFunction(size_t tableSize, char *key)
{
    int len_of_key = strlen(key);
    
    int key_value = 0; 
	char *xnum = NULL;
	int j = 0;
	
	xnum = malloc(sizeof(char) *strlen(key-3)+10);
	
    for (int i = 0; i < len_of_key; i++) {
        char individual_letter = key[i];
        if(individual_letter == 'I' || individual_letter =='@'){
		
			int letter_number = (int) individual_letter;
			key_value = key_value + letter_number;
		}
		else if (individual_letter == 'F'){
			key_value = 10;
		}
		else{
			xnum[j] = key[i];
			j++;
		}
    }
    
    int num = atoi(xnum);
    key_value = key_value+num;
     	
    int index_value = (key_value % tableSize);
    return index_value;
}

//print all values
void printForward(HTable *list)
{
	if(list == NULL)
	{
		return;
	}

    int i;
	for(i = 0; i < list -> size; i++)
	{
		if(list -> table[i] != NULL)
		{
			//printf("%s\n", list -> table[i] -> data);
		}
	}


}
//Function to search for key and values
void * lookupData(HTable * hashTable, char *key)
{
//	int index;
//	index = 0;
	HNode *tempHNode = NULL;
    if(hashTable == NULL)
	{
		return NULL;
	}

	if(hashTable != NULL)
	{
		tempHNode = hashTable ->table[hashFunction(hashTable -> size, key)];
	}

	while(tempHNode != NULL)
	{
		if(strcmp((char*)tempHNode->key,key) == 0) 
		{
			return (void *) tempHNode;
		}

		tempHNode = tempHNode -> next;
	}

	return NULL;
}



void printData(void * ptr)
{
	char * data = (char * )ptr;

	printf("Your data is: %s\n", data);
}

void destroyData(void * data){
	char *dat = (char *) data;
	free(dat);
}





