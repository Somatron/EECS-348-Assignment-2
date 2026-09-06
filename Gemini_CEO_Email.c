/*
    Program name: Gemini_CEO_Email.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 256

// Structure to hold individual email data
typedef struct {
    char category[50];
    char subject[MAX_STR];
    char date_str[20];
    int priority_score; // Numeric score for category
    long date_score;     // Numeric score for date (YYYYMMDD)
} Email;

// Max-Heap structure for the priority queue
typedef struct {
    Email *data;
    int size;
    int capacity;
} MaxHeap;

// Helper to convert category string to an integer score
int get_category_score(const char *cat) {
    if (strcmp(cat, "Boss") == 0) return 5;
    if (strcmp(cat, "Subordinate") == 0) return 4;
    if (strcmp(cat, "Peer") == 0) return 3;
    if (strcmp(cat, "ImportantPerson") == 0) return 2;
    if (strcmp(cat, "OtherPerson") == 0) return 1;
    return 0;
}

// Helper to convert MM-DD-YYYY string to YYYYMMDD integer for chronological comparison
long get_date_score(const char *date_str) {
    int month, day, year;
    if (sscanf(date_str, "%d-%d-%d", &month, &day, &year) == 3) {
        return (long)year * 10000 + month * 100 + day;
    }
    return 0;
}

// Compares two emails. Returns > 0 if a has higher priority than b
int compare_emails(Email a, Email b) {
    if (a.priority_score != b.priority_score) {
        return a.priority_score - b.priority_score;
    }
    // If categories match, newer date (larger YYYYMMDD) has higher priority
    if (a.date_score > b.date_score) return 1;
    if (a.date_score < b.date_score) return -1;
    return 0;
}

// Initialize Max-Heap
MaxHeap* create_heap() {
    MaxHeap *heap = (MaxHeap *)malloc(sizeof(MaxHeap));
    heap->capacity = 10;
    heap->size = 0;
    heap->data = (Email *)malloc(heap->capacity * sizeof(Email));
    return heap;
}

// Heapify up when inserting an element
void heapify_up(MaxHeap *heap, int index) {
    int parent = (index - 1) / 2;
    while (index > 0 && compare_emails(heap->data[index], heap->data[parent]) > 0) {
        Email temp = heap->data[index];
        heap->data[index] = heap->data[parent];
        heap->data[parent] = temp;
        
        index = parent;
        parent = (index - 1) / 2;
    }
}

// Heapify down when removing the root element
void heapify_down(MaxHeap *heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && compare_emails(heap->data[left], heap->data[largest]) > 0) {
        largest = left;
    }
    if (right < heap->size && compare_emails(heap->data[right], heap->data[largest]) > 0) {
        largest = right;
    }

    if (largest != index) {
        Email temp = heap->data[index];
        heap->data[index] = heap->data[largest];
        heap->data[largest] = temp;
        heapify_down(heap, largest);
    }
}

// Insert an email into the priority queue
void insert_email(MaxHeap *heap, Email email) {
    if (heap->size >= heap->capacity) {
        heap->capacity *= 2;
        heap->data = (Email *)realloc(heap->data, heap->capacity * sizeof(Email));
    }
    heap->data[heap->size] = email;
    heapify_up(heap, heap->size);
    heap->size++;
}

// Remove and return the highest priority email
void pop_email(MaxHeap *heap) {
    if (heap->size == 0) return;
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    heapify_down(heap, 0);
}

// Clean up heap memory
void free_heap(MaxHeap *heap) {
    free(heap->data);
    free(heap);
}

int main() {
    MaxHeap *heap = create_heap();
    char line[MAX_STR * 3];

    while (fgets(line, sizeof(line), stdin)) {
        // Strip newline characters
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        if (strncmp(line, "EMAIL ", 6) == 0) {
            Email new_email;
            char *tokens = line + 6;

            // Extract fields using comma delimiters
            char *cat = strtok(tokens, ",");
            char *subj = strtok(NULL, ",");
            char *date = strtok(NULL, ",");

            if (cat && subj && date) {
                strcpy(new_email.category, cat);
                strcpy(new_email.subject, subj);
                strcpy(new_email.date_str, date);
                new_email.priority_score = get_category_score(cat);
                new_email.date_score = get_date_score(date);

                insert_email(heap, new_email);
            }
        } 
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap->size);
        } 
        else if (strcmp(line, "NEXT") == 0) {
            if (heap->size > 0) {
                Email top = heap->data[0];
                printf("Next email:\n");
                printf("Sender: %s\n", top.category);
                printf("Subject: %s\n", top.subject);
                printf("Date: %s\n", top.date_str);
            }
        } 
        else if (strcmp(line, "READ") == 0) {
            if (heap->size > 0) {
                pop_email(heap);
            }
        }
    }

    free_heap(heap);
    return 0;
}
