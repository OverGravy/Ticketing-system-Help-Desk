#include "../../Libs/Server/Login_handler.h"

int Singing_in(int id, struct AgentNode* head){
    // generate a key for the user
    srand(time(NULL));
    int key = id * 12345 + (rand() % 2000 + 1); // even if the id is 0 you have a number that operate as a key 

    struct AgentNode* new_node = (struct AgentNode*)malloc(sizeof(struct AgentNode));
    if (new_node == NULL) {
        terminal_print(MSG_ERROR, "Memory allocation failed for new agent node", SERVER, "Server");
        return -1; 
    }

    // Initialize the new node with agent details
    new_node->agent.client_id = id;
    new_node->agent.password = key; // Store the generated key as password
    new_node->next = NULL;          // Initialize the next pointer to NULL

    // If the list is empty, set the new node as the head
    if (head == NULL)
    {
       head = new_node; 
       return key; // Return success
    }

    // else find the end of the list and append the new node
    struct AgentNode* current = head;
    while (current->next != NULL) {
        if (current->agent.client_id == id) { // check if agent is already in the list
            free(new_node);
            return 0; 
        }
        current = current->next; // append to the end of the list
    }
    current->next = new_node;

    return key; // Success
}
    
// Function to handle user login checking if the agent exists in the list
int Logging_in(int id, struct AgentNode* head){
    struct AgentNode* current = head->next; // Start from the first node
    while (current != NULL) {
        if (current->agent.client_id == id) {
            // User found, return success
            return 0;
        }
        current = current->next; // Move to the next node
    }
    // User not found, return failure
    return -1;
}