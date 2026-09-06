/*
    Program name: Gemini_CEO_Email.c
    Description: A max heap sorting program that automatically sorts emails for a CEO
    working at a busy company to read, the program prioritizes emails in order that
    makes it easier for the CEO to know which tasks to focus on first. 

    Inputs: User sends test file that includes Email types, counts email, goes to next email, and read email.
    Outputs: Depending on what text the user inputs, the output will either display how many emails to be read or shows user what email the person would be reading. 
    Collaborators: Som Javia
    
    Code Source: Google Gemini
    Author's name: Som Javia
    Creation Date: 9/8/2026 
    Revisions: Create program in C

    I AUTHORED ALL OF THE CODE HERE IN DISPLAY 
*/

#include <stdio.h> //input output
#include <stdlib.h> //library for general purpose c commands 
#include <string.h> //string manipulation

#define MAX_STR 256 //longest we will extract string from text is 256 characters

// Structure to hold individual email data
typedef struct {
    char category[50]; //sender
    char subject[MAX_STR]; //how long the message is
    char date_str[20]; //how long the date is
    int priority_score; // Numeric score for category
    long date_score;     // Numeric score for date (YYYYMMDD)
} Email;

// Max-Heap structure for the priority queue
typedef struct {
    Email *data; //pointer to memory address
    int size; //how big our heap is
    int capacity; //how much our heap can hold
} MaxHeap;

// Helper to convert category string to an integer score
int get_category_score(const char *cat) {
    if (strcmp(cat, "Boss") == 0) return 5; //rank senders based on importance
    if (strcmp(cat, "Subordinate") == 0) return 4; //for example higher the number
    if (strcmp(cat, "Peer") == 0) return 3; //the more important the person is
    if (strcmp(cat, "ImportantPerson") == 0) return 2;
    if (strcmp(cat, "OtherPerson") == 0) return 1;
    return 0;
}
 
// Helper to convert MM-DD-YYYY string to YYYYMMDD integer for chronological comparison
long get_date_score(const char *date_str) { //store bigger ints than what the standard is
    int month, day, year; //create variables for day month and year
    if (sscanf(date_str, "%d-%d-%d", &month, &day, &year) == 3) { //if our string follows the MMDDYYYY format then
        return (long)year * 10000 + month * 100 + day; //condence it all together into a single string
    }
    return 0; //otherwise return nothing
}

// Compares two emails. Returns > 0 if a has higher priority than b
int compare_emails(Email a, Email b) { 
    if (a.priority_score != b.priority_score) { //as long as both senders from emails arent the same
        return a.priority_score - b.priority_score; //calculate the score between sender A vs sender B
    }
    // If categories match, newer date (larger YYYYMMDD) has higher priority
    if (a.date_score > b.date_score) return 1;
    if (a.date_score < b.date_score) return -1;
    return 0; //if the dates are the same then return nothing
}

// Initialize Max-Heap
MaxHeap* create_heap() {
    MaxHeap *heap = (MaxHeap *)malloc(sizeof(MaxHeap)); //memory allocation of how big the heap is
    heap->capacity = 10; //allow a max of 10 elements in the heap
    heap->size = 0; //start out with a size of 0
    heap->data = (Email *)malloc(heap->capacity * sizeof(Email)); //adjust array in memory to store specific numbers of email
    return heap; //return our heap
}

// Heapify up when inserting an element
void heapify_up(MaxHeap *heap, int index) { //when adding to heap
    int parent = (index - 1) / 2; //compare index with parent
    while (index > 0 && compare_emails(heap->data[index], heap->data[parent]) > 0) { //becomes a while loop instead of claudes recursion 
        Email temp = heap->data[index]; //swap index
        heap->data[index] = heap->data[parent]; //with the parent
        heap->data[parent] = temp;
        
        index = parent; //index becomes the parent
        parent = (index - 1) / 2; //parent gets resetted to calculate left/right nodes
    }
}

