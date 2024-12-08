#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef struct node {
    char *name;
    char *email;
    char *address;
    char *status; // "confirmed" or "pending"
    char *bookingCode;
    int seatNum; // -1 if no seat assigned (pending)
    struct node *next;
} node;

typedef struct {
    char *movieName;
    int maxSeats;
    int seatsTaken; // Keep track of seats currently occupied
    node *head;
    node *tail;
} Movie;

void initialize(Movie *queue, char *movieName, int maxSeats);
int isEmpty(Movie *q);
void enqueue(Movie *queue, char *name, char *email, char *address);
void dequeue(Movie *queue);
void canceling(Movie *queue, const char *name, const char *bookingCode);
void reassignSeat(Movie *queue);
char *generateBookingCode(const char *name);
char *nameInitials(const char *name);
void menu();
void displayBookings(Movie *queue);

int main(void) {
    Movie queue;
    initialize(&queue, "Inception", 5); // Example movie with 5 seats

    int choice;
    char name[50], email[50], address[100], bookingCode[20];

    do {
        menu();
        printf("\nEnter your choice: ");
        scanf(" %d", &choice);
        getchar(); // Clear the newline from input buffer

        switch (choice) {
            case 1:
                printf("Enter your name: \n");
                getchar();
                fgets(name, sizeof(name), stdin);


                printf("Enter your email: ");
                getchar();
                fgets(email, sizeof(email), stdin);

                printf("Enter your address: ");
                getchar();                
                fgets(address, sizeof(address), stdin);


                enqueue(&queue, name, email, address);
                break;

            case 2:
                dequeue(&queue);
                break;

            case 3:
                printf("Enter the name or booking code to cancel: ");
                fgets(bookingCode, sizeof(bookingCode), stdin);
                getchar();

                canceling(&queue, bookingCode, bookingCode); // Name and bookingCode checked
                break;

            case 4:
                displayBookings(&queue);
                break;

            case 5:
                printf("Exiting the system. Goodbye!\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);

    return 0;
}

void initialize(Movie *queue, char *movieName, int maxSeats) {
    queue->movieName = strdup(movieName);
    queue->maxSeats = maxSeats;
    queue->seatsTaken = 0;
    queue->head = NULL;
    queue->tail = NULL;
}

int isEmpty(Movie *q) {
    return q->head == NULL;
}

void enqueue(Movie* queue, char *name, char *email, char *address) {
    node *newNode = (node *)malloc(sizeof(node));//create node (beginner)
    if (newNode == NULL) {
        printf("Memory allocation has failed\n");
        exit(1);
    }

    newNode->name = strdup(name);
    newNode->email = strdup(email);
    newNode->address = strdup(address);
    newNode->bookingCode = strdup(generateBookingCode(newNode->name));
    newNode->seatNum = (queue->seatsTaken < queue->maxSeats) ? ++queue->seatsTaken : -1;
    newNode->status = (newNode->seatNum == -1) ? strdup("pending") : strdup("confirmed");
    newNode->next = NULL; //create node (end)

    if (isEmpty(queue)) {
        queue->head = queue->tail = newNode;
        queue->tail = newNode;
        return;

    }
     else {
        queue->tail->next = newNode; // newNode : wafen akhay tail ana jit 7dak
        queue->tail = newNode; //newNode : anwli ana howa tail A3chiri
    }

    printf("Booking added! Name: %s, Code: %s, Seat: %d\n", 
           newNode->name, newNode->bookingCode, newNode->seatNum);
}

void dequeue(Movie *queue) {
    if (isEmpty(queue)) { //checking if the queue is empty
        printf("No bookings to process.\n");
        return;
    }

    node *temp = queue->head; //
    printf("Processing booking:\n");
    printf("Name: %s, Email: %s, Code: %s, Seat: %d\n", 
           temp->name, temp->email, temp->bookingCode, temp->seatNum);

    queue->head = queue->head->next;

    //if there is a single element in the queue
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    if (temp->seatNum != -1) {
        queue->seatsTaken--;
        reassignSeat(queue);
    }

    free(temp->name);
    free(temp->email);
    free(temp->address);
    free(temp->status);
    free(temp->bookingCode);
    free(temp);
}



void canceling(Movie *queue, const char *name, const char *bookingCode) {
    if (isEmpty(queue)) {
        printf("No bookings to cancel.\n");
        return;
    }

    node *current = queue->head, *prev = NULL;
    while (current != NULL) {
        if ((strcmp(current->name, name) == 0) || 
            (strcmp(current->bookingCode, bookingCode) == 0)) {
            break;
        }
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Booking not found.\n"); 
        return;
    }   
    

    // Update the status to "cancelled"
    printf("Cancelling booking for %s (Code: %s)\n", current->name, current->bookingCode);
    free(current->status); // Free the old status
    current->status = strdup("cancelled"); // Set status to "cancelled"

    // If the cancelled booking had a seat, reassign the seat to the first pending booking
    if (current->seatNum != -1) {
        int cancelledSeatNum = current->seatNum;
        node *pending = queue->head;

        // Find the first pending booking to reassign the seat
        while (pending != NULL && strcmp(pending->status, "pending") != 0) {
            pending = pending->next;
        }

        if (pending != NULL) {
            // Assign the cancelled seat to the first pending booking
            pending->seatNum = cancelledSeatNum;
            free(pending->status);
            pending->status = strdup("confirmed"); // Change the status to confirmed
            printf("Assigned cancelled seat %d to pending booking: %s (Code: %s)\n", 
                   cancelledSeatNum, pending->name, pending->bookingCode);
        } else {
            printf("No pending bookings found to reassign the seat.\n");
        }
    }
}



void reassignSeat(Movie *queue) {
    node *current = queue->head;
    while (current != NULL) {
        if (strcmp(current->status, "pending") == 0) {
            current->seatNum = ++queue->seatsTaken;
            free(current->status);
            current->status = strdup("confirmed");
            printf("Reassigned seat %d to %s (Code: %s)\n", 
                   current->seatNum, current->name, current->bookingCode);
            return;
        }
        current = current->next;
    }
}

char *nameInitials(const char *name) {
    int index = 0, initFlag = 1;
    char *initials = (char *)malloc(10 * sizeof(char));
    if (initials == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; name[i] != '\0'; i++) {
        if (initFlag && isalpha(name[i])) {
            initials[index++] = toupper(name[i]);
            initFlag = 0;
        }
        if (name[i] == ' ') initFlag = 1;
    }
    initials[index] = '\0';
    return initials;
}

char *generateBookingCode(const char *name) {
    // Get initials
    char *initials = nameInitials(name);

    // Get current time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // Format time as "YYYYMMDDHHMMSS" (e.g., 20241122143045)
    char timeStr[15];
    sprintf(timeStr, "%04d%02d%02d%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Combine initials, time, and a random number to create the booking code
    char *bookingCode = (char *)malloc(strlen(initials) + strlen(timeStr) + 6);
    if (bookingCode == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Generate a random number
    //int randomNum = rand() % 10000; // Generate a random number between 0 and 9999

    // Format the final booking code: <Initials><Time><RandomNumber>
    sprintf(bookingCode, "%s%s%04d", initials, timeStr);

    free(initials); // Free the memory for initials as it's no longer needed
    return bookingCode;
}


void menu() {
    printf("\n--- Cinema Booking System ---\n");
    printf("1) Add a booking\n");
    printf("2) Process a booking (dequeue)\n");
    printf("3) Cancel a booking\n");
    printf("4) Display all bookings\n");
    printf("5) Exit\n");
}

void displayBookings(Movie *queue) {
    if (isEmpty(queue)) {
        printf("No bookings to display.\n");
        return;
    }

    node *current = queue->head;
    printf("\n--- Current Bookings ---\n");
    while (current != NULL) {
        printf("Name: %s, Email: %s, Code: %s, Seat: %s\n",
               current->name, current->email, current->bookingCode, 
               (current->seatNum == -1) ? "Pending" : "Confirmed");
        current = current->next;
    }
}
