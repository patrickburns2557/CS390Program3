/*
Patrick Burns
November 11, 2022
Class: CS 390
Assignment: Program 3
Program: Filter Program
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <malloc.h>
#define ONE_MB 1000000

/* Function Prototypes */
void removeNL(char* input);
int ReadBinaryFile(const char* fileName);
int isCharText(char c);
int IsTextFile();
void ConvertToUnix(const char* fileName);


/* Global Variables */
unsigned char* buffer;
int buffersize = 0;
int datasize = 0;


int main(int argc, char* argv[])
{
    int status = 0;
    char fileName[1024] = "temp";

    buffersize = 5 * ONE_MB;
    buffer = (unsigned char*) malloc(buffersize);

    while(fgets(fileName, sizeof(fileName), stdin))
    {
        removeNL(fileName);
        status = ReadBinaryFile(fileName);

        /* If there was an error reading the file */
        if(status > 0)
        {
            if(status == 1)
            {
                fprintf(stderr, "ERR: '%s': Could not open file.\n", fileName);
            }
            else if(status == 2)
            {
                fprintf(stderr, "ERR: '%s': File is zero length.\n", fileName);
            }
            else if(status == 3)
            {
                fprintf(stderr, "ERR: '%s': File is too large for buffer.\n", fileName);
            }
            else if(status == 4)
            {
                fprintf(stderr, "ERR: '%s': Error reading file.\n", fileName);
            }
        }
        else /* no error in reading file */
        {
            if(!IsTextFile())
                fprintf(stderr, "ERR: '%s': File is NOT a text file.\n", fileName);
            else
                if (strstr((char*)buffer, "\r"))
                {
                    fprintf(stdout, "     '%s': File is a Windows text file.\n", fileName);
                    

                    if(argc > 1 && strcmp(argv[1], "-u\n"))
                    {
                        ConvertToUnix(fileName);
                        fprintf(stdout, "       File converted.\n");
                    }
                }
                else
                {
                    fprintf(stdout, "     '%s': File is a Unix text file.\n", fileName);
                }
        }
    }

    

    if(buffer)
        free(buffer);

    return 0;
}


/* removes newline and carriage return characters at the end of the passed in string, if there are any */
void removeNL(char* input)
{
    for(; *input; ++input)
    {
        if(*input == '\n')
        {
            *input = '\0';
        }
        if(*input == '\r')
        {
            *input = '\0';
        }
    }
    
}

/* Reads in a file that is treated as binary and stores it into the 'buffer' variable */
/* Stores the size of the file in bytes into the 'datasize' variable */
int ReadBinaryFile(const char* fileName)
{
    int status = 0;
    int bytesInFile = 0;
    FILE* inFile = 0;

    inFile = fopen(fileName, "rb");
    if(!inFile)
    {
        status = 1;
    }
    else
    {
        /* check if file fits in buffer */
        fseek(inFile, 0, SEEK_END);
        bytesInFile = (int) ftell(inFile);
        fseek(inFile, 0, SEEK_SET);
        
        /* File was empty */
        if(bytesInFile == 0)
        {
            status = 2;
        }
        else
        {
            /* File was too large for buffer size */
            if(bytesInFile >= (buffersize -1 ))
            {
                status = 3;
            }
            else
            {
                datasize = (int) fread(buffer, 1, bytesInFile, inFile);
                /* if the datasize read in doesn't match the byte number in the file, read error occured */
                if(datasize != bytesInFile)
                {
                    status = 4;
                }
                else
                {
                    /* convert read in data to a 'C' string */
                    buffer[datasize] = '\0';
                }
            }
        }
    }
    if(inFile)
        fclose(inFile);
    return status;
}

/* Tests if the given character is a text character or not */
int isCharText(char c)
{
    int result = 0;
    if(isprint(c))
    {
        result = 1;
    }
    else
    {
        switch(c)
        {
            case '\f':
            case '\t':
            case '\r':
            case '\n':
            case '\a':
            case '\b':
            case '\v': result = 1;
                       break;
        }
    }
    return result;
}

/* iterates through the current file stored in the buffer to see if it's a text file or not */
int IsTextFile()
{
    int i, count;
    bool isText = true;
    count = datasize < 100 ? datasize : 100;
    
    for(i = 0; i < count; i++)
    {
        if(!isCharText(buffer[i]))
        {
            isText = true;
            break;
        }
    }

    return isText;
}

/* stores the file currently in the buffer out to the passed in filename
   if any carriage returns are encountered, it skips those, thus converting
   the file to a Unix text file */
void ConvertToUnix(const char* fileName)
{
    int i;
    FILE* outFile = 0;

    outFile = fopen(fileName, "wb");
    
    /* write out the file character by character */
    for(i = 0; i < datasize; i++)
    {
        /* if the character is a carriage return, skip writing it */
        if(buffer[i] == '\r')
        {
            continue;
        }
        fputc( buffer[i], outFile);
    }

    if(outFile)
        fclose(outFile);
}