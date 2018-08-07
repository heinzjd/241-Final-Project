#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha1.h"


int front = 0;
int rear = -1;
int itemCount = 0;

char *files[]= {"Directory.txt", "a.txt", "b.txt", "c.txt", "d.txt"};

//, a.txt, b.txt, c.txt, d.txt

struct TreeNode
{
    char name[500];// node data
    char hashValue[40]; //hash value of node
    struct TreeNode *leftPtr;  // pointer to left subtree
    struct TreeNode *rightPtr; // pointer to right subtree
}; // end struct TreeNode


typedef struct TreeNode TreeNode;
typedef TreeNode *TreeNodePtr;

// function prototypes
bool isEmpty();
bool isFull();
int size();
void enQueue(TreeNodePtr data, TreeNodePtr* nodeArray);
TreeNodePtr deQueue(TreeNodePtr* nodeArray);


void newSha(char* line, char* hashString);

struct TreeNode* newNode(char *name, char *hashValue);

int lines = 0;

int main()
{
    //string for sha1 hash value
    char hashString[40];

    //set up for reading from file
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;


    int j = 0;

    for(j = 0; j < 5; j++)
    {
        fp = fopen(files[j], "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);

        //read all the lines to get total number of lines
        while ((read = getline(&line, &len, fp)) != -1)
        {
            lines++;
        }
        //close the file
        fclose(fp);
    }
    //create a node array of tree node pointers the length of the file
    TreeNodePtr nodeArray[lines];

    printf("%d lines in the file\n", lines);

    for(j = 0; j < 5; j++)
    {
        //open file again
        fp = fopen(files[j], "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);

        //read each line of the file and get a hash for it
        while ((read = getline(&line, &len, fp)) != -1)
        {
            //printf("\n%s\n", line);
            newSha(line, hashString);
            //printf("%s", hashString);
            //put each line as a new node into a queue array
            enQueue(newNode(line, hashString), nodeArray);
        }

        //close the file
        fclose(fp);
    }
    //create three nodes for finding the root node
    TreeNodePtr first;
    TreeNodePtr second;
    TreeNodePtr temp;

    while(! isEmpty())
    {
        //dequeue the first and second nodes to add for their parent node
        first = deQueue(nodeArray);

        second = deQueue(nodeArray);

//    printf("\n%s\n", first->name);
//    printf("\n%s\n", first->hashValue);
//
//    printf("\n%s\n", second->name);
//    printf("\n%s\n", second->hashValue);

        char firstHash[40];
        char secondHash[40];

        //copy the nodes has values into temporary strings
        strcpy(firstHash,first->hashValue);
        strcpy(secondHash,second->hashValue);

        //caluculate new sha1 has from the concat of the two sha1 hashes
        newSha(strcat(firstHash, secondHash), hashString);

//    printf("\n%s\n", hashString);

//create a new temp node named the concat of the two hashes
        temp = newNode(firstHash,
                       hashString);


        //set first and second to be temps left and right children
        temp->leftPtr=first;
        temp->rightPtr=second;

//    printf("\n%s\n", temp->name);
//add temp to the node array
        enQueue(temp, nodeArray);
        //cycle until one node remains: the root
    }

    printf("\nThe root node is named:\n%s\n",nodeArray[0]->name);
    printf("And has sha1 value:\n%s\n",nodeArray[0]->hashValue);



    return 0;
}


//check if queue is empty
bool isEmpty()
{
    return itemCount == 0;
}

//check if queue is full
bool isFull()
{
    return itemCount == lines;
}

//check the size of the queue
int size()
{
    return itemCount;
}

//add a node to the queue
void enQueue(TreeNodePtr data, TreeNodePtr *nodeArray)
{
    if(!isFull())
    {
        if(rear == lines-1)
        {
            rear = -1;
        }

        nodeArray[++rear] = data;
        itemCount++;
    }
}

//get the next node in the queue
TreeNodePtr deQueue(TreeNodePtr *nodeArray)
{
    TreeNodePtr data = nodeArray[front++];
    if(front == lines)
    {
        front = 0;
    }
    itemCount--;
    return data;
}

//create a new tree node
TreeNodePtr newNode(char* name, char* hashValue)
{
    struct TreeNode* TreeNode = (struct TreeNode*)
                                malloc(sizeof(struct TreeNode));
    strcpy(TreeNode->name, name);
    strcpy(TreeNode->hashValue, hashValue);
    TreeNode->leftPtr = NULL;
    TreeNode->rightPtr = NULL;

    return(TreeNode);
}

//sha-1 function that calls other sha-1 stuff
void newSha(char* line, char* hashString)
{
    SHA1Context sha;
    int i;
    uint8_t Message_Digest[20];
    char buffer[2];
    int err;

    err = SHA1Reset(&sha);
    if (err)
    {
        fprintf(stderr, "SHA1Reset Error %d.\n", err );
    }

    err = SHA1Input(&sha,
                    (const unsigned char *) line,
                    strlen(line));

    err = SHA1Result(&sha, Message_Digest);
    if (err)
    {
        fprintf(stderr,
                "SHA1Result Error %d, could not compute message digest.\n",
                err );
    }
    else
    {
        for(i = 0; i < 20 ; ++i)
        {
            sprintf(buffer,"%02X", Message_Digest[i]);
            hashString[(i*2)] = buffer[0];
            hashString[(i*2)+1] = buffer[1];
        }
    }
}