// Heapify down when removing the root element
void heapify_down(MaxHeap *heap, int index) {
    int largest = index; //max heap so we make largest
    int left = 2 * index + 1; //check left
    int right = 2 * index + 2; //check right

    if (left < heap->size && compare_emails(heap->data[left], heap->data[largest]) > 0) { //call our compare email function and see what our email returns to
        largest = left; //if left is bigger (more priority) then replace largest with left
    }
    if (right < heap->size && compare_emails(heap->data[right], heap->data[largest]) > 0) {
        largest = right; //same logic with right
    }

    if (largest != index) { //if our index isnt the largest number (or at the root)
        Email temp = heap->data[index]; //swap index with largest
        heap->data[index] = heap->data[largest];
        heap->data[largest] = temp;
        heapify_down(heap, largest); //recursive call
    }
}

// Insert an email into the priority queue
void insert_email(MaxHeap *heap, Email email) {
    if (heap->size >= heap->capacity) { //if our size is becoming bigger than how much the heap can hold
        heap->capacity *= 2; //double by 2
        heap->data = (Email *)realloc(heap->data, heap->capacity * sizeof(Email)); //adjust size in memory for our email (counting size for sender, message length, and date)
    }
    heap->data[heap->size] = email; //update data to point to address
    heapify_up(heap, heap->size); //reorder our heap
    heap->size++; //add 1 to our heap size
}

// Remove and return the highest priority email
void pop_email(MaxHeap *heap) { //remove email
    if (heap->size == 0) return; //if there is nothing in heap
    heap->data[0] = heap->data[heap->size - 1]; //otherwise we adjust memory size
    heap->size--; //subtract heap count by -1
    heapify_down(heap, 0); //reorder size
}

// Clean up heap memory
void free_heap(MaxHeap *heap) { //after program ran
    free(heap->data); //free memory buffer of our heap
    free(heap); //free heap overall
}

int main() {
    MaxHeap *heap = create_heap(); //make maxheap first
    char line[MAX_STR * 3]; //however long our file is, best to multiply how long our max message would be by 3

    char filename[100];
    FILE *file;

    printf("Hi please input text file name below: ");

    //read string including spaces and handle new line
    if (fgets(filename, sizeof(filename), stdin) != NULL) {
        filename[strcspn(filename, "\n")] = '\0'; //save for null pointer
    }

    //open file in read mode
    file = fopen(filename, "r");

    if (file == NULL) {
        printf("ERROR: Could not open file provided :[ \n");
        return 1; //exit program with error
    }


    printf("\nFile opened successfullly!!!\n \n");
    
    while (fgets(line, sizeof(line), file)) { //read single line text from standard input
        // Strip newline characters
        line[strcspn(line, "\r\n")] = 0; //strip away whitespace characters
        if (strlen(line) == 0) continue; //if string empty continue

        if (strncmp(line, "EMAIL ", 6) == 0) { //email command
            Email new_email; //make new email format
            char *tokens = line + 6; // memory address manipulation by shifting address by 6 bytes

            // Extract fields using comma delimiters
            char *cat = strtok(tokens, ","); //sender
            char *subj = strtok(NULL, ","); //message
            char *date = strtok(NULL, ","); //date

            if (cat && subj && date) { //coppy message if all 3 parameters are met
                strcpy(new_email.category, cat); //copy string  category into a new destination buffer
                strcpy(new_email.subject, subj);
                strcpy(new_email.date_str, date);
                new_email.priority_score = get_category_score(cat); //rank based on who the sender is
                new_email.date_score = get_date_score(date); //if senders are both same rank email on how new it is

                insert_email(heap, new_email); //add it into the email
            }
        } 
        else if (strcmp(line, "COUNT") == 0) { //check how many elements are in heap
            printf("\nThere are %d emails to read.\n", heap->size); //print message of heap elements
        } 
        else if (strcmp(line, "NEXT") == 0) {
            if (heap->size > 0) { //as long has theres something in the heap
                Email top = heap->data[0]; //top is our priority email to read
                printf("\nNext email:\n");
                printf("\tSender: %s\n", top.category); //prints out who sent the email, message, and when it was sent
                printf("\tSubject: %s\n", top.subject);
                printf("\tDate: %s\n", top.date_str);
            }
        } 
        else if (strcmp(line, "READ") == 0) { //remove email from heap
            if (heap->size > 0) { //as long as heap isnt empty
                pop_email(heap); //remove element from heap
            }
        }
    }

    free_heap(heap); //free heap since our program has finished
    return 0; //code ran successfully
}
