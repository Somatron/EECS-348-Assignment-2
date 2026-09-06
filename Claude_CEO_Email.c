/*
    Program name: Claude_CEO_Email.c
    Description: A max heap sorting program that automatically sorts emails for a CEO
    working at a busy company to read, the program prioritizes emails in order that
    makes it easier for the CEO to know which tasks to focus on first. 

    Inputs: User sends test file that includes Email types, counts email, goes to next email, and read email.
    Outputs: Depending on what text the user inputs, the output will either display how many emails to be read or shows user what email the person would be reading. 
    Collaborators: Som Javia
    
    Code Source: Anthropic Claude
    Author's name: Som Javia
    Creation Date: 9/8/2026 
    Revisions: Create program in C

    I AUTHORED ALL OF THE CODE HERE IN DISPLAY 
*/



/* =====================================================================
   ceo_email_priority.c

   A priority queue for a busy CEO's inbox, implemented from scratch as
   an ARRAY (list) based MAX-HEAP. No library heap/priority-queue code
   is used anywhere in this file.

   ---------------------------------------------------------------------
   PRIORITY RULES
   ---------------------------------------------------------------------
   Emails are prioritized first by sender category, then (within the
   same category) by date, newest first:

        Boss             -> highest priority (read first)
        Subordinate
        Peer
        ImportantPerson
        OtherPerson      -> lowest priority (read last)

   If two emails share the same sender category, the one with the more
   recent date wins (read first). Dates are given as MM-DD-YYYY.

   ---------------------------------------------------------------------
   INPUT FILE FORMAT
   ---------------------------------------------------------------------
   The file begins with zero or more lines of the form:

        EMAIL <sender category>,<subject line>,<date>

   followed by zero or more command lines:

        NEXT   -> show (but do not remove) the highest priority email
        READ   -> remove the highest priority email (no output of it)
        COUNT  -> print how many unread emails remain

   The program must gracefully handle:
     - NEXT or READ when the inbox is empty
     - two NEXTs in a row with no READ between them (same output twice)
     - two READs in a row with no NEXT between them (silently discards
       the top two emails)

   ---------------------------------------------------------------------
   USAGE
   ---------------------------------------------------------------------
       gcc -o ceo_email_priority ceo_email_priority.c
       ./ceo_email_priority testfile.txt

   or pipe a file in via stdin:

       ./ceo_email_priority < testfile.txt
   ===================================================================== */

#include <stdio.h> //input output
#include <stdlib.h> //general purpose 
#include <string.h> //manipulates c strings and memory
#include <ctype.h> //ctype is made to clarify characters instead of depending on ASCII

/* ---------------------------------------------------------------------
   Email record stored in the heap.
   --------------------------------------------------------------------- */
typedef struct { //allows variable to be in different formats
    char senderCategory[32]; //Whoever sent us this message, probably your boss
    char subject[256];  // The overall message, we decided to make this as big as possible because messages are pretty long ngl
    char date[16];      /* MM-DD-YYYY, stored as given for display */
    int  categoryRank;  /* higher number == higher priority */
    long dateValue;     /* YYYYMMDD as an integer for easy comparison */
} Email;

/* ---------------------------------------------------------------------
   MaxHeap: array (list) based implementation.
   --------------------------------------------------------------------- */
typedef struct { //we also are gonna need to make a struct out of our helpful heap
    Email *data; //create pointer variable to hold memory address of email
    int size;       /* number of elements currently stored */
    int capacity;   /* allocated capacity of data[] */
} MaxHeap;

/* ---------------------------------------------------------------------
   Utility: map a sender category string to its numeric priority rank.
   Larger value = higher priority = read sooner.
   --------------------------------------------------------------------- */
int categoryToRank(const char *category) { //create pointer variable to store memory for each of our peoples categories
    if (strcmp(category, "Boss") == 0)            return 5;
    if (strcmp(category, "Subordinate") == 0)     return 4;
    if (strcmp(category, "Peer") == 0)            return 3;
    if (strcmp(category, "ImportantPerson") == 0) return 2;
    if (strcmp(category, "OtherPerson") == 0)     return 1;
    return 0; /* unknown category -> lowest possible priority */
}

