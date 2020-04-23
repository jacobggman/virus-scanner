
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dirent.h"

#define MAX 2000

char ** getFiles(char* v);
char * getInput(char *folder, char* virus);
void scan(char * folder, char * virus, int regular);
int rScan(char *fileName, char *virus);
int fScan(char *fileName, char *virus);
char * concat(const char *s1, const char *s2);
void startLog(char * folder, char * virus, int normal);
void addResult(char * folder, char * text, char * filename);
char * bunos(char * fileName, int first, int last);

int number_of_files = 0;
int percent_file = 0;
long virusFileLen = 0;
int choice = 0;
int main(void)
{

	char folder[MAX] = { 0 };
	char virus[MAX] = { 0 };
	char *VirusSign  = getInput(folder, virus);
	
	


	printf("\n\n%d\n", virusFileLen);
	scan(folder, VirusSign, choice);

	free(VirusSign);
	getchar();
	getchar();
	getchar();
	getchar();
	return 0;
}
/*
This function get the files names from folder name
Input: string folder name
Output: array of pointers of files names
*/
char ** getFiles(char * v)
{
	int len = 0;
	int NumberOfElements = 0;

	struct dirent *pDirent;
	DIR *pDir;
	//remove /n
	strtok(v, "\n");

	pDir = opendir(v);
	
	char **orderedIds = (char**) malloc(1);

	int i = 0;
	//loop for every file name
	while ((pDirent = readdir(pDir)) != NULL)
	{
		number_of_files++;
		orderedIds = realloc(orderedIds, (i + 1) * sizeof(char*));
		//save it
		orderedIds[i] = (char*)malloc(strlen(pDirent->d_name) * sizeof(char) + 1);
		strcpy(orderedIds[i], pDirent->d_name);
		i++;
	}
	closedir(pDir);
	return orderedIds;
}
/*
This function get the Input of the virus file and the folder of the files 
Input: pointers of the virus file and the folder of the files
Output: Regular (1) scan or fast (0) 
*/
char * getInput(char *folder, char* virus)
{

	printf("Welcome to my virus scan!\n\n");
	printf("Enter a folder to scan: ");
	fgets(folder, MAX, stdin);

	printf("Virus signature file: ");
	fgets(virus, MAX, stdin);
	//remove /n
	strtok(virus, "\n");
	strtok(folder, "\n");
	//I did not succeed make it bytes....
	FILE *fileptr;

	fileptr = fopen(virus, "rb");  // Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	virusFileLen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file

	char *returnPoint = (char *)malloc((virusFileLen + 1) * sizeof(char)); // Enough memory for file + \0
	fread(returnPoint, virusFileLen, 1, fileptr); // Read in the entire file
	fclose(fileptr); // Close the file

	printf("1 for normal else for qwick: ");
	scanf("%d", &choice);

	return returnPoint;
}
/*
This function scan the files and send them to regular of fast scan
Input: pointers of the virus file and the folder of the files and if it regular scan (0 or 1)
Output: None
*/
void scan(char * folder, char * virus, int regular)
{

	char **files = getFiles(folder);
	startLog(folder, virus, regular);
	char * s = 0;
	//loop for all the files
	for (int i = 2; i < number_of_files; i++)
	{
		//get File location by concat (location and name) 
		char * filePlace = concat(s = concat(folder, "/"), files[i]);
		if (regular)
		{
			if (rScan(filePlace, virus))
			{
				printf("%s - Infected!\n", files[i]);
				addResult(folder, " - Infected!\n", files[i]);
			}
			else
			{
				printf("%s - Clean\n", files[i]);
				addResult(folder, " - Clean\n", files[i]);
			}
		}
		else//if fast
		{
			if (fScan(filePlace, virus))
			{
				printf("%s - Infected!\n", files[i]);
				//check where he find this
				if (percent_file == 20)
				{
					addResult(folder, " - Infected! (first 20%)\n", files[i]);
				}
				else if (percent_file == -20)
				{
					addResult(folder, " - Infected! (last 20%)\n", files[i]);
				}
				else
				{
					addResult(folder, " - Infected! (Between 20% to 80%) \n", files[i]);
				}
				percent_file = 0;
			}
			else
			{
				printf("%s - Clean\n", files[i]);
				addResult(folder, " - Clean\n", files[i]);
			}
		}
		free(s);
		free(filePlace);
	}
	printf("\nscan complete!\n");
	printf("\nsee log at the files folder! (Log.txt)\n");
	free(files);
}
/*
This function is the regular scan
Input: File location and virus signature (string)
Output: if file is virus
*/
int rScan(char *fileName, char *virus)
{
	
	FILE * file;
	file = fopen(fileName, "rb");

	int i = 0;
	int c = 0;
	int flag = 0;
	if (file)
	{
		while ((c = getc(file)) != EOF && !flag)
		{
			//if find len of virus signature in a row ?
			if (i >= virusFileLen - 2)
			{
				//it virus
				flag = 1;
			}
			//if it not in a row
			if (c != virus[i])
			{
				//reset counting
				i = 0;
			}
			else
			{
				//add to the counting
				i++;
			}
		}
	}
	fclose(file);
	return flag;
}
/*
This function is combine two string
Input: two strings
Output: the two string combine
*/
char* concat(const char *s1, const char *s2)
{
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
/*
This function is start a log file
Input: folder of the log and virus signature and scan mode
Output: None
*/
void startLog(char * folder, char * virus, int normal)
{
	char *s = concat(folder, "/Log.txt");
	FILE *f = fopen(s,"w");
	free(s);

	fprintf(f, "Anti-virus began! Welcome!\n\n");
	fprintf(f, "Folder to scan:\n%s\n", folder);
	fprintf(f, "Virus signature:\n%s\n\nScanning option:\n", virus);
	//scan mode
	if (normal)
	{
		fprintf(f, "Normal Scan");
	}
	else
	{
		fprintf(f, "Fast Scan");
	}

	fprintf(f, "\n\nResults:\n");
	fclose(f);
}
/*
This function add Result to log
Input: folder of the log and text to enter and the filename (who have/dont virus)
Output: None
*/
void addResult(char * folder, char * text, char * filename)
{
	char *s = concat(folder, "/Log.txt");
	FILE *f = fopen(s, "a");
	free(s);
	//add to text
	fprintf(f, "%s%s",filename ,text);
	fclose(f);
}
/*
This function fast scan
Input: fileName to scan and virus signature
Output: if file is virus
*/
int fScan(char *fileName, char *virus)
{
	FILE * file;
	file = fopen(fileName, "rb");
	//get size
	fseek(file, 0L, SEEK_END);
	int fileSize = ftell(file);
	rewind(file);

	int twentyPercent = (int)fileSize * 20 / 100;
	
	int i = 0;
	int c = 0;
	int flag = 0;
	int byte = 0;

	//first 20%
	while ((c = getc(file)) != EOF && byte < twentyPercent && !flag)
	{
		//scan
		byte++;
		if (i >= virusFileLen - 2)
		{
			percent_file = 20;
			flag = 1;
		}
		if (c != virus[i])
		{
			i = 0;
		}
		else
		{
			i++;
		}
	}

	//last 20%
	if (!flag)
	{
		//scan
		rewind(file);
		fseek(file, -twentyPercent, SEEK_END);
		while ((c = getc(file)) != EOF && !flag)
		{
			byte++;
			if (i >= virusFileLen - 2)
			{
				percent_file = -20;
				flag = 1;
			}
			if (c != virus[i])
			{
				i = 0;
			}
			else
			{
				i++;
			}
		}
	}
	
	//rest
	if (!flag)
	{
		//scan
		rewind(file);
		byte = twentyPercent;
		fseek(file, twentyPercent, SEEK_SET);
		while ((c = getc(file)) != EOF && !flag && (byte <= fileSize - twentyPercent))
		{
			byte++;
			if (i >= virusFileLen - 2)
			{
				flag = 1;
			}
			if (c != virus[i])
			{
				i = 0;
			}
			else
			{
				i++;
			}
		}
	}
	fclose(file);
	return flag;
	
}
/*
This function is geting data betwing two numbers in file
Input: fileName to scan and start and end
Output: the text
*/
char * bunos(char * fileName, int first, int last)
{
	FILE * file;
	file = fopen(fileName, "rb");

	char c = 0;
	int byte = 0;
	//dinamyc array
	char * tempArray = (char*)malloc(sizeof(char));
	//go to the start number
	fseek(file, first, SEEK_SET);
	//to end
	while ((c = getc(file)) != EOF && byte < last)
	{
		tempArray = realloc(tempArray, (1 + byte) * sizeof(char*) + 1);
		tempArray[byte] = c;
		byte++;
	}
	return tempArray;
}