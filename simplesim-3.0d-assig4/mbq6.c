#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

Node* appendNode(Node* head, int data) {
    if (head == NULL) {
        return createNode(data);
    } else {
        Node* current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = createNode(data);
        return head;
    }
}

Node* createLinkedList(int size) {
    Node* head = NULL;
    int i;
    for (i = 1; i <= size; ++i) {
        head = appendNode(head, i);
    }
    return head;
}

int main() {
    Node* head = createLinkedList(10000);

    // Operations on the list (e.g., traverse, modify, etc.)

    // Free the allocated memory
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
// In this implementation, we created a linked list consisting of nodes, each containing 
// an integer and a pointer to the next node. We dynamically allocated and linked together 
// a total of 10000 nodes. The values of the linked list were accessed sequentially 
// using the next pointers. Due to the non-consecutive allocation of nodes, a stride 
// prefetcher would likely be ineffective in prefetching values for this list. However, an 
// open-ended prefetcher, equipped with additional stream buffers, can effectively track the 
// pattern of accesses. By utilizing this pattern, it's possible to prefetch the addresses of 
// subsequent nodes, leading to a reduced rate of cache misses.