/* ---------------------------------------------------------------------
   Utility: convert MM-DD-YYYY into a single comparable integer
   YYYYMMDD (bigger number == later/newer date).
   --------------------------------------------------------------------- */
long dateToValue(const char *date) { //memory address for our date
    int month = 0, day = 0, year = 0;
    if (sscanf(date, "%d-%d-%d", &month, &day, &year) != 3) { //check if one of the formats in our (say date, month, year whatever) is missing
        return 0; /* malformed date defensively treated as oldest */
    }
    return (long)year * 10000L + (long)month * 100L + (long)day; //otherwise return as an integer 
}

/* ---------------------------------------------------------------------
   Returns 1 if email 'a' has strictly higher priority than email 'b',
   0 otherwise. This is the single comparison function that drives the
   entire heap ordering.
   --------------------------------------------------------------------- */
int hasHigherPriority(const Email *a, const Email *b) { //compare the senders, for this example im comparring Boss to OtherPerson, since Boss returns a higher value
    if (a->categoryRank != b->categoryRank) { //Then we return a 1
        return a->categoryRank > b->categoryRank;
    }
    /* same category: newer date wins */
    return a->dateValue > b->dateValue;
}

/* =======================================================================
   MaxHeap core operations (all implemented from scratch)
   ======================================================================= */

void heapInit(MaxHeap *heap, int initialCapacity) { //Allocate memory to the best of our abilities for C
    heap->data = (Email *)malloc(sizeof(Email) * initialCapacity); //this code actually executes allocating the memory for us, 
    if (heap->data == NULL) { //check if our memory has been actually allocated or not
        fprintf(stderr, "Fatal error: could not allocate heap memory.\n");
        exit(EXIT_FAILURE);  //dont forget stderr is a pointer for file, which means when we load our file, we need stderr to point our file structure
    }
    heap->size = 0; //start out as 0
    heap->capacity = initialCapacity; //hard limit, so our program doesnt consume all of the computer RAM 
}

void heapFree(MaxHeap *heap) { //after our program finishes, we're gonna free all of the memory associated with our Max Heap
    free(heap->data);
    heap->data = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

void heapGrow(MaxHeap *heap) { //adjust our heap size as new elements are added into our heap
    int newCapacity = heap->capacity * 2; //double our compacity by 2
    Email *newData = (Email *)realloc(heap->data, sizeof(Email) * newCapacity); //create a new pointer that should resize our new capacity
    if (newData == NULL) { //if our new data is pointing to null, then thats a check engine light that we cant adjust more space for our memory
        fprintf(stderr, "Fatal error: could not grow heap memory.\n");
        exit(EXIT_FAILURE);
    }
    heap->data = newData; //otherwise update our data to our new data
    heap->capacity = newCapacity; //same with capacity, update capacity 
}

void heapSwap(Email *a, Email *b) { //usually the function of max heaps is swapping elements in a list
    Email temp = *a;
    *a = *b;
    *b = temp; //this swaps elements, usually whenever we add or remove programs
}

/* Move the element at index up until the heap property is restored. */
void siftUp(MaxHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (hasHigherPriority(&heap->data[index], &heap->data[parent])) { //compare the email sender, depending on the person we call this function to determine if we need to swap elements or not
            heapSwap(&heap->data[index], &heap->data[parent]); //if thats the case we call function
            index = parent; //oh btw this is a recursive loop, we're going to keep doing this until we meet the standard order our program tells us to swap
        } else {
            break;
        }
    }
}

