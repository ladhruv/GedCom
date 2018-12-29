/*
* Dhruv Lad
* 0928018 
* CIS 2750 - Dennis Nikitenko
*/
#include "GEDCOMparser.h"
#include "assert.h"
#include "GEDCOMUtilities.h"
#include "hashTable.h"

GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj){

	FILE *data;
	char buff[300];
	int filesiz;
	char* str;

	 GEDCOMerror gederr;

	 //checks if the filename is empty 
	if (fileName == NULL)
	{
		gederr.type = INV_FILE;
		gederr.line = -1;
		goto END;
	}

	//checks if the file ends with .ged
	filesiz = strlen(fileName);
	str = &fileName[filesiz-4];

	if (strcmp(str,".ged") != 0)
	{
		gederr.type = INV_FILE;
		gederr.line = -1;
		goto END;
	}

	//opens the file and reads it 
	data = fopen (fileName, "r");
	if (data == NULL)
	{
		gederr.type = INV_FILE;
		gederr.line = -1;
		goto END;
	}
	
	// mallocing and initializing for the array fields 
	char *fieldstr[50];
	for (int i = 0 ; i < 50; i++){
		fieldstr[i] = malloc(1 * sizeof(char));
		if(fieldstr[i] == NULL){
			gederr.type = OTHER_ERROR;
			gederr.line = -1;
			goto END;
		}
		strcpy(fieldstr[i],"\0");
	}

	char *fieldval [50];
	for (int i = 0 ; i < 50; i++){
		fieldval [i] = malloc(1 * sizeof(char));
		if(fieldval [i] == NULL){
			gederr.type = OTHER_ERROR;
			gederr.line = -1;
			goto END;
		}
		strcpy(fieldval[i],"\0");
	}

	char *eventfield[50];

	for (int i = 0; i < 50; i++){
		eventfield[i] = malloc(1 * sizeof(char));
		if(eventfield [i] == NULL){
			gederr.type = OTHER_ERROR;
			gederr.line = -1;
			goto END;
		}
		strcpy(eventfield[i],"\0");
	}

	char *submfield[50];

	for (int i = 0 ;i < 50; i++){
		submfield[i] = malloc(1 *sizeof(char));
		if(submfield [i] == NULL){
			gederr.type = OTHER_ERROR;
			gederr.line = -1;
			goto END;
		}
		strcpy(submfield[i],"\0");	
	}

	char *famfield[50];

	for (int i = 0; i < 50; i++){
		famfield[i] = malloc(1 * sizeof(char));
		if(famfield [i] == NULL){
			gederr.type = OTHER_ERROR;
			gederr.line = -1;
			goto END;
		}
		strcpy(famfield[i],"\0");		
	}

	// initilize the variables 
	bool headflag = false, sour = false, submf = false, charsf = false,gedf = false;
	bool indifalg = false, headerflag = false,submiteeflag = false,trlrflag = false;
	bool sumbflag= false;
	int chkval = 1;
	int srtnum = 0, prevnum = 0;
	char *namstr = NULL;
	int sizstr = 0;
	int linenum = 0;
	bool isnum = false, famflag = false;
	char *givna = NULL, *surn = NULL;
	int count = 0;
	char *agefield = NULL;
	char *x_ref = NULL, *ref= NULL;
	int n = 0 , e = 0, u = 0, p = 0;


	//create a tmperory gedcom object 
	GEDCOMobject *tmpobj = malloc(sizeof(GEDCOMobject));
	if (tmpobj == NULL){
		gederr.type = OTHER_ERROR;
		gederr.line = -1;
		goto END;
	}
	
	HTable *hash = malloc(sizeof(HTable));
	if (hash == NULL){
		gederr.type = OTHER_ERROR;
		gederr.line = -1;
		goto END;		
	}
	//initialize the list for inidividual and family 
	tmpobj->individuals = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
	tmpobj->families = initializeList(&printFamily,&deleteFamily,&compareFamilies);
	
	hash = createTable(1000,&hashFunction,&destroyData,&printData);
	
	while (fgets(buff,300,data) != NULL)
	{
		char *restr = NULL;
		char *trimstr = NULL;
		
		int checkref = 0;
		int strsizee = strlen(buff);

		// checks if the line is greater than 255 
		if (strsizee > 255){
			
			gederr.type = INV_RECORD;
			gederr.line = linenum+1;
			goto END;
		}
	
		// create a individual struct 
		Individual *indivi = malloc (sizeof(Individual));
		if (indivi == NULL){
			gederr.type = OTHER_ERROR;
			gederr.line = -1;
			goto END;
		}

		//initalize the list for otherfield nad events in individual 
		indivi->otherFields = initializeList(&printField,&deleteField,&compareFields);
		indivi->events = initializeList(&printEvent,&deleteEvent,&compareEvents);	
		indivi->families = initializeList(&printFamily,&deleteFamily,&compareFamilies);

		Family *fam = malloc (sizeof(Family));
		if (fam == NULL){
			gederr.type = OTHER_ERROR;
			gederr.line = -1;
			goto END;
		}
		fam->otherFields = initializeList(&printField,&deleteField,&compareFields);
		fam->events = initializeList(&printEvent,&deleteEvent,&compareEvents);
		fam->children = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
		trimstr = strtok(buff, "\n\r\n\r\r\n");

		restr = strtok(trimstr," ");

		int numsize = strlen(restr);

		if (numsize >= 2){
			for (int i = 0; i < numsize; i++){
			
				if(strcmp(&restr[0],"0") == 0){;
					gederr.type = INV_GEDCOM;
					gederr.line = -1;
					goto END;
				}
			}
		}

		srtnum = atoi(restr);

		// checks if the level is correct 
		chkval = checkifnumisoff (srtnum,prevnum);
		
		if (chkval == 0 && linenum == 0){
			gederr.type = INV_GEDCOM;
			gederr.line = -1;
			goto END;
		}

		if (srtnum >= 0 && srtnum <= 99){
			isnum = true;
		}
		
		if (isnum == false){
			gederr.type = INV_GEDCOM;
			gederr.line = -1;
			goto END;
		}

		restr = strtok(NULL, " ");
		if (linenum == 0){
			if (strcmp(restr,"HEAD") != 0){
				gederr.type = INV_GEDCOM;
				gederr.line = -1;
				goto END;
			}
		}
		//checks the size of the tag	
		int tagsize = strlen(restr);
		if (strcmp(&restr[0],"@") == 0){
			tagsize = 3;
		}

		//checks if the tag are upper case 
		int isup = checkforupper(restr);

		if (isup == 0 || (tagsize > 4 && tagsize < 3)){
			if (headflag == true){
				gederr.type = INV_HEADER;
				gederr.line = linenum;
				goto END;
			}
			else if (indifalg == true){
				gederr.type = INV_RECORD;
				gederr.line = linenum;
				goto END;
			}
			else if (submiteeflag == true){
				gederr.type = INV_RECORD;
				gederr.line = linenum;
				goto END;
			}
			else if (famflag == true){
				gederr.type = INV_RECORD;
				gederr.line = linenum;
				goto END;
			}
			else{
				gederr.type = INV_GEDCOM;
				gederr.line = linenum;
				goto END;
			}
		}

		/*
 			---------INDI CHECK INDI CHECK INDI CHECK-------------
		*/
		sizstr = strlen(restr);
		
		for (int i = 0 ; i <= sizstr; i++){
			if (strcmp(&restr[i],"@") == 0){
				checkref = 1;
			}
			else if (restr[i] == 'I' || restr[i] == 'F'){
				x_ref = malloc(strlen(restr)+1*sizeof(char));
				strcpy(x_ref,restr);
			}
		}

		if ((srtnum == 0 && strcmp(&restr[sizstr-1],"@") == 0 && checkref == 1) || strcmp(restr,"INDI") == 0|| strcmp(restr,"FAM")==0 || strcmp(restr,"TRLR") == 0){
			if (headflag == true){
				gederr.type = INV_HEADER;
				gederr.line = linenum;
				goto END;
			}
			if (strcmp(restr,"INDI") == 0){
				indifalg = true; 
				submiteeflag = false;
				famflag = false;
				count++;					
			}
			else if (strcmp(restr,"TRLR") == 0){
			}
			else{
				restr = strtok(NULL,"\n\r\n\r\r\n");
			}
				// adds the individual record to the list with events and other fields 
				if (indifalg == true){
					if (strcmp(surn,"/") == 0){
						//printf ("in herer\n");
						
						//int givsize = strlen(givna);
						if (strcmp(givna," ") == 0){
							indivi->givenName = (char*)malloc(2 *sizeof(char));
							strcpy(indivi->givenName,"\0");
							indivi->surname = malloc(1*sizeof(char));
							strcpy(indivi->surname,"\0");
							surn = NULL;
							givna = NULL;
						}
						else{
							char *name = strtok(givna," ");
							int givsize = strlen(name);
							indivi->givenName = (char*)malloc(givsize+1 *sizeof(char));
							strcpy(indivi->givenName,name);
							givna = NULL;
							indivi->surname = malloc(1*sizeof(char));
							strcpy(indivi->surname,"\0");
							surn = NULL;
						}
					}
					else if (strcmp(givna," ") == 0){
						
					//	printf("empty name\n");
						indivi->givenName = malloc(1 * sizeof(char));
						strcpy(indivi->givenName,"\0");
						char *snam = strtok(surn,"/");
						int sursize = strlen(snam);
						indivi->surname = malloc(sursize+1 *sizeof(char));
						strcpy(indivi->surname,snam);
						surn = NULL;

					}
					else{
						int count = 0;
						int siz = strlen(givna);
						for (int i = 0; i < siz; i++){
							if (givna[i] == 32){
								count++;
							}
						}
						if (count == 1){
							char *name = strtok(givna," ");
							int givsize = strlen(name);
							indivi->givenName = (char*)malloc(givsize+1 *sizeof(char));
							strcpy(indivi->givenName,name);
							givna = NULL;
						}
						else if (count == 2) {
							char *name = strtok(givna," ");
							char *prttwo = strtok(NULL," ");
							indivi->givenName = (char *)malloc(sizeof(char) * strlen(name)+ strlen(prttwo)+1);
							strcpy(indivi->givenName,name);
							strcat(indivi->givenName," ");
							strcat(indivi->givenName,prttwo);
							givna = NULL;
						}
						char *snam = strtok(surn,"/");
						int sursize = strlen(snam);
						indivi->surname = malloc(sursize+1 *sizeof(char));
						strcpy(indivi->surname,snam);
						surn = NULL;	
				
					}
					int t = 0,m = 0;

					//add the other field in individual record 
					while (strcmp(fieldstr[t],"\0") != 0){
					
						Field *field = malloc (sizeof(Field));
						if (field == NULL){
							gederr.type = OTHER_ERROR;
							gederr.line = -1;
							goto END;
						}
						field->tag = malloc(strlen(fieldstr[t])+1 * sizeof(char));
						strcpy(field->tag,fieldstr[t]);

						field->value = malloc (strlen(fieldval[t])+1 *sizeof(char));
						strcpy(field->value,fieldval[t]);
						
						insertBack(&indivi->otherFields,field);
						t++;
					}

					//reset the array list of value and tag to empty
					for (int i = 0 ; i < 50; i++){
						strcpy(fieldstr[i],"\0");
					}

					for (int i = 0 ; i < 50; i++){
						strcpy(fieldval[i],"\0");
					}

					// adds the events in the individual records 
					while (strcmp(eventfield[m],"\0") != 0)
					{	

						if (strcmp(eventfield[m],"BIRT") == 0 || strcmp(eventfield[m],"CHR") == 0 || strcmp(eventfield[m],"DEAT") == 0
							|| strcmp(eventfield[m],"BURI") == 0 || strcmp(eventfield[m],"CREAM") == 0 || strcmp(eventfield[m],"ADOP") == 0
							|| strcmp(eventfield[m],"BAPM") == 0 || strcmp(eventfield[m],"BARM") == 0 || strcmp(eventfield[m],"BASM") == 0
							|| strcmp(eventfield[m],"BLES") == 0 || strcmp(eventfield[m],"CHRA") == 0 || strcmp(eventfield[m],"CONF") == 0 
							|| strcmp(eventfield[m],"FCOM") == 0 || strcmp(eventfield[m],"ORDN") == 0 || strcmp(eventfield[m],"NATU") == 0
							|| strcmp(eventfield[m],"EMIG") == 0 || strcmp(eventfield[m],"IMMI") == 0 || strcmp(eventfield[m],"CENS") == 0
							|| strcmp(eventfield[m],"PROB") == 0 || strcmp(eventfield[m],"WILL") == 0 || strcmp(eventfield[m],"GRAD") == 0
							|| strcmp(eventfield[m],"RETI") == 0 || strcmp(eventfield[m],"EVEN") == 0){
							char *datee;
							Event *event = malloc (sizeof(Event));

							strcpy(event->type,eventfield[m]);
							m++;
							datee = strtok(eventfield[m]," ");
							
							if (agefield != NULL){

								event->otherFields = initializeList(&printField,&deleteField,&compareFields);
								
								Field * field = malloc (sizeof (Field));
								if (field == NULL){
									gederr.type = OTHER_ERROR;
									gederr.line  = -1;
									goto END;
								}
								field->tag =malloc (strlen("AGE")+1 * sizeof(char));
								if (field->tag == NULL){
									gederr.type = OTHER_ERROR;
									gederr.line  = -1;
									goto END;									
								}
								strcpy(field->tag,"AGE");
								field->value =malloc (strlen(agefield)+1 * sizeof(char));
								if (field->tag == NULL){
									gederr.type = OTHER_ERROR;
									gederr.line  = -1;
									goto END;									
								}	
								strcpy(field->value,agefield);
								insertBack(&event->otherFields,field);
								agefield = NULL;	
							}
							if (strcmp(datee,"DATE") == 0){
								datee = strtok(NULL,"\n\r\n\r\r\n");
								event->date = malloc (strlen(datee)+1 * sizeof(char));
								strcpy(event->date,datee);
								m++;

								if (strcmp(eventfield[m],"\0") == 0){
								}
								else {
									datee = strtok(eventfield[m]," ");	
								}
								
								if (strcmp(datee,"PLAC") == 0){
									datee = strtok(NULL,"\n\r\r\n\n\r");
									event->place = malloc (strlen(datee)+1 * sizeof(char));
									strcpy(event->place,datee);
								}
								else {
									event->place = malloc (1 * sizeof(char));
									strcpy(event->place,"\0");
									m--;									
								}
								insertBack(&indivi->events,event);
							}
							else if (strcmp(datee,"PLAC") == 0){
								datee = strtok(NULL,"\n\r\n\r\r\n");
								event->place = malloc (strlen(datee)+1 * sizeof(char));
								strcpy(event->place,datee);		
								m++;

								if (strcmp(eventfield[m],"\0") == 0){
								}
								else {
									datee = strtok(eventfield[m]," ");	
								}
								if (agefield != NULL){

									event->otherFields = initializeList(&printField,&deleteField,&compareFields);
									
									Field * field = malloc (sizeof (Field));
									if (field == NULL){
										gederr.type = OTHER_ERROR;
										gederr.line  = -1;
										goto END;
									}
									field->tag =malloc (strlen("AGE")+1 * sizeof(char));
									if (field->tag == NULL){
										gederr.type = OTHER_ERROR;
										gederr.line  = -1;
										goto END;									
									}
									strcpy(field->tag,"AGE");
									field->value =malloc (strlen(agefield)+1 * sizeof(char));
									if (field->tag == NULL){
										gederr.type = OTHER_ERROR;
										gederr.line  = -1;
										goto END;									
									}	
									strcpy(field->value,agefield);
									insertBack(&event->otherFields,field);
									agefield = NULL;	
								}

								if (strcmp(datee,"DATE") == 0){
									datee = strtok(NULL,"\n\r\r\n\n\r");
									event->date = malloc (strlen(datee)+1 * sizeof(char));
									strcpy(event->date,datee);
								}
								else{
									event->date = malloc (strlen(datee)+1 * sizeof(char));
									strcpy(event->date,"");	
									m--;								
								}
								insertBack(&indivi->events,event);														
							}
							else {
								event->date = malloc (1 * sizeof(char));
								strcpy(event->date,"\0");
								event->place = malloc (1 * sizeof(char));
								strcpy(event->place,"\0");	
								insertBack(&indivi->events,event);																				
							}						
					}																										
						m++;
				}

				// resets the eventfield array to empty 
				for (int i = 0 ; i < 50; i++){
					eventfield[i] = malloc(1 * sizeof(char));
					if (eventfield[i] == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(eventfield[i],"\0");
				}

				e = 0;
				n = 0;

				// add the indivudual records into gedcom object 
				insertBack(&tmpobj->individuals,indivi);
				insertData(hash,ref,(void *)indivi);
			}

			int b = 0;

			// adds the fields and required names to the submitter 
			if (submiteeflag == true){
				int t = 0;
				while (strcmp(fieldstr[t],"\0") != 0){
					
					Field *field = malloc (sizeof(Field));
					if (field == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}

					field->tag = malloc(strlen(fieldstr[t])+1 * sizeof(char));
					strcpy(field->tag,fieldstr[t]);

					field->value = malloc (strlen(fieldval[t])+1 * sizeof(char));
					strcpy(field->value,fieldval[t]);
					
					insertBack(&tmpobj->submitter->otherFields,field);
					t++;
				}
				
				for (int i = 0; i < 50; i++){
		
					strcpy(fieldstr[i],"\0");
					strcpy(fieldval[i],"\0");
				}
				char *name = NULL;
				char *addr = NULL; 
				while (strcmp(submfield[b],"\0") != 0){
					char * strsum = NULL;
					strsum = strtok(submfield[b]," ");

					if (strcmp(strsum,"NAME") == 0){
						name = strtok(NULL,"\n\r\n\r\r\n");
					}
					else if (strcmp(strsum,"ADDR") == 0){
						addr = strtok(NULL,"\n\r\n\r\r\n");
					}
					b++;
				}

				if (addr != NULL){
					Submitter *tmpsub = malloc (sizeof(Submitter)+ sizeof(char) * strlen(addr)+1);
					tmpsub->otherFields = initializeList(&printField,&deleteField,&compareFields);
					if (tmpsub != NULL){
						strcpy(tmpsub->submitterName,name);
						strcpy(tmpsub->address,addr);
					}
					tmpobj->submitter = tmpsub;	
					tmpobj->header->submitter = tmpsub;
				}
				else {
					int size = 1;
					Submitter *tmpsub = malloc (sizeof(Submitter)+ sizeof(char) * size+ 1);	
					tmpsub->otherFields = initializeList(&printField,&deleteField,&compareFields);
					if (tmpsub != NULL){
						strcpy(tmpsub->submitterName,name);
						strcpy(tmpsub->address,"\0");
					}
					tmpobj->submitter = tmpsub;	
					tmpobj->header->submitter = tmpsub;			
				}
			}

			// add the family record to the list in gedcomobj
			if (famflag == true){

				int t = 0;
				//add the other field in individual record 
				while (strcmp(fieldstr[t],"\0") != 0){
					
					Field *field = malloc (sizeof(Field));
					if (field == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}

					field->tag = malloc(strlen(fieldstr[t])+1 * sizeof(char));
					strcpy(field->tag,fieldstr[t]);

					field->value = malloc (strlen(fieldval[t])+1 * sizeof(char));
					strcpy(field->value,fieldval[t]);
					
					insertBack(&fam->otherFields,field);
					t++;
				}

				//reset the array list of value and tag to empty
				for (int i = 0 ; i < 50; i++){
					fieldstr[i] = malloc(1 * sizeof(char));
					if(fieldstr[i] == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(fieldstr[i],"\0");
				}

				for (int i = 0 ; i < 50; i++){
					fieldval [i] = malloc(1 * sizeof(char));
					if(fieldval [i] == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(fieldval[i],"\0");
				}

				int m = 0;
				while (strcmp(famfield[m],"\0") != 0)
				{	

					if (strcmp(famfield[m],"MARL") == 0 || strcmp(famfield[m],"MARS") == 0 || strcmp(famfield[m],"ANUL") == 0
						|| strcmp(famfield[m],"CENS") == 0 || strcmp(famfield[m],"DIV") == 0 || strcmp(famfield[m],"DIVF") == 0
						|| strcmp(famfield[m],"ENGA") == 0 || strcmp(famfield[m],"MARB") == 0|| strcmp(famfield[m],"MARC") == 0
						|| strcmp(famfield[m],"MARR") == 0 || strcmp(famfield[m],"RESI") == 0 || strcmp(famfield[m],"EVEN") == 0){
						
						char *datee = NULL;
						Event *event = malloc (sizeof(Event));
						strcpy(event->type,famfield[m]);

						m++;
						
						datee = strtok(famfield[m]," ");
						if (datee == NULL){
							event->date = malloc (1 * sizeof(char));
							strcpy(event->date,"\0");
							event->place = malloc (1 * sizeof(char));
							strcpy(event->place,"\0");	
							insertBack(&fam->events,event);
							break;
						}
						if (agefield != NULL){

							event->otherFields = initializeList(&printField,&deleteField,&compareFields);
							
							Field * field = malloc (sizeof (Field));
							if (field == NULL){
								gederr.type = OTHER_ERROR;
								gederr.line  = -1;
								goto END;
							}
							field->tag =malloc (strlen("AGE")+1 * sizeof(char));
							if (field->tag == NULL){
								gederr.type = OTHER_ERROR;
								gederr.line  = -1;
								goto END;									
							}
							strcpy(field->tag,"AGE");
							field->value =malloc (strlen(agefield)+1 * sizeof(char));
							if (field->tag == NULL){
								gederr.type = OTHER_ERROR;
								gederr.line  = -1;
								goto END;									
							}	
							strcpy(field->value,agefield);

							insertBack(&event->otherFields,field);
							agefield = NULL;	
						}

						if (strcmp(datee,"DATE") == 0){
							datee = strtok(NULL,"\n\r\n\r\r\n");
							event->date = malloc (strlen(datee)+1 * sizeof(char));
							strcpy(event->date,datee);
							m++;

							if (strcmp(eventfield[m],"\0") == 0){
							}
							else {
								datee = strtok(eventfield[m]," ");	
							}
							
							if (strcmp(datee,"PLAC") == 0){
								datee = strtok(NULL,"\n\r\r\n\n\r");
								event->place = malloc (strlen(datee)+1 * sizeof(char));
								strcpy(event->place,datee);
							}
							else {
								event->place = malloc (1 * sizeof(char));
								strcpy(event->place,"\0");
								m--;									
							}
							insertBack(&fam->events,event);
						}
						else if (strcmp(datee,"PLAC") == 0){
							datee = strtok(NULL,"\n\r\n\r\r\n");
							event->place = malloc (strlen(datee)+1 * sizeof(char));
							strcpy(event->place,datee);		
							m++;

							if (strcmp(eventfield[m],"\0") == 0){
							}
							else {
								datee = strtok(eventfield[m]," ");	
							}
							if (agefield != NULL){

								event->otherFields = initializeList(&printField,&deleteField,&compareFields);
								
								Field * field = malloc (sizeof (Field));
								if (field == NULL){
									gederr.type = OTHER_ERROR;
									gederr.line  = -1;
									goto END;
								}
								field->tag =malloc (strlen("AGE")+1 * sizeof(char));
								if (field->tag == NULL){
									gederr.type = OTHER_ERROR;
									gederr.line  = -1;
									goto END;									
								}
								strcpy(field->tag,"AGE");
								field->value =malloc (strlen(agefield)+1 * sizeof(char));
								if (field->tag == NULL){
									gederr.type = OTHER_ERROR;
									gederr.line  = -1;
									goto END;									
								}	
								strcpy(field->value,agefield);
								insertBack(&event->otherFields,field);
								agefield = NULL;	
							}

							if (strcmp(datee,"DATE") == 0){
								datee = strtok(NULL,"\n\r\r\n\n\r");
								event->date = malloc (strlen(datee)+1 * sizeof(char));
								strcpy(event->date,datee);
							}
							else{
								event->date = malloc (strlen(datee)+1 * sizeof(char));
								strcpy(event->date,"");	
								m--;								
							}
							insertBack(&fam->events,event);														
						}
						else {

							event->date = malloc (1 * sizeof(char));
							strcpy(event->date,"\0");
							event->place = malloc (1 * sizeof(char));
							strcpy(event->place,"\0");	
							insertBack(&fam->events,event);

						}					
					}
					m++;
				}
				n = 0;
				p = 0;
				insertBack(&tmpobj->families,fam);
				insertData(hash,ref,(void *)fam);
			}

			for(int i = 0; i < 50; i++){
				famfield[i] = malloc(1 *sizeof(char));

				strcpy(famfield[i],"\0");
			}
			//checks if the string is equal to individual 
			if (strcmp(restr,"INDI") == 0){
				indifalg = true; 
				submiteeflag = false;
				famflag = false;
				count++;
			}

			//checks if the string is equal to individual
			else if (strcmp(restr,"SUBM") == 0){
				submiteeflag = true;
				indifalg = false;
				sumbflag = true;
				famflag = false;
			}
			else if (strcmp(restr,"FAM") == 0){
				famflag = true;
				submiteeflag = false;
				indifalg = false;
			}
			else if (strcmp(restr,"TRLR") == 0){
				trlrflag = true;
				//break;
			}
			else {
				gederr.type = INV_RECORD;
				gederr.line = linenum;
				goto END; 
			}
		}
		if (trlrflag == true){
			break;
		}
		/*
		----------------HEADER HEADER HEADER HEADER---------------
		*/
		if (srtnum == 1 ) {
			namstr = malloc (strlen(restr)+1 * sizeof(char));
			if (namstr == NULL){
				gederr.type = OTHER_ERROR;
				gederr.line = -1;
			}
			strcpy(namstr,restr);
		}

		if (strcmp(restr,"HEAD") == 0 && srtnum == 0)
		{
			headflag = true;
			headerflag = true;
			chkval = checkifnumisoff (srtnum,prevnum);
			if (chkval == 0){
				gederr.type = INV_HEADER;
				gederr.line = linenum;
				goto END;
			}
			tmpobj->header = malloc(sizeof(Header));
			if (tmpobj->header == NULL){
				gederr.type = OTHER_ERROR;
				gederr.line = -1;
				goto END;
			}

			tmpobj->header->otherFields = initializeList(&printField,&deleteField,&compareFields);		
		}
		else if (strcmp(restr,"SOUR") == 0 && srtnum == 1){
			sour = true;
		}
		else if (strcmp(restr,"SUBM") == 0 && srtnum == 1){
			submf = true;
			headflag = false;
		}
		else if (strcmp(restr,"GEDC") == 0 && srtnum == 1){
			gedf = true;
		}
		else if (strcmp(restr,"CHAR") == 0 && srtnum == 1){
			charsf = true;
		}

		   if (headflag == true ){
		   		if (srtnum > 99 && srtnum < 0){
		   			gederr.type = INV_HEADER;
		   			gederr.line = linenum;
		   			goto END;
		   		}
		   }
		if (headflag == true){
		
		chkval = checkifVaild(srtnum,restr);


		if (strcmp(restr,"SOUR") == 0){
			restr = strtok(NULL,"\n\r\n\r\r\n");
			strcpy(tmpobj->header->source,restr);
		}
		else if (gedf == true){
			
			if (strcmp(restr,"VERS") == 0){
				int num = checkifVaild(srtnum,restr);
				if (num == 0){
					gederr.type = INV_RECORD;
					gederr.line = linenum+1;
					goto END;
				}
				restr = strtok(NULL,"\n\r\n\r\r\n");
				float ver= atof(restr);
				tmpobj->header->gedcVersion = ver;
			}
		}
		else if (sour == true){
			if(strcmp(restr,"VERS") == 0){
				Field *field = malloc (sizeof(Field));
		 		if (field == NULL){
	 				gederr.type = OTHER_ERROR;
	 				gederr.line = -1;
	 				goto END;
		 		}

				field->tag = malloc (strlen(restr)+1 * sizeof(char *));	 	
				if (field->tag == NULL){
	 				gederr.type = OTHER_ERROR;
	 				gederr.line = -1;
	 				goto END;
	 			}
	 			strcpy(field->tag,restr);
	 			restr = strtok(NULL,"\n\r\n\r\r\n");
	 			if (restr == NULL){
	 				gederr.type = INV_HEADER;
	 				gederr.line = linenum;
	 				goto END;
	 			}

	 			field->value = malloc (strlen(restr)+ 1 *sizeof(char));
	 			if (field->value == NULL){
	 				gederr.type = OTHER_ERROR;
	 				gederr.line = -1;
	 				goto END;
	 			}
	 			strcpy(field->value,restr);	
		 		insertBack (&tmpobj->header->otherFields,field);	 								
			}
		}
		if (strcmp(restr,"CHAR") == 0){
			restr = strtok(NULL,"\n\r\r\n\n\r");			
			if (restr == NULL){
				gederr.type = INV_HEADER;
				gederr.line = linenum;
				goto END;
			}
			if (strcmp(restr,"ANSEL") == 0){
				tmpobj->header->encoding = ANSEL;
			}
			else if (strcmp(restr,"UTF-8") == 0){
				tmpobj->header->encoding = UTF8;
			}
			else if (strcmp(restr,"UNICODE") == 0){
				tmpobj->header->encoding = UNICODE;
			}
			else if (strcmp(restr,"ASCII") == 0){
				tmpobj->header->encoding = ASCII;
			}
			else {
				gederr.type = INV_HEADER;
				gederr.line = linenum;
				goto END;
			}
		}

		if (chkval == 0){
			chkval = checkifnumisoff (srtnum,prevnum);
			if (chkval == 0){
				gederr.type = INV_RECORD;
				gederr.line = linenum+1;
				goto END;
			}
			else{
				if (headflag == true){
				Field *field = malloc (sizeof(Field));
		 		if (field == NULL){
	 				gederr.type = OTHER_ERROR;
	 				gederr.line = -1;
	 				goto END;
		 			}	
		 			field->tag = malloc (strlen(restr)+1 * sizeof(char ));
		 			if (field->tag == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 			strcpy( field->tag,restr);
		 			restr = strtok(NULL,"\n\r\n\r\r\n");
		 			field->value = malloc (strlen(restr)+ 1 *sizeof(char ));
		 			if (field->value == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 			strcpy(field->value,restr);

		 			insertBack (&tmpobj->header->otherFields,field);
	 			}
			}
		}
	}

		if (headflag == false){
			if (sour != true || submf != true || charsf != true || gedf != true){
					gederr.type = INV_HEADER;
					gederr.line = linenum;
					goto END;
			}
		}

		/*
		----------------INDIVIDUAL INDIVIDUAL INDIVIDUAL---------------
		*/
		if (indifalg == true){
			chkval = checkifnumisoff(srtnum,prevnum);
			if (chkval == 0){
				gederr.type = INV_RECORD;
				gederr.line = linenum;
				goto END;
			}

			if (strcmp(restr,"NAME") == 0){

				restr = strtok(NULL,"/");
				if (strcmp(restr,"") == 0){
					givna = malloc(strlen(restr)+1 * sizeof(char));
					strcpy(givna,restr);
				} 
				else{
					givna = malloc(strlen(restr)+1 *sizeof(char));
					strcpy(givna,restr);
				}
				restr = strtok(NULL,"\n\r\n\r\r\n");

				if (strcmp(restr,"/") == 0){
					surn = malloc(1 * sizeof(char));
					strcpy(surn,"");
				}
				
				if (strcmp(restr,"\0") == 0){
					surn = malloc(1 * sizeof(char));
					strcpy(surn,"");
				}
				else{
					surn = malloc(strlen(restr)+2 * sizeof(char));	
					strcpy(surn,restr);

				}
			}
			else if (strcmp(restr,"GIVN") == 0){
				restr = strtok(NULL,"\n\r\n\r\r\n");
				if (givna == NULL){
					givna = malloc(strlen(restr)+1 * sizeof(char));
					strcpy(givna,restr);
				}
				else {
					fieldstr[n] = malloc(strlen("GIVN")+1 *sizeof(char));
		 			if (fieldstr[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 			strcpy(fieldstr[n],"GIVN");

		 		 	fieldval[n] = malloc(strlen(restr)+1 * sizeof(char));
		 		 	if (fieldval[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 		 	strcpy(fieldval[n],restr);
					n++;
				}
			}
			else if (strcmp(restr,"SURN") == 0){
				restr = strtok(NULL,"\n\r\n\r\r\n");
				if (surn == NULL){
					surn = malloc(strlen(restr)+1 * sizeof(char));
					strcpy(surn,restr);
				}
				else {
					fieldstr[n] = malloc(strlen("SURN")+1 *sizeof(char));
		 			if (fieldstr[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 			strcpy(fieldstr[n],"SURN");
		 					 		 	
		 		 	fieldval[n] = malloc(strlen(restr)+1 * sizeof(char));
		 		 	if (fieldval[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 		 	strcpy(fieldval[n],restr);
					n++;
				}				
			}
			
			else if (strcmp(restr,"INDI") == 0){
				ref = malloc(strlen(x_ref)+1 * sizeof(char));
				strcpy(ref,x_ref);
			}

			else if (strcmp(restr,"DATE") == 0){
				char *finstr = NULL;
				 restr = strtok(NULL,"\n\r\n\r\r\n");
				if (restr == NULL){
					gederr.type = INV_RECORD;
					gederr.line = linenum;
					goto END;
				}
				 finstr = malloc(strlen(restr)+ strlen("DATE ") + 1 *sizeof(char));
				 strcpy(finstr,"DATE ");
				 strcat(finstr,restr);

				 eventfield[e] = malloc(strlen(finstr)+1 * sizeof(char));
				 strcpy(eventfield[e],finstr);
				 free(finstr);		
				 e++;
			}
			else if (strcmp(restr,"PLAC") == 0){
				char *finstr;
				restr = strtok(NULL,"\n\r\n\r\r\n");
				if (restr == NULL){
					gederr.type = INV_RECORD;
					gederr.line = linenum;
					goto END;
				}
				 finstr = malloc(strlen(restr)+ strlen("PLAC ") + 1 *sizeof(char));
				 strcpy(finstr,"PLAC ");
				 strcat(finstr,restr);

				 eventfield[e] = malloc(strlen(finstr)+1 * sizeof(char));
				 strcpy(eventfield[e],finstr);
				 free(finstr);
				 e++;

			}
			else if (strcmp(restr,"AGE") == 0){
				restr = strtok(NULL,"\n\r\n\r\r\n");
				if (restr == NULL){
					gederr.type = INV_RECORD;
					gederr.line = linenum;
					goto END;
				}
				agefield = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(agefield,restr);
			}
			else if (strcmp(restr,"FAMC") == 0){
				fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
		 		if (fieldstr[n] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldstr[n],restr);

		 		restr = strtok(NULL,"\n\r\n\r\r\n");
		 		fieldval[n] = malloc (strlen(restr)+1 * sizeof(char));
		 		if (fieldstr[n] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldval[n],restr);
		 		n++;		 		
			}
			else if (strcmp(restr,"FAMS") == 0){
				fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
		 		if (fieldstr[n] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldstr[n],restr);

		 		restr = strtok(NULL,"\n\r\n\r\r\n");
		 		fieldval[n] = malloc (strlen(restr)+1 * sizeof(char));
		 		if (fieldstr[n] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldval[n],restr);
		 		n++;	
			}
			else if (strcmp(restr,"BIRT") == 0 || strcmp(restr,"CHR") == 0){
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;
			}
			else if (strcmp(restr,"DEAT") == 0 ){	
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;				
			}
			else if ((strcmp(restr,"BURI") == 0 || strcmp(restr,"CREAM") == 0)){
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;											
			}
			else if (strcmp(restr,"ADOP") == 0){
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;								
			}
			else if ((strcmp(restr,"BAPM") == 0 || strcmp(restr,"BARM") == 0 || 
				strcmp(restr,"BASM") == 0 || strcmp(restr,"BLES") == 0)){
			
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;								
			}
			else if ((strcmp(restr,"CHRA") == 0 || strcmp(restr,"CONF") == 0 || strcmp(restr,"FCOM") == 0 || strcmp(restr,"ORDN") == 0)){			
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;									
			}
			else if ((strcmp(restr,"NATU") == 0 ||  strcmp(restr,"EMIG") == 0 || strcmp(restr,"IMMI") == 0)){
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;							
			}
			else if ((strcmp(restr,"CENS") == 0 || strcmp(restr,"PROB") == 0 || strcmp(restr,"WILL") == 0)){
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;			
			}
			else if ((strcmp(restr,"GRAD") == 0 || strcmp(restr,"RETI") == 0)){	
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;	
			}
			else if (strcmp(restr,"EVEN") == 0){
				eventfield[e] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(eventfield[e],restr);
				e++;	
			}
			else {

				if (indifalg == true){

		 			fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
		 			if (fieldstr[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 			strcpy(fieldstr[n],restr);


		 		 	restr = strtok(NULL,"\n\r\n\r\r\n"); 
		 		 	if (restr == NULL){
		 		 		gederr.type = INV_RECORD;
		 		 		gederr.line = linenum; 
		 		 		goto END;
		 		 	}
		 		 	fieldval[n] = malloc(strlen(restr)+1 * sizeof(char));
		 		 	if (fieldval[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 		 	strcpy(fieldval[n],restr);
					n++;
				}
			}
		}


		/*
		------------ FAMILY FAMILY FAMILY FAMILY ----------------
		*/
		 if (famflag == true){
			
			chkval = checkifnumisoff(srtnum,prevnum);
			if (chkval == 0){
				gederr.type = INV_RECORD;
				gederr.line = linenum;
				goto END;					
			}	
		 	 if (strcmp(restr,"HUSB") == 0){
				fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
		 		if (fieldstr[n] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldstr[n],restr);

		 		restr = strtok(NULL,"\n\r\n\r\r\n");
		 		fieldval[n] = malloc (strlen(restr)+1 * sizeof(char));
		 		if (fieldstr[p] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldval[n],restr);	
		 		n++;
		 	 }
		 	 else if (strcmp(restr,"WIFE") == 0){
		 	 	fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
		 		if (fieldstr[n] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldstr[n],restr);

		 		restr = strtok(NULL,"\n\r\n\r\r\n");
		 		fieldval[n] = malloc (strlen(restr)+1 * sizeof(char));
		 		if (fieldstr[p] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldval[n],restr);	
		 		n++;
		 	 }
		 	 else if (strcmp(restr,"CHIL") == 0){
		 	 	fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
		 		if (fieldstr[n] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldstr[n],restr);

		 		restr = strtok(NULL,"\n\r\n\r\r\n");
		 		fieldval[n] = malloc (strlen(restr)+1 * sizeof(char));
		 		if (fieldstr[p] == NULL){
		 			gederr.type = OTHER_ERROR;
		 			gederr.line = -1;
		 			goto END;
		 		}
		 		strcpy(fieldval[n],restr);	
		 		n++;
		 	 }
		 	 else if (strcmp(restr,"DATE") == 0){
				char *finstr = NULL;
				 restr = strtok(NULL,"\n\r\n\r\r\n");
				if (restr == NULL){
					gederr.type = INV_RECORD;
					gederr.line = linenum;
					goto END;
				}
				 finstr = malloc(strlen(restr)+ strlen("DATE ") + 1 *sizeof(char));
				 strcpy(finstr,"DATE ");
				 strcat(finstr,restr);

				 famfield[p] = malloc(strlen(finstr)+1 * sizeof(char));
				 strcpy(famfield[p],finstr);
				 free(finstr);		
				 p++;
			}
			else if (strcmp(restr,"PLAC") == 0){
				char *finstr;
				restr = strtok(NULL,"\n\r\n\r\r\n");
				if (restr == NULL){
					gederr.type = INV_RECORD;
					gederr.line = linenum;
					goto END;
				}
				 finstr = malloc(strlen(restr)+ strlen("PLAC ") + 1 *sizeof(char));
				 strcpy(finstr,"PLAC ");
				 strcat(finstr,restr);

				 famfield[p] = malloc(strlen(finstr)+1 * sizeof(char));
				 strcpy(famfield[e],finstr);
				 free(finstr);
				 p++;

			}
			else if (strcmp(restr,"AGE") == 0){
				restr = strtok(NULL,"\n\r\n\r\r\n");
				if (restr == NULL){
					gederr.type = INV_RECORD;
					gederr.line = linenum;
					goto END;
				}
				agefield = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(agefield,restr);
			}			
		 	 else if (strcmp(restr,"MARR") == 0){
				famfield[p] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(famfield[p],restr);
				p++;	
		 	 }
		 	 else if (strcmp(restr,"ANUL") == 0 || strcmp(restr,"CENS") == 0 || 
		 	 	strcmp(restr,"DIV") == 0 || strcmp(restr,"DIVF") == 0){
				famfield[p] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(famfield[p],restr);
				p++;	
		 	 }
		 	else if (strcmp(restr,"ENGA") == 0 || strcmp(restr,"MARB") == 0 || strcmp(restr,"MARC") == 0){
				famfield[p] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(famfield[p],restr);
				p++;	
		 	}
		 	else if (strcmp(restr,"MARL") == 0 || strcmp(restr,"MARS") == 0){
				famfield[p] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(famfield[p],restr);
				p++;	
		 	}
		 	else if (strcmp(restr,"EVEN") == 0){
				famfield[p] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(famfield[p],restr);
				p++;	
		 	}
		 	else if (strcmp(restr,"RESI") == 0){
				famfield[p] = malloc (strlen(restr)+1 * sizeof(char));
				strcpy(famfield[p],restr);
				p++;	
		 	}
		 	else if (strcmp(restr,"FAM") == 0) {
		 		ref = malloc(strlen(x_ref)+1 * sizeof(char));
				strcpy(ref,x_ref);
		 	}

		 	else{
		 		if (famflag == true){
					fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
		 			if (fieldstr[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 			strcpy(fieldstr[n],restr);


		 		 	restr = strtok(NULL,"\n\r\n\r\r\n"); 
		 		 	if (restr == NULL){
		 		 		gederr.type = INV_RECORD;
		 		 		gederr.line = linenum;
		 		 		goto END;
		 		 	}

		 		 	fieldval[n] = malloc(strlen(restr)+1 * sizeof(char));
		 		 	if (fieldval[n] == NULL){
		 				gederr.type = OTHER_ERROR;
		 				gederr.line = -1;
		 				goto END;
		 			}
		 		 	strcpy(fieldval[n],restr);
					n++;
		 		}
		 	}
		 } 
		/*
			----------------SUBMITTER SUBMITTER SUBMiTTER ---------
		*/
			if (submiteeflag == true){
				chkval = checkifnumisoff(srtnum,prevnum);
				if (chkval == 0){
					gederr.type = INV_RECORD;
					gederr.line = linenum;
					goto END;					
				}
				if (strcmp(restr,"NAME") == 0){
					char *sunstr = NULL;
					restr = strtok(NULL,"\n\r\r\n\n\r");
					if (restr == NULL){
						gederr.type = INV_RECORD;
						gederr.line = linenum;
						goto END;
					}
					sunstr = malloc(strlen("NAME ") + strlen(restr)+1 *sizeof(char));
					if (sunstr == NULL){
						gederr.type = INV_RECORD;
						gederr.line = linenum;
						goto END;
					}
					strcpy(sunstr,"NAME ");
					strcat(sunstr,restr);

					submfield[u] = malloc (strlen(sunstr)+1 * sizeof(char));
					if (submfield[u] == NULL){
						gederr.type= OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(submfield[u],sunstr);
					u++;
				}
				else if (strcmp(restr,"ADDR") == 0){
					char *sunstr = NULL;
					restr = strtok(NULL,"\n\r\r\n\n\r");
					if (restr == NULL){
						gederr.type = INV_RECORD;
						gederr.line = linenum;
						goto END;

					}
					sunstr = malloc(strlen("ADDR ") + strlen(restr)+1 *sizeof(char));
					if (sunstr == NULL){
						gederr.type= OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(sunstr,"ADDR ");
					strcat(sunstr,restr);

					submfield[u] = malloc (strlen(sunstr)+1 * sizeof(char));
					if (submfield[u] == NULL){
						gederr.type= OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(submfield[u],sunstr);
					u++;
				}
				else if (strcmp(restr,"SUBM") == 0){

				}
				else if (strcmp(restr,"CONT") == 0 || strcmp(restr,"CONC") == 0){
					char *sunstr = NULL;
					char *prttwo = NULL;

					prttwo = strtok(NULL,"\n\r\r\n\n\r");
					
					if (restr == NULL){
						gederr.type = INV_RECORD;
						gederr.line = linenum;
						goto END;

					}
					sunstr = malloc(strlen(restr)+strlen(prttwo)+1 *sizeof(char));
					if (sunstr == NULL){
						gederr.type= OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(sunstr,restr);
					strcat(sunstr," ");
					strcat(sunstr,prttwo);

					submfield[u] = malloc (strlen(sunstr)+1 * sizeof(char));
					if (submfield[u] == NULL){
						gederr.type= OTHER_ERROR;
						gederr.line = -1;
						goto END;
					}
					strcpy(submfield[u],sunstr);
					u++;
				}
				else{
					if (submiteeflag == true){
						fieldstr[n] = malloc(strlen(restr)+1 *sizeof(char));
			 			if (fieldstr[n] == NULL){
			 				gederr.type = OTHER_ERROR;
			 				gederr.line = -1;
			 				goto END;
			 			}
			 			strcpy(fieldstr[n],restr);


			 		 	restr = strtok(NULL,"\n\r\n\r\r\n"); 
			 		 	if (restr == NULL){
			 		 		gederr.type = INV_RECORD;
			 		 		gederr.line  = linenum;
			 		 		goto END;
			 		 	}
			 		 	fieldval[n] = malloc(strlen(restr)+1 * sizeof(char));
			 		 	if (fieldval[n] == NULL){
			 				gederr.type = OTHER_ERROR;
			 				gederr.line = -1;
			 				goto END;
			 			}
			 		 	strcpy(fieldval[n],restr);
						n++;
					}					
				}
			}
		prevnum = srtnum;
		linenum++;	
	}
	//change this 
	if (headerflag == false)
	{
		gederr.type = INV_GEDCOM;
		gederr.line = -1;
		goto END;
	}

	if (trlrflag == false){
		gederr.type = INV_GEDCOM;
		gederr.line = -1;
		goto END;
	}
	if (trlrflag == true){
		if (sumbflag == false || headerflag == false){
			gederr.type = INV_GEDCOM;
			gederr.line = -1;
			goto END;			
		}
	}

	List indilist = tmpobj->individuals;
	Node* nodeindo = indilist.head;
	
	List tmpListind = initializeList(&printField,&deleteField,&compareFields);

	// links the families inside individuals 
	while (nodeindo != NULL){
		Individual *ind = (Individual *) nodeindo->data;
		
		if (ind != NULL){
			List otfieldInd = ind->otherFields;
			Node *othefid = otfieldInd.head;
			
			while(othefid != NULL){
				Field* otherfied = (Field*) othefid->data;
			
				if (strcmp(otherfied->tag,"FAMS") == 0 || strcmp(otherfied->tag,"FAMC") ==0){
					HNode *hasnode = lookupData(hash,otherfied->value);
					
					if (hasnode  == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = linenum;
						goto END;
					}
					Family* indfam = (Family*)hasnode->data;
					
					if(indfam != NULL){
						insertBack(&ind->families,indfam); 
						insertBack(&tmpListind,otherfied);
					}	
				}
				othefid = othefid->next;
			}
		}
		ListIterator listtmp = createIterator(tmpListind);
		void *elemof;

		while((elemof = nextElement(&listtmp)) != NULL){
			deleteDataFromList(&ind->otherFields,elemof);
		}
		// clearList(&tmpListind);
     	nodeindo = nodeindo->next;
	}

	List famlist = tmpobj->families;
	Node* nodefam = famlist.head;

	List tmpList = initializeList(&printField,&deleteField,&compareFields);

	//links the individual inside family 
	while (nodefam != NULL){
		Family *fami = (Family *) nodefam->data;
		
		if (fami != NULL){
			List otfieldfam = fami->otherFields;
			Node *othefidf = otfieldfam.head;
			
			while(othefidf != NULL){
				Field* otherfiedf = (Field*) othefidf->data;
				//adds it herer;
				if (strcmp(otherfiedf->tag,"HUSB") == 0){
					HNode *hasnodef = lookupData(hash,otherfiedf->value);
					
					if (hasnodef == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = linenum;
						goto END;
					}
					Individual* famind = (Individual*)hasnodef->data;
					fami->husband = famind;	
				//	insertBack(&tmpList,otherfiedf);
				}
				else if (strcmp(otherfiedf->tag,"WIFE") == 0){
					HNode *hasnodef = lookupData(hash,otherfiedf->value);
					
					if (hasnodef == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = linenum;
						goto END;
					}
					Individual* famind = (Individual*)hasnodef->data;
					fami->wife = famind;
				//	insertBack(&tmpList,otherfiedf);						
				}
				else if (strcmp(otherfiedf->tag,"CHIL") == 0){
					HNode *hasnodef = lookupData(hash,otherfiedf->value);
					
					if (hasnodef == NULL){
						gederr.type = OTHER_ERROR;
						gederr.line = linenum;
						goto END;
					}
					Individual* famind = (Individual*)hasnodef->data;
					insertBack(&fami->children,famind);	

					insertBack(&tmpList,otherfiedf);				
				}				
				othefidf = othefidf->next;
			}
			
			ListIterator listtmp = createIterator(tmpList);
			void *elemof;

			while((elemof = nextElement(&listtmp)) != NULL){
				deleteDataFromList(&fami->otherFields,elemof);
			}
			//clearList(&tmpList);
		}
        nodefam = nodefam->next;
	}


	*obj = tmpobj;

	gederr.type = OK;
	gederr.line = -1;
	goto END;
	
	fclose (data);

	END:return gederr;
}

char* printGEDCOM(const GEDCOMobject* obj){
	char *str;
	//char *famstr;
	char *indstr;
	char buffer[255];
	char sumb[255];
	char *subfield;
	char *finalstring;
	char *charset = NULL ;
	int len = 0,len2 = 0, len3 = 0, len4 = 0,len5 = 0;

	if (obj != NULL)
	{
		if (obj->header->encoding == 0){
			charset = malloc(strlen("ANSEL")+1 *sizeof(char));
			if (charset == NULL){
				return NULL;
			}
			strcpy(charset,"ANSEL");
		}
		else if (obj->header->encoding == 1){
			charset = malloc(strlen("UTF-8")+1 *sizeof(char));
			if (charset == NULL){
				return NULL;
			}
			strcpy(charset,"UTF-8");
		}
		else if (obj->header->encoding == 2){
			charset = malloc(strlen("UNICODE")+1 *sizeof(char));
			if (charset == NULL){
				return NULL;
			}
			strcpy(charset,"UNICODE");
		}
		else if (obj->header->encoding == 3){
			charset = malloc(strlen("ASCII")+1 *sizeof(char));
			if (charset == NULL){
				return NULL;
			}
			strcpy(charset,"ASCII");
		}

		len = sprintf(buffer,"SOURCE:%s\nGEDCVERSION:%0.2f\nCHAR:%s",obj->header->source,obj->header->gedcVersion,charset);
		str = toString(obj->header->otherFields);
		if (str == NULL){
			return NULL;
		}
		len2 = strlen(str);

//		famstr = printFamily((void*)&obj->families);
		
		// if (famstr == NULL){
		// 	return NULL;
		// }

		//len += strlen(famstr);
		if(isListEmpty(obj->individuals)){
			printf ("here\n");
		}

		indstr = toString(obj->individuals);
		if (indstr == NULL){
			return NULL;
		}
		len3 =strlen(indstr);
		
		len4 = sprintf(sumb,"\nNAME %s\n",obj->submitter->submitterName);
		//subfield = printField((void*)&obj->submitter->otherFields);
		subfield = toString(obj->submitter->otherFields);
		if (subfield == NULL){
				return NULL;
		}
		len5 = strlen(subfield);
		finalstring = malloc (sizeof(char)* (len+len2+len3+len4+len5+1));
		if (finalstring == NULL){
			return NULL;
		}
		strcpy(finalstring,buffer);
		strcat(finalstring,str);
		//strcat(finalstring,famstr);
		strcat(finalstring,indstr);
		strcat(finalstring,sumb);
		strcat(finalstring,subfield);


		free(subfield);
		free(indstr);
		//free(famstr);

	}
	else{
		return NULL;
	}

	return finalstring;
}

void deleteGEDCOM(GEDCOMobject* obj){
	Header *tempHead;

	if (obj != NULL)
	{
		if (obj->header != NULL)
		{
			tempHead = obj->header;
			if (tempHead->submitter != NULL)
			{

				Submitter *tempsubmi = tempHead->submitter;
				clearList(&(tempsubmi->otherFields));
				free(tempsubmi);
				tempHead->submitter = NULL;
			}
			clearList(&(tempHead->otherFields));
			free(tempHead);
			obj->header = NULL;
		}
		if (!isListEmpty(obj->families))
		{
			clearList(&(obj->families));
		}
		if (!isListEmpty(obj->individuals))
		{
			clearList(&(obj->individuals));
		}
		free(obj);
	}
	else {
		return;
	}

}

char* printError(GEDCOMerror err){
	char *errstr = NULL;
	char num[5];
	
	switch (err.type)
	{

		case (OK):
			errstr = malloc(strlen("OK")+2 *sizeof(char));
			if (errstr == NULL)
			{
				return NULL;
			}	
			strcpy(errstr,"OK");
			return errstr;
			break;
		case (INV_FILE):
			errstr = malloc(strlen("INV FILE")+1 *sizeof(char));
			if (errstr == NULL)
			{
				return NULL;
			}	
			strcpy(errstr,"INV FILE");
			return errstr;
			break;
		case (INV_GEDCOM):
			errstr = malloc(strlen("INV GEDCOM")+1 *sizeof(char));
			if (errstr == NULL)
			{
				return NULL;
			}	
			strcpy(errstr,"INV GEDCOM");
			return errstr;
			break;
		case (INV_HEADER):	
			sprintf (num,"%d",err.line);
			int size = strlen(num)+ strlen("INV HEADER at line:")+1;
			errstr = malloc (size *sizeof(char));
			if (errstr == NULL){
				return NULL;
			}
			strcpy(errstr,"INV HEADER at line:");
			strcat(errstr,num);	
			return errstr;
			break;	
		case (INV_RECORD):
			sprintf (num,"%d",err.line);
			int sizee = strlen(num)+ strlen("INV RECORD at line:")+1;			
			errstr = malloc (sizee * sizeof(char));
			if (errstr == NULL){
				return NULL;
			}
			strcpy(errstr,"INV RECORD at line:");
			strcat(errstr,num);					
			return errstr;
			break;
		case (WRITE_ERROR):
			errstr = malloc(strlen("WRITE ERROR")+1 *sizeof(char));
			if (errstr == NULL)
			{
				return NULL;
			}	
			strcpy(errstr,"WRITE ERROR");
			return errstr;
			break;				
		case (OTHER_ERROR):
			errstr = malloc(strlen("OTHER ERROR")+1 *sizeof(char));
			if (errstr == NULL)
			{
				return NULL;
			}	
			strcpy(errstr,"OTHER ERROR");
			return errstr;
			break;	
	}
	return NULL;
}

int checkifVaild (int num, char *str){
	
	if (strcmp(str,"HEAD") == 0 && num == 0)
	{
		return 1;
	}
	else if (strcmp(str,"SOUR") == 0 && num == 1){
		return 1; 
	}
	else if (strcmp(str,"VERS") == 0 && (num == 2 || num == 1)){
		return 1; 
	}
	else if (strcmp(str,"GEDC") == 0 && num == 1){
		return 1; 
	}
	else if (strcmp(str,"SUBM") == 0 && num == 1){
		return 1; 
	}
	else if (strcmp(str,"SUBN") == 0 && num == 1){
		return 1; 
	}
	else if (strcmp(str,"CHAR") == 0 && num == 1){
		return 1;
	}
	else {
		return 0;
	}
}
int checkifnumisoff (int num, int prevnum){
	if (num > prevnum +2){
		return 0;
	}
	return 1;
}

void headerParse(char *namstr,char *restr,int srtnum,Header *head){
	if (strcmp(namstr,"SOUR") == 0){
		if (strcmp(restr,"VERS") == 0 && srtnum == 2){
			return;
		}
		else if (strcmp(restr,"NAME") == 0 && srtnum == 2){
			printf ("name\n");
			return;
		}
		else if (strcmp(restr,"CORP") == 0 && srtnum == 2){
			printf ("corp\n");
			return;
		}
		else if (strcmp(restr,"DATE") == 0 && srtnum == 3){
			printf ("date\n");
			return;
		}
		else if (strcmp(restr,"COPR") == 0 && srtnum == 3){
			return;
		}
		else if (strcmp(restr,"CONT") == 0 && srtnum == 4){
			return;
		}
	}
	else if (strcmp(namstr,"HEAD") == 0){
		return;
	}
	else if (strcmp(namstr,"DEST") == 0){
	}
	else if (strcmp(namstr,"GEDC") == 0)
	{
		if (strcmp(restr,"VERS") == 0 && srtnum == 2){
			return;
		}
		else if (strcmp(restr,"FORM") == 0 && srtnum == 2){
			return;
		}
	}
	else if (strcmp(namstr,"CHAR") == 0)
	{
		if (strcmp(restr,"VERS") == 0 && srtnum == 2){
			return;
		}
	}
	else if (strcmp(namstr,"NOTE") == 0){
		if (strcmp(restr,"CONC") == 0 && srtnum == 2){
			return;
		}
	}
	else if (strcmp(namstr,"PLAC") == 0){
		if (strcmp(restr,"FORM") == 0 && srtnum == 2){
			return;
		}	
	}
	else if (strcmp(namstr,"DATE") == 0){
	}
	else if (strcmp(namstr,"FILE") == 0){
	}
	else if (strcmp(namstr,"COPR") == 0){
	}
	else if (strcmp(namstr,"LANG") == 0){
	}
	else if (strcmp(namstr,"SUBM") == 0){
	}
	else if (strcmp(namstr,"SUBN") == 0){
	}
}
int compareIndividuals(const void* first,const void* second){
	int diff = 0;

	char *firststring, *secstring;
	Individual * tmpindi;
	Individual *tmpindit;

	if (first == NULL && second == NULL){
		return 0;
	}
	if (first != NULL && second == NULL){
		return 1;
	}
	if (first == NULL && second != NULL){
		return -1;
	}

	tmpindi = (Individual*)first;

	int sizee = strlen(tmpindi->givenName) + strlen(tmpindi->surname) + 200;
	firststring = malloc(sizee * sizeof(char));
	
	if (firststring == NULL){
		return 0;
	}

	strcpy(firststring,tmpindi->surname);
	strcat(firststring,",");
	strcat(firststring,tmpindi->givenName);


	tmpindit = (Individual*)second;

	int sizeet = strlen(tmpindit->givenName) + strlen(tmpindit->surname) + 200;
	secstring = malloc(sizeet * sizeof(char));
	
	if (secstring == NULL){
		return 0;
	}

	strcpy(secstring,tmpindit->surname);
	strcat(secstring,",");
	strcat(secstring,tmpindit->givenName);
	
	diff = strcmp(firststring,secstring);

	if (diff == 0){

		int evenlen2 = tmpindit->events.length;
		int evenlen1 = tmpindi->events.length;
		int othefie1 = tmpindi->otherFields.length;
		int othefie2 = tmpindit->otherFields.length;

		int differenceEvent = evenlen2 - evenlen1;
		int differenceFields = othefie2 - othefie1;

		if (differenceEvent == 0 && differenceFields == 0) {
			
			return 0;
		} else if (differenceFields != 0) {
			return differenceFields;
		} else if (differenceEvent != 0) {
			return differenceEvent;
		}

	}
	return diff;
   
}
void deleteIndividual(void* toBeDeleted){
	Individual *tmpindi;
	if (toBeDeleted == NULL){
		return;
	}
	tmpindi = (Individual*)toBeDeleted;
	
	if (!isListEmpty(tmpindi->otherFields)){
		clearList(&tmpindi->otherFields);
	}
	if (!isListEmpty(tmpindi->events)){
		clearList(&tmpindi->events);
	}
	free(tmpindi->surname);
	free(tmpindi->givenName);
	free(tmpindi); 	
}

char* printIndividual(void* toBePrinted){
	Individual *tmpindi;
	char *finalstring;

	if (toBePrinted == NULL){
		return NULL;
	}
	tmpindi = (Individual*) toBePrinted;
	char *fieldstring = toString(tmpindi->otherFields);
	char *evenstring = toString(tmpindi->events);
	int size =strlen(tmpindi->givenName)+strlen(tmpindi->surname)+strlen(evenstring)+strlen(fieldstring)+10;

	finalstring = malloc (sizeof(char) * size);
	if (finalstring == NULL){
		return NULL;
	}

	strcpy (finalstring,tmpindi->givenName);
	strcat (finalstring," ");
	strcat (finalstring,tmpindi->surname);
	strcat (finalstring," ");
	strcat (finalstring,fieldstring);
	strcat (finalstring," ");	
	strcat (finalstring,evenstring);
	strcat (finalstring," ");	

	return finalstring;

}

void deleteField(void* toBeDeleted){
	Field *tmpfield; 

	tmpfield = (Field*) toBeDeleted;
	if (tmpfield == NULL){
		return;
	}
	free(tmpfield->value);
	free(tmpfield->tag);
	free(tmpfield);
}
int compareFields(const void* first,const void* second){
	int diff = 0;

	char *firststring, *secstring;
	Field * tmpfield;
	Field *tmpfieldt;

	if (first == NULL && second == NULL){
		return 0;
	}
	else if (first == NULL && second != NULL){
		return -1;
	}
	else if (first != NULL && second == NULL){
		return 1;
	}
	else{
		tmpfield = (Field*)first;
		int sizee = strlen(tmpfield->tag) + strlen(tmpfield->value) + 2;
		firststring = malloc(sizee * sizeof(char));

		strcpy(firststring,tmpfield->tag);
		strcat(firststring," ");
		strcat(firststring,tmpfield->value);

		tmpfieldt = (Field*)second;

		int sizeet = strlen(tmpfieldt->tag) + strlen(tmpfieldt->value) + 2;
		secstring = malloc(sizeet * sizeof(char));

		strcpy(secstring,tmpfieldt->tag);
		strcat(secstring," ");
		strcat(secstring,tmpfieldt->value);

		diff = strcmp(firststring,secstring);
		free(secstring);
		free(firststring);
	}
	return diff;

}
char* printField(void* toBePrinted){
	Field *tmpfield;
	char *str1;
	char *str2;
	char *finalstring;

	if (toBePrinted == NULL){
		return NULL;
	}
	tmpfield = (Field*) toBePrinted;
	str1 = tmpfield->tag;
	str2 = tmpfield->value;

	int size = strlen(str1) + strlen(str2)+ 2;

	finalstring = malloc (size *sizeof(char));

	strcpy (finalstring,str1);
	strcat (finalstring," ");
	strcat (finalstring, str2);

	free(tmpfield);

	return finalstring;
}
void deleteEvent(void* toBeDeleted){
	Event *tmpevent;
	tmpevent = (Event*) toBeDeleted;
	if (toBeDeleted == NULL){
		return;
	}
	tmpevent = (Event *)toBeDeleted;
	if (!isListEmpty(tmpevent->otherFields)){
		clearList(&(tmpevent->otherFields));
	}

	free(tmpevent->place);
	free(tmpevent->date);
	free(tmpevent->type);
	free(tmpevent);
}

int compareEvents(const void* first,const void* second){
	Event *event,*events;
	if (first == NULL && second == NULL){
		return 0;
	}
	if (first != NULL && second == NULL){
		return 1;
	}
	if (first == NULL && second != NULL){
		return -1;
	}

	event = (Event*)first; 
	events =(Event*)second;  

	//printf ("%s %s\n", event -> type, events -> type);


	int diff = strcmp(event -> type, events -> type);
	if (diff == 0){

		int datediff = strcmp(event->date,events->date);
		if (datediff == 0){
			return 0;
		}
			return datediff;
	}


	return diff;
}

char* printEvent(void* toBePrinted){
	Event *tmpevent;
	char *finalstring;

	if (toBePrinted == NULL){
		return NULL;
	}
	tmpevent = (Event*) toBePrinted;

	char *fieldstring = toString(tmpevent->otherFields);
	int size = strlen(tmpevent->date)+strlen(tmpevent->place)+strlen(tmpevent->type)+strlen(fieldstring)+4;

	finalstring = malloc (size *sizeof(char));
	if (finalstring == NULL)
	{
		return NULL;
	}
	strcpy(finalstring,tmpevent->type);
	strcat(finalstring," ");
	strcat(finalstring,tmpevent->date);
	strcat(finalstring," ");
	strcat(finalstring,tmpevent->place);
	strcat(finalstring," ");
	strcat(finalstring,fieldstring);

	free(fieldstring);
	free(tmpevent);

	return finalstring;
}

void deleteFamily(void* toBeDeleted){
	Family *tmpfams; 
	tmpfams = (Family *) toBeDeleted;
	if (toBeDeleted == NULL){
		return ;
	}
	if (!isListEmpty(tmpfams->children)){
		clearList(&(tmpfams->children));
	}
	if (!isListEmpty(tmpfams->events)){
		clearList(&(tmpfams->events));
	}
	if (!isListEmpty(tmpfams->otherFields)){
		clearList(&(tmpfams->otherFields));
	}
	free(tmpfams);
}

int compareFamilies(const void* first,const void* second){
	int famcount = 0;
	int famtcount = 0;
	int diff = 0;
	Family* tmpfam, *tmpfams;

	void * elem;
	void * elems;

	tmpfam = (Family *)first;
	tmpfams = (Family *)second;
	
	ListIterator iter = createIterator (tmpfam->children);

	ListIterator ite = createIterator (tmpfams->children);
	
	if (first == NULL && second == NULL){
		return 0;
	}
	else if (first != NULL && second == NULL){
		return 1;
	}
	else if (first == NULL && second != NULL){
		return -1;
	}
	else {
		if (tmpfam->wife != NULL){
			famcount++;
		}
		if (tmpfam->husband != NULL){
			famcount++;
		}
		while ((elem = nextElement(&iter)) != NULL){
			famcount++;
		}

		if (tmpfams->wife != NULL){
			famtcount++;
		}
		if (tmpfams->husband != NULL){
			famtcount++;
		}
		while ((elems = nextElement(&ite)) != NULL){
			famtcount++;
		}

		if (famcount > famtcount){
			diff = 1;
		}
		else if (famtcount > famcount){
			diff = -1;
		}
		else if (famcount == famtcount){
			diff = 0;
		}
	}
	return diff;
}	

char* printFamily(void* toBePrinted){
	char *finalstring = NULL;
	char *wifestr = NULL, *husstr = NULL;
	char *lischil = NULL, *eventf = NULL, *othefi = NULL;

	Family *tmpfams = (Family *) toBePrinted;
	
	if (tmpfams->wife != NULL){
		//wifestr = printIndividual((void*)tmpfams->wife);
		if (wifestr == NULL){
			return NULL;
		}
	}
	if (tmpfams->husband != NULL){
		husstr = printIndividual((void*)tmpfams->husband);
		if (husstr == NULL){
			return NULL;
		}
	}

	lischil = toString(tmpfams->children);
	eventf = toString(tmpfams->events);
	othefi = toString(tmpfams->otherFields);

	int sizee = strlen(wifestr) + strlen(husstr) + strlen(lischil) + strlen(eventf) + strlen(othefi) + 5;
	
	finalstring = malloc (sizee * sizeof(char));
	if (finalstring == NULL){
		return NULL;
	}

	strcpy(finalstring,wifestr);
	strcat(finalstring,"\n");
	strcat(finalstring,husstr);
	strcat(finalstring,"\n");
	strcat(finalstring,lischil);
	strcat(finalstring,"\n");
	strcat(finalstring,eventf);
	strcat(finalstring,"\n");
	strcat(finalstring,othefi);

	free(husstr);
	free(wifestr);

	return finalstring;
}
bool compare(const void* first, const void* second){
	char *tmp; 
	char *tmpf; 
	int num = 0;
	tmp = (char*)first;
	tmpf = (char*)second;

	num = strcmp(tmp,tmpf);

	return num;
}

bool isListEmpty(List list){
	return list.head == NULL && list.tail == NULL;
}

int checkforupper(char *restr){
	int size = strlen(restr);
	for (int i = 0; i < size; i++){
		if ((restr[i] >= 'A' && restr[i] <= 'Z')){
		}
		else if (restr[i] == '@' ){
		}
		else if (restr[i] >= '0' && restr[i] <= '9'){
		}
		else{
			return 0;
		}
	}
	return 1;
}
Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person){
	bool num;
	
	if (familyRecord == NULL){
		return NULL;
	}
	List  tmpIndL = familyRecord->individuals;
	if (tmpIndL.head == NULL){
		return NULL;
	}
	ListIterator iter = createIterator(tmpIndL);

	void *data;

	while((data = nextElement(&iter)) != NULL){
		Individual *tmpindo = (Individual*)data;
		num = compare((void*) tmpindo, person);
		if (num == true){
			return (Individual*)data;
		}
	}

	return NULL;
}
bool compareIndividualb(const void* first, const void* second){
	if (first == second){
		return true;
	}
	if (first == NULL || second == NULL){
		return false;
	}
	bool val;

	char *firststring, *secstring;
	Individual * tmpindi;
	Individual *tmpindit;

	tmpindi = (Individual*)first;

	int sizee = strlen(tmpindi->givenName) + strlen(tmpindi->surname) + 2;
	firststring = malloc(sizee * sizeof(char));
	if (firststring == NULL){
		return 0;
	}
	strcpy(firststring,tmpindi->surname);
	strcat(firststring,",");
	strcat(firststring,tmpindi->givenName);

	tmpindit = (Individual*)second;

	int sizeet = strlen(tmpindit->givenName) + strlen(tmpindit->surname) + 2;
	secstring = malloc(sizeet * sizeof(char));

	strcpy(secstring,tmpindit->surname);
	strcat(secstring,",");
	strcat(secstring,tmpindit->givenName);

	val = strcmp(firststring,secstring);

	free(secstring);
	free(firststring);
	free(tmpindi);
	free(tmpindit);

	return val;
}

void getChild(Individual *ind, List *list){
	
	ListIterator fam = createIterator(ind->families);	
	void *data;
	while((data = nextElement(&fam)) != NULL){
		Family *tmpfam = (Family *)data;
		int numw = 0, num =0; 
		int lasnam = 0,lastname = 0;

		numw = strcmp(ind->givenName,tmpfam->husband->givenName);
		lasnam = strcmp(ind->surname,tmpfam->husband->surname);
		num = strcmp(ind->givenName,tmpfam->wife->givenName);
		lastname = strcmp(ind->surname,tmpfam->wife->surname);
		
		if ((numw == 0 && lasnam == 0) || (num == 0 && lastname == 0)){

			if (getLength(tmpfam->children) <= 0){
				return;
			}
			
			ListIterator chil = createIterator(tmpfam->children);
			
			void *elem;
			while((elem = nextElement(&chil)) != NULL){
				Individual * tmpchild = (Individual *)elem;
				
				getChild(tmpchild,list);
				insertBack(list, tmpchild);
			}
		}
	}
}

List getDescendants(const GEDCOMobject* familyRecord, const Individual* person){
	
	List list = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
	
	if (familyRecord == NULL || person == NULL){
		return list;
	}

	Individual *in = findPerson(familyRecord,&compareIndividualb,person);

	if (in == NULL){
		return list;
	}
	getChild(in,&list);

	return list;
}
void deleteGeneration(void* toBeDeleted){
	if (toBeDeleted == NULL){
		return;
	}
}
int compareGenerations(const void* first,const void* second){
	if (first == NULL && second == NULL){
		return 0;
	}
	if (first == second){
		return 0;
	}
	if (first == NULL && second != NULL){
		return -1;
	}
	if (second == NULL && first != NULL){
		return 1;
	}
	return 0;
}
char* printGeneration(void* toBePrinted){
	if (toBePrinted == NULL){
		return NULL;
	}
	return NULL;
}
GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj){
	GEDCOMerror err;
	char *ext;
	FILE *fw;
	char vers[10];
	char *charset = NULL;
	char *evenstr = NULL,*fieldstr = NULL;

	if (fileName == NULL){
		 err.type = WRITE_ERROR;
		 err.line = -1;
		 return err;
	}
	if (obj == NULL){
		 err.type = WRITE_ERROR;
		 err.line = -1;
		 return err;
	}
	ext = &fileName[(strlen(fileName)-4)];
	if (strcmp(ext,".ged") != 0){
		 err.type = WRITE_ERROR;
		 err.line = -1;
		 return err;
	}

	ErrorCode error = validateGEDCOM(obj);

	if (error != OK){
		
		err.type = WRITE_ERROR;
		err.line = -1;
		return err; 

	}

	fw = fopen(fileName,"w");
	if (fw == NULL){
		 err.type = WRITE_ERROR;
		 err.line = -1;
		 return err;
	}

	sprintf (vers,"%0.1f", obj->header->gedcVersion);
	if (obj->header->encoding == 0){
		charset = malloc(strlen("ANSEL")+1 *sizeof(char));
		if (charset == NULL){
		 	err.type = WRITE_ERROR;
		 	err.line = -1;
		 	return err;
		}
		strcpy(charset,"ANSEL");
	}
	else if (obj->header->encoding == 1){
		charset = malloc(strlen("UTF-8")+1 *sizeof(char));
		if (charset == NULL){
			 err.type = WRITE_ERROR;
			 err.line = -1;
			 return err;
		}
		strcpy(charset,"UTF-8");
	}
	else if (obj->header->encoding == 2){
		charset = malloc(strlen("UNICODE")+1 *sizeof(char));
		if (charset == NULL){
			err.type = WRITE_ERROR;
			err.line = -1;
			return err;
		}
		strcpy(charset,"UNICODE");
	}
	else if (obj->header->encoding == 3){
		charset = malloc(strlen("ASCII")+1 *sizeof(char));
		if (charset == NULL){
			err.type = WRITE_ERROR;
			err.line = -1;
			return err;
		}
		strcpy(charset,"ASCII");
	}
	int count = 0;
	fprintf (fw, "0 HEAD\n1 SOUR %s\n1 GEDC\n2 VERS %s\n1 CHAR %s\n2 FORM LINEAGE-LINKED\n1 SUBM @U%d@\n", obj->header->source,vers,charset,++count);

	if (strcmp(obj->submitter->address,"\0") == 0){
		fprintf (fw, "0 @U%d@ SUBM\n1 NAME %s\n",count,obj->submitter->submitterName);
	}
	else {
		fprintf (fw, "0 @U%d@ SUBM\n1 NAME %s\n1 ADDR %s\n",count,obj->submitter->submitterName,obj->submitter->address);	
	}

	if (getLength(obj->individuals) > 0){
		
		ListIterator ind = createIterator(obj->individuals);
		void *data;
		
		int count = 0;

		while ((data = nextElement(&ind)) != NULL){
			Individual *tmpind = (Individual *) data;
			

			fprintf(fw,"0 @I0%d@ INDI\n",++count);	
			fprintf(fw,"1 NAME %s /%s/\n",tmpind->givenName, tmpind->surname);
			
			if (getLength(tmpind->otherFields) > 0){
				fieldstr = writeField(tmpind->otherFields);
				fprintf (fw,"%s",fieldstr);
			}
			if (getLength(tmpind->events) > 0){
				evenstr = writeEvent(tmpind->events);
				fprintf (fw,"%s",evenstr);
			}
			
			if (getLength(obj->families) > 0){
				ListIterator tmpf = createIterator(tmpind->families);
				void *famd; 

				while((famd = nextElement(&tmpf)) != NULL){
	
					ListIterator fam = createIterator(obj->families);
					void *mainfam;
					int famco = 1;
					
					while((mainfam = nextElement(&fam)) != NULL){
			
						Family *famsut = (Family *) mainfam;

						int num = compareFamilies(famd,mainfam);

						if (num == 0){	
		
							if (compareIndividuals(famsut->husband,data) == 0 || compareIndividuals(famsut->wife,data) == 0){
								fprintf(fw,"1 FAMS @F0%d@\n",famco);
								break;
							}
							ListIterator child = createIterator(famsut->children);
							void *chil;
							
							while((chil = nextElement(&child)) != NULL){
							
								if(compareIndividuals(chil,data) == 0){
									fprintf(fw, "1 FAMC @F0%d@\n",famco);
									break;
								}
							}
						}
						famco++;
					}
				}
			}

		}
	}

	if (getLength(obj->families) > 0 ){
		ListIterator fam = createIterator(obj->families);
		void *data;
		int famcount = 1;
		while ((data= nextElement(&fam)) != NULL){
			
			Family *tmpfam = (Family *)data;
			fprintf (fw,"0 @F0%d@ FAM\n",famcount);
			
			ListIterator ind = createIterator(obj->individuals);
			void *indi;
			int huscunt = 1;

			bool foundhus = false;
			bool foundwife = false;

			while((indi = nextElement(&ind)) != NULL){
	

				Individual *tmpind = (Individual *)indi;
				
				if(compareIndividuals(tmpfam->husband,tmpind) == 0 ){
					foundhus = true;
					fprintf(fw,"1 HUSB @I0%d@\n",huscunt);

				}
				else if (compareIndividuals(tmpfam->wife,tmpind) == 0){
					foundwife = true;
					fprintf(fw,"1 WIFE @I0%d@\n",huscunt);
				}

				if (foundwife == true && foundhus == true) {
					char *eventstr = NULL;
					if(getLength(tmpfam->events) > 0){
						eventstr = writeEvent(tmpfam->events);
						fprintf(fw,"%s",eventstr);
					}

					foundwife = false;
					foundhus = false;

				}
	
				if (getLength(tmpfam->children) > 0){

					ListIterator chil = createIterator(tmpfam->children);
					void *child;

					while((child = nextElement(&chil)) != NULL){
						
						if (compareIndividuals(child,tmpind) == 0){
							Individual* i1 = (Individual*) child;
							Individual* i2 = (Individual*) tmpind;

							if (getLength(i1 -> events) > 0 && getLength(i2 -> events) > 0) {					
								
									fprintf(fw,"1 CHIL @I0%d@\n",huscunt);
									break;
							} 	
						}
					}
				}
				huscunt++;
			}			
				
			famcount++;
		}
	}

	fprintf (fw,"0 TRLR\n");
	
	fclose(fw);
	

	err.type = OK;
	err.line = -1;

	return err;
}
char *writeField (List list){
	char *line = NULL;
	
	ListIterator field = createIterator(list);
	void *data;
	
	line = malloc (sizeof(char)* 1);
	strcpy(line,"");

	while((data = nextElement(&field))!= NULL){
		Field *tmpfield = (Field *) data;
		if (strcmp(tmpfield->tag,"GIVN") == 0 || strcmp(tmpfield->tag,"SURN") == 0){
			line = realloc(line,sizeof(char) * strlen(tmpfield->tag)+ strlen(tmpfield->value) + 50);
			strcat (line, "2 ");
			strcat (line,tmpfield->tag);
			strcat (line," ");
			strcat (line,tmpfield->value);
			strcat (line,"\n");
		}
		if (strcmp(tmpfield->tag,"RESN") == 0 || strcmp(tmpfield->tag,"SEX") == 0 || 
			strcmp(tmpfield->tag,"RFN") == 0 || strcmp(tmpfield->tag,"AFN") == 0 ||
			strcmp(tmpfield->tag,"RIN") == 0){
			line = realloc (line,sizeof(char) * strlen(tmpfield->tag) + strlen(tmpfield->value)+ 20);
			strcat (line,"1 ");
			strcat(line,tmpfield->tag);
			strcat(line," ");
			strcat(line,tmpfield->value);
			strcat(line,"\n");
		}
	}
	return line;
}
char * writeEvent (List list){
	char *temp = NULL;
	
	temp = malloc(sizeof(char) * 1);
	strcpy(temp,"");

	ListIterator event = createIterator(list);
	void *data;
	while ((data = nextElement(&event)) != NULL){
		
		Event *tmpevent = (Event *)data;
		temp = realloc (temp,sizeof(char) * strlen(tmpevent->type) + strlen(tmpevent->date) + strlen(tmpevent->place)+ 200);
		strcat(temp,"1 ");
		strcat(temp,tmpevent->type);
		strcat(temp,"\n");
		
		if (strcmp(tmpevent->date,"") != 0){
			strcat(temp,"2 ");
			strcat(temp,"DATE ");
			strcat(temp,tmpevent->date);
			strcat(temp,"\n");
		}

		if (strcmp(tmpevent->place,"") != 0){
			strcat(temp,"2 ");
			strcat(temp,"PLAC ");
			strcat(temp,tmpevent->place);
			strcat(temp,"\n");
		}
	}
	printf ("%s\n",temp);
	return temp;
}
ErrorCode validateField (List list){
	//ListIterator field = createIterator(list);
	///void* data;
	// while ((data = nextElement(&field)) != NULL){
	// 	//Field *field = (Field *) data;
	// 	// if (field->tag == NULL || field->value == NULL || strcmp(field->tag,"") == 0 ||
	// 	// 	strcmp(field->value,"") == 0)
	// 	// {
	// 	// 	printf ("infield  here\n");
	// 	// 	return INV_RECORD;
	// 	// } 
	// }
	return OK;
}
ErrorCode validateEvents(List list){
	ListIterator event = createIterator(list);
	void* data;
	while((data = nextElement(&event)) != NULL){

		Event *tmpevent = (Event *) data;
		if (tmpevent->date == NULL || tmpevent->place == NULL){
			return INV_RECORD;
		}
		else{
			ErrorCode error = validateField(tmpevent->otherFields);
			if (error != OK){
				return error;
			}
		}
	}
	return OK;
}
ErrorCode validateIndividual(List list){
	ErrorCode error;
	ListIterator iter = createIterator(list);
	void* elem;

	while((elem = nextElement(&iter)) != NULL){
		Individual *tmpindo = (Individual *)elem;
		
		if(tmpindo->givenName == NULL || tmpindo->surname == NULL || getLength(tmpindo->events) <= -1 
			|| getLength(tmpindo->families) <= -1 || getLength(tmpindo->otherFields) <= -1){
		
			return INV_RECORD;
		}
		else if (getLength(tmpindo->events) > 0){
			
			error = validateEvents(tmpindo->events);
			if (error != OK){
				return error;
			}
		}
		 else if (getLength(tmpindo->otherFields) > 0){

			error = validateField(tmpindo->otherFields);
			if (error != OK){
				return error;
			}
		}	

	}
	return OK;
}

ErrorCode validatefamilies (List list){
	
	ListIterator itr = createIterator(list);
		void *data;

	while((data = nextElement(&itr)) != NULL){
		Family *fam = (Family *) data;
		if (getLength(fam->children) <= -1 || getLength(fam->events) <= -1 || getLength(fam->otherFields) <= -1){
			return INV_RECORD;
		}
		else if (getLength(fam->children) > 0){
			ErrorCode error = validateIndividual(fam->children);
			if (error != OK){
				return error;
			}
		}
		else if (getLength(fam->events) > 0){
			ErrorCode error = validateEvents(fam->events);
			if (error != OK){
				return error;
			}
		}
		else if (getLength(fam->otherFields) > 0){
			ErrorCode error = validateField(fam->otherFields);
			if (error != OK){
				return error;
			}
		}
	}
	return OK;
}
ErrorCode validateGEDCOM(const GEDCOMobject* obj){
	
	ErrorCode error;

	if (obj == NULL){
		return INV_GEDCOM;
	}
	else{
		
		if (obj->header == NULL || obj->submitter == NULL){	
			return INV_GEDCOM;
		}
		
		if (obj->header != NULL){
			if (strlen(obj->header->source) != 0 && (obj->header->encoding == 0 || obj->header->encoding == 1 
				|| obj->header->encoding == 2 || obj->header->encoding == 3) && obj->header->gedcVersion >= 0.0 && obj->header->submitter != NULL){
			}
			else {
				return INV_HEADER;
			}
		}
		else {
			return INV_HEADER;
		}

		if (obj->submitter == NULL){	
			return INV_RECORD;
		}
		else{
			if (strlen(obj->submitter->submitterName) == 0){			
				return INV_RECORD;
			}
			if (strlen(obj->submitter->submitterName) > 60){
				return INV_RECORD;
			}
			if (getLength(obj->submitter->otherFields) <= -1){	
				return INV_RECORD;
			}
		}
		
		if (getLength (obj->individuals) <= -1){

			return INV_RECORD;
		}
		else if (getLength(obj->individuals) > 0){
			
			error = validateIndividual(obj->individuals);
	
			if (error != OK){
				return error;
			}
			ListIterator iter = createIterator(obj->individuals);
			void *data;
			while((data = nextElement(&iter)) != NULL){
				Individual *tmpind = (Individual *) data;
				if (getLength(tmpind->families) > 0){
					ErrorCode gerror = validatefamilies(tmpind->families);
					if(gerror != OK){
						return gerror;
					}
				}
			}
		}

		if (getLength (obj->families) <= -1){
			return INV_RECORD;
		}
		else if (getLength(obj->families) > 0){
			error = validatefamilies(obj->families);
			if (error != OK){
				return error;
			}
		}
	}

	return OK;
}
char* indToJSON(const Individual* ind){
	char *jsonstring = NULL;

	if (ind == NULL){
		jsonstring = malloc(sizeof(char) * 10);
	 	strcpy(jsonstring,"\"{""");
		strcat(jsonstring,"\"");
		strcat(jsonstring,"\"");
		strcat(jsonstring,"}");
		strcat(jsonstring,"\"");
	}
	else if (ind->givenName != NULL || ind->surname != NULL){
	 
	 	jsonstring = malloc (sizeof(char) * strlen(ind->givenName) + strlen(ind->surname)+ 200);
	 	
	 	strcpy(jsonstring,"{");
	 	strcat(jsonstring,"\"givenName\"");
	 	strcat(jsonstring,":");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,ind->givenName);
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,",");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,"surname");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,":");
	 	strcat(jsonstring,"\"");	 	
	 	strcat(jsonstring,ind->surname);
	 	strcat(jsonstring,"\"");	 	
	 	strcat(jsonstring,"}");
	 }

	return jsonstring;
}

Individual* JSONtoInd(const char* str){
	int count  = 0, count2 = 0;

	if (strlen(str) == 0){
		return NULL;
	}

	char *jsonstr = NULL;
	jsonstr = malloc(sizeof(char) * strlen(str) + 1);
	strcpy(jsonstr,str);

	int size = strlen(jsonstr);
	
	for (int i = 0; i < size; i++){
		if (jsonstr[i] == 44){
			count++;
		}
		if (jsonstr[i] == 58){
			count2++;
		}
	}

	if (count > 1 || count == 0){
		return NULL;
	}
	if (count2 != 2){
		return NULL;
	}
	Individual *tmpind = malloc(sizeof(Individual));
	if (tmpind == NULL){
		return NULL;
	}
	tmpind->events = initializeList(&printEvent,&deleteEvent,&compareEvents);
	tmpind->otherFields = initializeList(&printField,&deleteField,&compareFields);
	tmpind->families = initializeList(&printFamily,&deleteFamily,&compareFamilies);

	char *str2 = strtok(jsonstr,"{");
 	str2 = strtok(str2,"}");
 	
 	char *fullstr = strtok(str2,",");

  	char *firsthalf = malloc(sizeof(char) * strlen(fullstr)+ 1);
  	strcpy(firsthalf,fullstr);
  	
  	fullstr = strtok(NULL,"");

  	int sur = 0;
  	for(int i = 0; i < strlen(fullstr); i++){
  		if (fullstr[i] == 58){
  			if(fullstr[i+1] == 34 && fullstr[i+2] == 34){
  				sur = 1;
  			}
  		}
  	}
  	printf ("%d\n",sur);
  	fullstr = strtok(fullstr,":");
  	if (strcmp(fullstr,"\"surname\"") == 0){
  		fullstr = strtok(NULL,"\"");

  		if (fullstr == NULL) {
  			if (sur == 1){
  			tmpind->surname = malloc(sizeof(char) * 1);
  			strcpy(tmpind->surname,"\0");
 			} 
 			else {
 				return NULL;
 			}		
  		}
  		else {
  			tmpind->surname = malloc(sizeof(char) * strlen(fullstr) + 1);
  			strcpy(tmpind->surname,fullstr);
  		
  		}
  	}
  	else{
  		return NULL;
  	}

  	int giv = 0;
  	for(int i = 0; i < strlen(firsthalf); i++){
  		if (firsthalf[i] == 58){
  			if(firsthalf[i+1] == 34 && firsthalf[i+2] == 34){
  				giv = 1;
  			}
  		}
  	}
  	firsthalf = strtok(firsthalf,":");
	if (strcmp(firsthalf,"\"givenName\"") == 0){
		
		firsthalf = strtok(NULL,"\"");
		
		if (firsthalf == NULL){
			if (giv == 1){
			tmpind->givenName = malloc(sizeof(char)+ 1);
  			strcpy(tmpind->givenName,"\0");
			}
			else{
				return NULL;
			}  			
		}
		else {
			tmpind->givenName = malloc(sizeof(char) * strlen(firsthalf) + 1);
  			strcpy(tmpind->givenName,firsthalf);
		}
	}
	else{
		return NULL;
	}

	return tmpind;
}
GEDCOMobject* JSONtoGEDCOM(const char* str){
	char *gedobj = NULL;

	gedobj = malloc(sizeof(char) * strlen(str) + 2);
	strcpy(gedobj,str);

	if (gedobj == NULL){
		return NULL;
	}

	int count = 0,count2 = 0;
  	int size = strlen(gedobj);
  
  	for (int i = 0; i <size; i++){
  		if(gedobj[i] == 44){
  			count++;
  		}
  		if (gedobj[i] == 58){
  			count2++;
  		}
  	}

  	if (count != 4){
  		return NULL;
  	}
  	if (count2 != 5){
  		return NULL;
  	}

  	GEDCOMobject *obj  = malloc (sizeof(GEDCOMobject));
  	if (obj == NULL){
  		return NULL;
  	}
  	
  	obj->individuals = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  	obj->families = initializeList(&printFamily,&deleteFamily,&compareFamilies);

  	char *str2 = strtok(gedobj,"{");
  	str2 = strtok(str2,"}");

  	char *sour = strtok(str2,",");
  	char *vers = strtok(NULL,",");
  	char *charset = strtok(NULL,",");
  	char *subnam = strtok(NULL,",");
  	char *subadd = strtok(NULL,"");
	
	obj->header = malloc(sizeof(Header));
	if (obj->header == NULL){
		return NULL;
	}
  	obj->header->otherFields = initializeList(&printField,&deleteField,&compareFields);

  	sour = strtok(sour,":");
  	if (strcmp(sour,"\"source\"") == 0){
  		sour = strtok(NULL,"\"");
  		if (sour == NULL){
  			return NULL;
  		}
  		else {

  			strcpy(obj->header->source,sour);
  		}
  	}
  	else {
  		return NULL;
  	}

  	vers = strtok(vers,":");
  	if (strcmp(vers,"\"gedcversion\"") == 0){
  		vers = strtok(NULL,"\"");
  	  	if (vers == NULL){
  			return NULL;
  		}
  		else {

  			float ver = atof(vers);
  			obj->header->gedcVersion = ver;
  		}	
  	}
  	 else {
  		return NULL;
  	}

  	charset = strtok(charset,":");
  	if (strcmp(charset,"\"encoding\"") == 0){
  		charset = strtok(NULL,"\"");
  		if (charset == NULL){
  			return NULL;
  		}
  		else {
  			if (strcmp(charset,"ANSEL") == 0){
				obj->header->encoding = ANSEL;
			}
			else if (strcmp(charset,"UTF-8") == 0){
				obj->header->encoding = UTF8;
			}
			else if (strcmp(charset,"UNICODE") == 0){
				obj->header->encoding = UNICODE;
			}
			else if (strcmp(charset,"ASCII") == 0){
				obj->header->encoding = ASCII;
			}
			else {
				return NULL;
			}

  		}
  	}
   	else {
  		return NULL;
  	}

  	char *subname = NULL;

  	subnam = strtok(subnam,":");
  	if (strcmp(subnam,"\"submitterName\"") == 0){
  		subnam = strtok(NULL,"\"");

    	if (subnam == NULL){
  			return NULL;
  		}
  		else {
  			subname = malloc(sizeof(char) * strlen(subnam) + 2);
  			strcpy(subname,subnam);
  		}		
  	}
   	else {
  		return NULL;
  	} 
	int subads = 0;

	for(int i = 0; i < strlen(subadd); i++){
  		if (subadd[i] == 58){
  			if(subadd[i+1] == 34 && subadd[i+2] == 34){
  				subads = 1;
  			}
  		}
  	}
  	
  	subadd = strtok(subadd,":");
  	if (strcmp(subadd,"\"submitterAddress\"") == 0){
  		subadd = strtok(NULL,"\"");
  		if (subadd == NULL){
  			if (subads == 1){
  				int size = 1;
  				Submitter *tmpsub = malloc (sizeof(Submitter)+ sizeof(char) * size + 1);	
				tmpsub->otherFields = initializeList(&printField,&deleteField,&compareFields);
				if (tmpsub != NULL){
					strcpy(tmpsub->submitterName,subname);
					strcpy(tmpsub->address,"\0");
				}
				obj->submitter = tmpsub;	
				obj->header->submitter = tmpsub;		
  			}
  			else{
  				return NULL;
  			}

  		}
  		else {
  			Submitter *tmpsub = malloc(sizeof(Submitter)+ sizeof(char) * strlen(subadd) + 2);
  			tmpsub->otherFields = initializeList(&printField,&deleteField,&compareFields);
  			if (tmpsub != NULL){
  				strcpy(tmpsub->submitterName,subname);
  				strcpy(tmpsub->address,subadd);
  			}
  			obj->submitter = tmpsub;
  			obj->header->submitter = tmpsub;
  		}
  	}
   	else {
  		return NULL;
  	} 	

  	return obj;

}

char* iListToJSON(List iList){
	char *final ;
	if (getLength(iList) <= -1){
		final = malloc(sizeof(char) * 2);
		strcpy(final,"\0");
		return final;
	}

	if (getLength(iList) == 0){
		final = malloc(sizeof(char) * 5);
		strcpy(final,"[]");
		return final;
	}
	final = malloc(sizeof(char));
	strcpy(final,"");

	final = malloc(sizeof(char) * 3);
	strcat(final,"[");
	
	int count = 0;

	ListIterator itr = createIterator(iList);
	void *dataa;
	while((dataa = nextElement(&itr)) != NULL){
		count++;
	}
	int countt = 1;
	ListIterator iter = createIterator(iList);
	void *data;
	while((data = nextElement(&iter)) != NULL){
		Individual *tmpind  = (Individual *) data;
		
		char *indstr = indToJSON(tmpind);
		final = realloc(final,sizeof(char) * strlen(indstr)+ 200);
		strcat(final,indstr);
		if (countt < count){
			strcat(final,",");
		}
		countt++;
	}

	strcat (final,"]");
	return final;
}
char* gListToJSON(List gList){
	char *final = NULL;
	int count = 0,count1 = 1;
	if (getLength(gList) <= -1){
		final = malloc(sizeof(char) * 2);
		strcpy(final,"\0");
		return final;
	}
	if (getLength(gList) == 0){
		final = malloc(sizeof(char) * 5);
		strcpy(final,"[]");
		return final;
	}
	final = malloc(sizeof(char)* 1);
	strcpy(final,"");

	final = malloc (sizeof(char) *3);
	strcat(final,"[");
	
	ListIterator itr = createIterator(gList);
	void *dataa;
	while((dataa = nextElement(&itr)) != NULL){
		count++;
	}
	ListIterator iter = createIterator(gList);
	void *data;
	
	while((data = nextElement(&iter)) != NULL){
		char *indstr = iListToJSON(gList);
		final = realloc(final,sizeof(char) * strlen(indstr) + 300);
		strcat(final,indstr);
		if (count1 < count){
			strcat(final,",");
		}
	}
	strcat(final,"]");

	return final;
}
List getAncestorListN(const GEDCOMobject* familyRecord, const Individual* person, int maxGen){
	List list = initializeList(&printGeneration,&deleteGeneration,&compareGenerations);
	if (familyRecord == NULL || person == NULL){
		return list;
	}
	Individual *in  = findPerson(familyRecord,&compareIndividualb,person);
	
	if (in == NULL){
		return list;
	}

	return list;
}
List getDescendantListN(const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen){
	List list = initializeList(&printGeneration,&deleteGeneration,&compareGenerations);

	if (familyRecord == NULL || person == NULL){
		return list;
	}
	Individual *in = findPerson(familyRecord,&compareIndividualb,person);
	if(in == NULL){
		return list;
	}
	return list;
}
void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded){
	if(obj == NULL || toBeAdded == NULL){
		return;
	}
	insertBack(&obj->individuals,(void *)toBeAdded);
}

char *GEDCOMjson (const GEDCOMobject* obj){
	char *final = NULL;
	char *charset = NULL;
	char vers[10];

	if(obj == NULL){
		return NULL;
	}

	sprintf(vers,"%0.1f",obj->header->gedcVersion);

	if (obj->header->encoding == 0){
		charset = malloc(strlen("ANSEL")+1 *sizeof(char));

		strcpy(charset,"ANSEL");
	}
	else if (obj->header->encoding == 1){
		charset = malloc(strlen("UTF-8")+1 *sizeof(char));

		strcpy(charset,"UTF-8");
	}
	else if (obj->header->encoding == 2){
		charset = malloc(strlen("UNICODE")+1 *sizeof(char));

		strcpy(charset,"UNICODE");
	}
	else if (obj->header->encoding == 3){
		charset = malloc(strlen("ASCII")+1 *sizeof(char));

		strcpy(charset,"ASCII");
	}
	ListIterator ind = createIterator(obj->individuals);
	void *data;
	int count = 0;
	while((data= nextElement(&ind)) != NULL){
		count ++;
	}
	ListIterator fam = createIterator(obj->families);
	void *elem;
	int fams=0;

	while((elem= nextElement(&fam)) != NULL){
		fams++;
	}

	char countnum[10], famsnum[10];
	sprintf(countnum,"%d",count);
	sprintf(famsnum,"%d",fams);

	final = malloc(strlen(obj->header->source)+ strlen(vers) + strlen(charset)+ strlen(obj->submitter->submitterName)+strlen(obj->submitter->address)+200);
	strcpy(final,"{");
	strcat(final,"\"""Source");
	strcat(final,"\"");
	strcat(final,":");
	strcat(final,"\"");
	strcat(final,obj->header->source);
	strcat(final,"\"");
	strcat(final,",");
	strcat(final,"\"""gedVersion");
	strcat(final,"\"");
	strcat(final,":");
	strcat(final,"\"");
	strcat(final,vers);
	strcat(final,"\"");
	strcat(final,",");
	strcat(final,"\"""encoding");
	strcat(final,"\"");
	strcat(final,":");
	strcat(final,"\"");
	strcat(final,charset);
	strcat(final,"\"");
	strcat(final,",");
	strcat(final,"\"""subname");
	strcat(final,"\"");
	strcat(final,":");
	strcat(final,"\"");
	strcat(final,obj->submitter->submitterName);
	strcat(final,"\"");
	strcat(final,",");
	strcat(final,"\"""subAddress");
	strcat(final,"\"");
	strcat(final,":");
	strcat(final,"\"");
	strcat(final,obj->submitter->address);
	strcat(final,"\"");
	strcat(final,",");
	strcat(final,"\"""Number Individual");
	strcat(final,"\"");
	strcat(final,":");
	strcat(final,"\"");
	strcat(final,countnum);
	strcat(final,"\"");
	strcat(final,",");
	strcat(final,"\"""Number Family");
	strcat(final,"\"");
	strcat(final,":");
	strcat(final,"\"");
	strcat(final,famsnum);
	strcat(final,"\"");
	strcat(final,"}");


	return final;
}
char* individualToJSON(const Individual* ind){
	char *jsonstring = NULL;
	char *value = NULL;
	bool yessex = false;

	if (ind == NULL){
		jsonstring = malloc(sizeof(char) * 10);
	 	strcpy(jsonstring,"\"{""");
		strcat(jsonstring,"\"");
		strcat(jsonstring,"\"");
		strcat(jsonstring,"}");
		strcat(jsonstring,"\"");
	}

	else{
	
		jsonstring = malloc(sizeof(char) * 2);
		strcpy(jsonstring,"");

	 	ListIterator field = createIterator(ind->otherFields);
	 	void *data;

	 	while((data=nextElement(&field)) != NULL){
	 		Field *tmpfiled = (Field *) data;
	 		if (strcmp(tmpfiled->tag,"SEX") == 0){
	 			value = malloc(sizeof(char) * strlen(tmpfiled->value));
	 			strcpy(value,tmpfiled->value);
	 			yessex = true;
	 			break;
	 		}
	 	}

	 	if (yessex == false){
	 		value = malloc(sizeof(char) * 10);
	 		strcpy(value,"");

	 	}

	 	ListIterator fam = createIterator(ind->families);
	 	void *elem;
		int count = 0;
		bool nohusnowife = false;

	 	while((elem=nextElement(&fam))!= NULL){
	 		Family *tmpfam = (Family *) elem;
	 		
	 		if (tmpfam->husband != NULL){
	 			if (!compareIndividuals(tmpfam->husband,ind)){
					count = 2;
					nohusnowife = true;

					ListIterator child = createIterator(tmpfam->children);
 					void *chil;
 		
 					while((chil = nextElement(&child)) != NULL){
 						count += 1;
 					}
	 			}
	 		}

	 		if (tmpfam->wife != NULL){
	 			if (!compareIndividuals(tmpfam->wife,ind)){
					count = 2;
					nohusnowife = true;

					ListIterator child = createIterator(tmpfam->children);
 					void *chil;
 		
 					while((chil = nextElement(&child)) != NULL){
 						count += 1;
 					}
	 			}
	 		}

	 	}

	 	if (nohusnowife == false){
	 			count = 1;
	 	}

	 	char famcount[10];
	 	
	 	sprintf(famcount,"%d",count);

	 	jsonstring = realloc(jsonstring, sizeof(char) * strlen(ind->givenName) + strlen(ind->surname)+ strlen(value) + strlen(famcount) + 300);

	 	strcat(jsonstring,"{");
	 	strcat(jsonstring,"\"givenName\"");
	 	strcat(jsonstring,":");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,ind->givenName);
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,",");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,"surname");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,":");
	 	strcat(jsonstring,"\"");	 	
	 	strcat(jsonstring,ind->surname);
	 	strcat(jsonstring,"\"");	
	 	strcat(jsonstring,",");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,"SEX");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,":");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,value);
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,",");
	 	strcat(jsonstring,"\"""Number Family");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,":");
	 	strcat(jsonstring,"\"");
	 	strcat(jsonstring,famcount);
	 	strcat(jsonstring,"\""); 	
	 	strcat(jsonstring,"}");

	 }

	return jsonstring;
}
char* indListToJSON(const GEDCOMobject* obj){
	
	char *final ;

	if (obj == NULL){

		final = malloc(sizeof(char) * 5);
		strcpy(final,"[]");
		return final;
	}

	final = malloc(sizeof(char));
	strcpy(final,"");

	final = malloc(sizeof(char) * 3);
	strcat(final,"[");
	
	int count = 0;

	ListIterator itr = createIterator(obj->individuals);
	void *dataa;

	while((dataa = nextElement(&itr)) != NULL){
		count++;
	}
	printf ("%d\n",count);

	int countt = 1;
	ListIterator iter = createIterator(obj->individuals);
	void *data;

	while((data = nextElement(&iter)) != NULL){
		Individual *tmpind  = (Individual *) data;
		
		char *indstr = individualToJSON(tmpind);
		final = realloc(final,sizeof(char) * strlen(indstr)+ 350);
		strcat(final,indstr);
		if (countt < count){
			strcat(final, ",");
		}
		countt++;
	}

	strcat (final,"]");
	return final;	
}

GEDCOMerror addingInd (GEDCOMobject* obj, char *str, char *fileName){
	GEDCOMerror err;
	Individual *toBeAdded;

	if (obj == NULL){
		err.type = OTHER_ERROR;
		err.line = -1;
		return err;
	}

	toBeAdded = JSONtoInd(str);
	
	if (toBeAdded == NULL){
		err.type = OTHER_ERROR;
		err.line = -1;
		return err;
	}

	addIndividual(obj, toBeAdded);
	err = writeGEDCOM(fileName,obj);

	if (err.type != OK){
		return err;
	}

	err.type = OK;
	err.line = -1;
	return err;
}
