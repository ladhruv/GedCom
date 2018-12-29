#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashTable.h"
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
/*
this function checks if the tags are valid and the level number with
the tags are appropriate.
@returns: this returns a int indicating wheather it valid (1) or not vlaid (0)
@parameter: this function takes in int number and the char pointer 
*/
int checkifVaild (int num, char *str);
/*
* this functuon chekcs if the new level number matches the previous
* level number and it not off 
@param: takes in two integer variable
@returns 0 if not valid and 1 if valid
*/
int checkifnumisoff (int num, int prevnum);
/*
this function chekcs if the list head and tail null
or not 
@param: takes in the list
@return: it returns a bool value
*/
bool isListEmpty(List list);
/*
this function checks if the tag are uppercase or not
@param: takes in a char pointer
@return: returns 0 if not valid and 1 if valid 
*/
int checkforupper(char *restr);

/**Function to return the data from the key given.
 *@pre The hash table exists and has a table size and key
 *@param size of the table
 *@param key integer that represents a piece of data in the table (eg 35->"hello")
 *@return returns integer value representing the positon
 */
int hashFunction(size_t tableSize, char *key);

/**Function to return the data from the key given.
 *@pre The hash table exists
 *@param void * ptr to print whatever value
 *@return returns nothing because void
 */
void printData(void * ptr);

char* printIn(const GEDCOMobject* obj);
 
ErrorCode validateIndividual(List list);

ErrorCode validateEvents(List list);

ErrorCode validateField (List list);

void getChild(Individual *ind, List *list);

bool compareIndividualb(const void* first, const void* second);

char *writeEvent (List list);

char *writeField (List list);

char *GEDCOMjson (const GEDCOMobject* obj);

char* individualToJSON (const Individual* ind);

char* indListToJSON(const GEDCOMobject* obj);

void destroyData(void * data);
/**Function to return the data from the key given.
 *@pre The hash table exists
 *@param hashtable pointer
 *@return returns nothing because void
 */
void printForward(HTable *list);
GEDCOMerror addingInd (GEDCOMobject* obj, char *str, char *fileName);