/* Move the element at index down until the heap property is restored. */
void siftDown(MaxHeap *heap, int index) {
    while (1) { //this is mostly for removing elements at a heap
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < heap->size && hasHigherPriority(&heap->data[left], &heap->data[largest])) { //check left, since this is a max heap, we switch elements in order
            largest = left; //from largest to smallest
        }
        if (right < heap->size && hasHigherPriority(&heap->data[right], &heap->data[largest])) {
            largest = right; //same logic goes for checking right
        }
        if (largest == index) {
            break; //when we swap, once our index (where we're at in the array) is matching the value of the largest value we exit out of the loop
        }
        heapSwap(&heap->data[index], &heap->data[largest]);
        index = largest; //downheap is a recurrsive call too
    }
}

/* Insert a new email into the heap. */
void heapPush(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity) {
        heapGrow(heap); //update size if we go over our memory limits
    }
    heap->data[heap->size] = email; //email becomes added into heap
    siftUp(heap, heap->size); //update heap ordering
    heap->size++; //add 1 to our size
}

/* Look at (but do not remove) the highest priority email.
   Returns 1 on success and fills *out, or 0 if the heap is empty. */
int heapPeek(MaxHeap *heap, Email *out) {
    if (heap->size == 0) {
        return 0;
    }
    *out = heap->data[0];
    return 1;
}

/* Remove the highest priority email from the heap.
   Returns 1 on success and fills *out, or 0 if the heap was empty. */
int heapPop(MaxHeap *heap, Email *out) {
    if (heap->size == 0) {
        return 0; //dont remove anything if heap empty
    }
    *out = heap->data[0]; //where in address will we delete memory
    heap->size--; //lower our size
    heap->data[0] = heap->data[heap->size]; //remove element
    if (heap->size > 0) {
        siftDown(heap, 0); //reorganize heap ordering
    }
    return 1;
}

/* =======================================================================
   Line / command parsing helpers
   ======================================================================= */

/* Strip trailing \n, \r and surrounding whitespace from a string in place. */
void trimNewlineAndSpace(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || //strip our unnessessary elements 
                        isspace((unsigned char)str[len - 1]))) { //check for spaces or unnessessary formatting and cleans it up, returns what char there is
        str[--len] = '\0';  // done by removing last character of string
    }
}

/* Parses a line of the form:
       EMAIL <sender category>,<subject line>,<date>
   into an Email struct. Returns 1 on success, 0 on failure. */
int parseEmailLine(const char *line, Email *email) {
    const char *rest = line + strlen("EMAIL");
    while (*rest == ' ') rest++; /* skip the space(s) after EMAIL */

    char buffer[512]; //fix size on the stack
    strncpy(buffer, rest, sizeof(buffer) - 1); //copy strings, reorganize size
    buffer[sizeof(buffer) - 1] = '\0'; //makesure it leaves room to point to null
    trimNewlineAndSpace(buffer); //oh also remove any whitespace with the function

    /* Split on commas: category , subject , date */
    char *firstComma = strchr(buffer, ','); //find comma the string and split it
    if (firstComma == NULL) return 0; //if cant find comma then return 0
    char *lastComma = strrchr(buffer, ','); //in order to check the date and organize emails based on the latest email, we check the last comma and label the last element after the last comma as a data
    if (lastComma == NULL || lastComma == firstComma) return 0; //if there was only 1 comma or no comma return 0

    size_t catLen = (size_t)(firstComma - buffer); //calculate distance (number of characters) between 2 memory addresses
    size_t subjLen = (size_t)(lastComma - firstComma - 1); //check length of substring 

    if (catLen >= sizeof(email->senderCategory)) catLen = sizeof(email->senderCategory) - 1; //deciding on the length of the message sender, resize memory  for sender
    if (subjLen >= sizeof(email->subject)) subjLen = sizeof(email->subject) - 1; //resize memory for the message length to store a null pointer 


    /*Copies string buffer to make sure it's valid string for both sender and the message context, we use a null pointer to know when the string ends */
    strncpy(email->senderCategory, buffer, catLen); 
    email->senderCategory[catLen] = '\0';

    strncpy(email->subject, firstComma + 1, subjLen);
    email->subject[subjLen] = '\0';

    strncpy(email->date, lastComma + 1, sizeof(email->date) - 1); //we do the same thing for date except it's the last comma we check to find the date
    email->date[sizeof(email->date) - 1] = '\0';
    trimNewlineAndSpace(email->date); //clean up string

    email->categoryRank = categoryToRank(email->senderCategory); //begin ranking emails based on who sent the emails
    email->dateValue = dateToValue(email->date); //format into a proper string YYYYMMDD

    return 1; //ran code successfully
}

