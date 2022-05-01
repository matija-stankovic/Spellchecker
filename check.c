#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 256

// Hash Function djb2 by Dan Bernsein. Source: http://www.cse.yorku.ca/~oz/hash.html
unsigned int hash(char *str){
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % 5000;
}

// Defining the node structure
typedef struct node{
    char *value;
    struct node *next;
} Node;

// Initialise the new node
Node *initializeNode(char *value,Node *next){
    Node *newNode = malloc(sizeof(Node));
    newNode->value = malloc((strlen(value)) + 1);
    strcpy(newNode->value, value);
    newNode->next = next;
    return newNode;
}

// Hash table structure
typedef struct openHashTable{
    int size;
    Node **table;
} OpenHashTable;

// Initialise new Hash Table
OpenHashTable *initializeHashTable(int size){
    OpenHashTable *hashTable = malloc(sizeof(OpenHashTable));
    hashTable->size = size;
    hashTable->table = malloc(sizeof(Node *) * size);
    for(int i = 0; i < size; i++) {
        hashTable->table[i] = NULL;
    }
    return hashTable;
}

// Insert new element into hash table
void insert(OpenHashTable *hashTable, char *value){
    unsigned int position = hash(value);
    Node *ptr = hashTable->table[position];
    while(ptr!=NULL){
        if(strcmp(ptr->value, value) == 0)
            return;
        else
            ptr=ptr->next;
    }
    hashTable->table[position] = initializeNode(value,hashTable->table[position]);
}

// Print the table (USED FOR DEBUGGING)
void printHashTable(OpenHashTable *hashTable){
    for(int i = 0; i < hashTable->size; i++){
        printf("Row %d: [", i);
        Node *ptr = hashTable->table[i];
        while(ptr!=NULL){
            printf(" %s",ptr->value);
            ptr = ptr->next;
        }
        printf(" ]\n");
    }
}

// Check if the file can be opened
void checkOpen(FILE *fp){
    if(fp == NULL){
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }
}

// Combining two strings, to avoid messing with char pointers
char *combine(char *str1, char *str2){
    char *str3 = malloc(strlen(str1)+ strlen(str2) + 1);
    int i = 0, j = 0;
    while (str1[i] != '\0') {
        str3[j] = str1[i];
        i++;
        j++;
    }
    i = 0;
    while (str2[i] != '\0') {
        str3[j] = str2[i];
        i++;
        j++;
    }
    str3[j] = '\0';
    return str3;
}

// Check if a node is a member of the hash table
int isMember(OpenHashTable *hashTable, char *value){
    unsigned int position = hash(value);
    Node *ptr = hashTable->table[position];

    while(ptr != NULL){
        if(strcmp(ptr->value, value) == 0) {
            return 1;
        }
        else{
            ptr=ptr->next;
        }
    }
    return 0;
}

// Remove the first character
char *removeFirst(char *str){
    char *p;
    p = str;
    p++;
    return p;
}

// Switch 2 characters
char *switchChar(char *str, int x){
    char *newStr = malloc(strlen(str) + 1);
    int i = 0, j = 0;

    while(i != x){
        newStr[j] = str[i];
        i++;
        j++;
    }
    newStr[j] = str[i+1];
    j++;
    newStr[j] = str[i];
    i = i + 2;
    j++;

    while(str[i] != '\0'){
        newStr[j] = str[i];
        i++;
        j++;
    }
    newStr[strlen(newStr)] = '\0';
    return newStr;
}

// Create a copy of the word
char *saveWord(char *word){
    char *newStr = malloc(strlen(word) + 1);
    int i = 0;
    while(word[i] != '\0'){
        newStr[i] = word[i];
        i++;
    }
    return newStr;
}