/* =======================================================================
   Command handlers
   ======================================================================= */

void handleNext(MaxHeap *heap) { //this is the fun part, we check for 
    //EMAIL, 
    //NEXT
    //READ
    //COUNT, and run our code based code commands


    Email top; //create variable out of variable struct
    if (!heapPeek(heap, &top)) { //check the top of the heap, if heap empty
        printf("There are no emails to read.\n\n"); //no emails to read
        return;
    }
    printf("Next email:\n"); //otherwise we read the top 
    printf("\tSender: %s\n", top.senderCategory);
    printf("\tSubject: %s\n", top.subject);
    printf("\tDate: %s\n\n", top.date);
}

void handleRead(MaxHeap *heap) { //reading email, we remove email off the heap
    Email removed; //remove memory address
    if (!heapPop(heap, &removed)) { //if there are no emails in the heap
        printf("There are no emails to read.\n\n"); //we dont have any emails to read
    }
    /* On success we intentionally print nothing: the CEO has read the
       email and dealt with it, we simply remove it from the queue. */
}

void handleCount(MaxHeap *heap) { //look at our size of heap and return email numbers
    printf("There are %d emails to read.\n\n", heap->size); //return how many emails we have
}

/* =======================================================================
   Main driver
   ======================================================================= */

int main(int argc, char *argv[]) { //the 2nd parameter loads the filename for our program to read
    FILE *fp = stdin; //read file from what we input

    if (argc > 1) { //make sure user actually passes an arguement into program
        fp = fopen(argv[1], "r"); //read dat file
        if (fp == NULL) { 
            fprintf(stderr, "Error: could not open file '%s'\n", argv[1]); //if there isnt a file we throw error
            return EXIT_FAILURE;
        }
    }

    MaxHeap heap; //variable heap with heap types
    heapInit(&heap, 16); //the most we're gonna allow our heap to carry is 16 elements 

    char line[1024]; //how big our line can be to read from
    while (fgets(line, sizeof(line), fp) != NULL) { 
        /* Work on a trimmed copy so we can compare command keywords safely */
        char trimmed[1024]; //make array with 1024 characters same as line
        strncpy(trimmed, line, sizeof(trimmed) - 1); //copy the string
        trimmed[sizeof(trimmed) - 1] = '\0'; //make space for null pointer
        trimNewlineAndSpace(trimmed); //and pass it to clean the string

        if (trimmed[0] == '\0') {
            continue; /* skip blank lines */
        }

        if (strncmp(trimmed, "EMAIL", 5) == 0) { //read EMAIL command
            Email email;
            if (parseEmailLine(trimmed, &email)) { //format the email like a database
                heapPush(&heap, email); //and add the email to heap
            } else { 
                fprintf(stderr, "Warning: could not parse EMAIL line: %s\n", trimmed); //if something goes wrong (like we go over our heap limit) print error
            }
        } else if (strcmp(trimmed, "NEXT") == 0) {
            handleNext(&heap); //show email categories
        } else if (strcmp(trimmed, "READ") == 0) {
            handleRead(&heap); //read email and remove in the heap
        } else if (strcmp(trimmed, "COUNT") == 0) {
            handleCount(&heap); //count heap size by counting how many elements are in heap
        } else {
            fprintf(stderr, "Warning: unrecognized command: %s\n", trimmed); //if we couldnt read anything throw error
        }
    }

    if (fp != stdin) {
        fclose(fp); //prevents undefined behavior, check if anything goes left
    }
    heapFree(&heap); //clear heap

    return EXIT_SUCCESS; //exit program successfully!!!!!!!!!!!
}