int main(int argc, char **argv)
{
	char *dictionaryFilePath = argv[1]; //this keeps the path to the dictionary file file
	char *inputFilePath = argv[2]; //this keeps the path to the input text file
	char *check = argv[3]; // this keeps the flag to whether we should insert mistyped words into dictionary or ignore
	int numOfWords=0; //this variable will tell us how much memory to allocate
    char lineInfo[BUFFSIZE]; //Keep the line info

	int insertToDictionary;

	if(strcmp(check,"add")==0)
		insertToDictionary = 1;
	else 
		insertToDictionary = 0;

    FILE *fp = fopen(dictionaryFilePath, "r");
    //FILE *fp = fopen("../words.txt", "r");
    checkOpen(fp);

    // Get the number of words from the dictionary
    while (fgets(lineInfo, 255, fp) != NULL){
        numOfWords++;
    }

    fseek(fp, 0, SEEK_SET);

    // Initialising the hash table with 5000 slots
    OpenHashTable *myHash = initializeHashTable(5000);
    char wrd[BUFFSIZE];
    // Insert dictionary words into hash table
    for (int i = 0; i < numOfWords; i++){
        fscanf(fp, "%s \n", wrd);
        insert(myHash,wrd);
    }
    fclose(fp);


    fp = fopen(inputFilePath, "r");
    //fp = fopen("../test.txt", "r");
    checkOpen(fp);

	int noTypo = 1; // This will turn to 0 if there is a typo
    int trigger = 0;

    char *word;
    char delimiter[]= " ,.:;!\n";
    // Adding the english alphabet for comparison
    char *letters[26]={"a", "b", "c", "d", "e","f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

    // temporary variables - they are necessary because pointer manipulation gets too messy
    char *temp;
    char *temp2;
    char *temp3;

    // Main Loop Checking for Errors
    while (fgets(lineInfo, 255, fp) != NULL){
        //Take the input word by word from the input file
        word = strtok(lineInfo,delimiter);
        while(word!=NULL){
            noTypo=1; //reset to 1 with every word
            unsigned int hashValue = hash(word); // find the hash value of the query word
            Node *ptr = myHash->table[hashValue]; // Set the first pointer to the hash table at the query hash value

            // Check if there is a typo in any of the words
            while(ptr!=NULL) {
                if(strcmp(ptr->value, word) == 0) {
                    noTypo = 0;
                    break;
                }
                else
                    ptr=ptr->next;
            }
            int cc = 0;
            if(noTypo != 0){
                printf("Misspelled word: %s\n",word);
                printf("Suggestions: ");
                trigger++;

                //Testing Condition
                for(int i = 0; i < 26; i++){
                    temp = combine(letters[i], word);
                    if(isMember(myHash, temp) == 1){
                        cc++;
                    }
                }

                // WITH SWITCHING LETTERS
                for(int i = 0; i < strlen(word) - 1; i++){
                    temp2 = switchChar(word, i);
                    if(isMember(myHash, temp2) == 1){
                        printf("%s ", temp2);
                        if(insertToDictionary == 1){
                            insert(myHash, word);
                        }
                    }
                }

                for(int i = 0; i < 26; i++){
                    // CHECK IF THERE IS A WORD WITH A NEW PREFIX LETTER
                    temp = combine(letters[i], word);
                    if(isMember(myHash, temp) == 1){
                        printf("%s ", temp);
                        if(insertToDictionary == 1){
                            insert(myHash, word);
                        }
                    }
                }

                // CHECK IF THERE IS A WORD WITH A NEW SUFFIX LETTER
                for(int i = 0; i < 26; i++){
                    temp = combine(word, letters[i]);
                    if(isMember(myHash, temp) == 1){
                        printf("%s ", temp);
                        if(insertToDictionary == 1){
                            insert(myHash, word);
                        }
                    }
                }

                // CHECK IF THERE IS A WORD WITHOUT THE PREFIX LETTER
                temp = removeFirst(word);
                if(isMember(myHash, temp) == 1){
                    printf("%s ", temp);
                    if(insertToDictionary == 1){
                        insert(myHash, word);
                    }
                }

                // CHECK IF THERE IS A WORD WITHOUT THE SUFFIX LETTER
                temp3 = saveWord(word);
                word[strlen(word)-1] = '\0';
                if(isMember(myHash, word) == 1){
                    printf("%s", word);
                    if(insertToDictionary == 1){
                        insert(myHash, temp3);
                    }
                }
                printf("\n");
            }
            word = strtok(NULL,delimiter);
        }
	}

	fclose(fp);
    // Clearing the memory
    for(int i = 0; i < 5000; i++){
        free(myHash->table[i]);
    }
    free(myHash->table);
    free(myHash);

    // If there was no typo count increase, then print no typo
    if(trigger == 0) {
        printf("No typo!\n");
    }
	return 0;
}
