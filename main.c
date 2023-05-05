#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS
#define DECK_SIZE 108

typedef struct card_s {
    char color[10];
    int value;
    char action[15];
    struct card_s* pt;
} card;

// Define players' hand structure
typedef struct node_s {
    card* c;
    struct node_s* next;
} node;

// Define center line
typedef struct centerrow_s {
    int numcards;
    card* head;
    card* tail;
} centerrow;

void print_card(card* c) {
    printf("%s ", c->color);
    if (c->value > 0) {
        printf("%d ", c->value);
    }
    else {
        printf("%s ", c->action);
    }
}

void print_list(node* head) {
    node* curr = head;
    while (curr != NULL) {
        printf(" ");
        print_card(curr->c);
        curr = curr->next;
    }
    printf("\n");
}

card* draw_card(card* deck[], int* deck_size) {
    if (*deck_size <= 0) {
        return NULL;
    }
    int idx = rand() % *deck_size;
    card* c = deck[idx];
    deck[idx] = deck[*deck_size - 1];
    deck[*deck_size - 1] = NULL;
    (*deck_size)--;
    return c;
}

void shuffle(card* deck) {
    srand(time(NULL));
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// Create the players' hands
void dealCards(card** p1_hand, card** p2_hand, card* deck) {
    int i;
    // Deal cards to players
    for (i = 0; i < 7; i++) {
        p1_hand[i] = &deck[i + 7];
        p2_hand[i] = &deck[i];
    }
}

// Add card to players hands
void add_to_player_hand(card* c, card* player_hand[], int* player_hand_size) {
    player_hand[*player_hand_size] = c;
    (*player_hand_size)++;
}

// Remove card from players hands
void remove_from_player_hand(card* player_hand[], int* player_hand_size, int index) {
    for (int i = index; i < *player_hand_size - 1; i++) {
        player_hand[i] = player_hand[i + 1];
    }
    (*player_hand_size)--;
}


// Add card to center row
void add_to_center_row(card* c, centerrow* center) {
    c->pt = NULL;
    if (center->numcards == 0) {
        center->head = c;
        center->tail = c;
    } else {
        center->tail->pt = c;
        center->tail = c;
    }
    center->numcards++;
}


// Remove card from center row
void remove_from_center_row(centerrow* center) {
    center->numcards--;
    if (center->numcards == 0) {
        center->head = NULL;
        center->tail = NULL;
    }
}

int main(void) {
    // Initialize the game
    FILE* fp;
    char line[108];
    int count = 0;
    centerrow center;
    int deck_size = DECK_SIZE;
    card* p1_hand[7], *p2_hand[7];
    int p1_hand_size = 0, p2_hand_size = 0;
    int current_player = 1;
    int turn_count = 0;
    card* deck = malloc(sizeof(card) * DECK_SIZE);
    
    // Open the file
    fp = fopen("deck.txt", "r");
    if (fp == NULL) {
        printf("Sorry I failed to open your file and will not pass.\n");
        return 1;
    }
    else{
        int shuffleornot = 0;
        
        printf("Let's Play a Game of DOS!!\n");
        printf("Press 1 to shuffle the DOS deck or 2 to load a deck from a file:");
        scanf("%d", &shuffleornot);
        printf("\n");
        
        if (shuffleornot == 1) {
            printf("\nThe deck has been shuffled!!\n");
            shuffle(deck);
        }
        else{
            printf("\nThe deck was not shuffled!!\n");
        }
    }
    
    // Read the file and initialize the deck
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0; // Remove newline characters
        sscanf(line, "%s %d %[^\n]", deck[count].color, &deck[count].value, deck[count].action);
        count++;
    }
    shuffle(deck);
    dealCards(p1_hand,p2_hand,deck);
    
    // Start game loop
    int game_over = 0;
    
    while (!game_over) {
    // Display the center line and the hand of the current player
        printf("Center: ");
        print_list(center.head);
        printf("Player %d Hand: ", current_player);
        print_list(current_player == 1 ? p1_hand[0] : p2_hand[0]);
    
        int choice = 0;
        printf("Player %d, choose a card to play (1-%d) or draw a card (0): ", current_player, current_player == 1 ? p1_hand_size : p2_hand_size);
        scanf("%d", &choice);
    
        if (choice == 0) {
        // Draw a card
            card* c = draw_card(deck, &deck_size);
            if (c == NULL) {
                printf("Oh no! The deck is empty!\n");
                game_over = 1;
            } else {
                printf("You drew ");
                print_card(c);
                printf("\n");
                add_to_player_hand(c, current_player == 1 ? p1_hand : p2_hand, current_player == 1 ? &p1_hand_size : &p2_hand_size);
                current_player = current_player == 1 ? 2 : 1;
            }
        } else if (choice >= 1 && choice <= (current_player == 1 ? p1_hand_size : p2_hand_size)) {
        // Play a card
        int index = choice - 1;
        card* c = current_player == 1 ? p1_hand[index] : p2_hand[index];
        
        if (center.numcards == 0 || strcmp(c->color, center.tail->color) == 0 || c->value == center.tail->value) {
            // Valid move play
            printf("You played ");
            print_card(c);
            printf("\n");
            remove_from_player_hand(current_player == 1 ? p1_hand : p2_hand, current_player == 1 ? &p1_hand_size : &p2_hand_size, index);
            add_to_center_row(c, &center);
        if (p1_hand_size == 0 || p2_hand_size == 0) {
                printf("Congratulations, Player %d wins!\n", p1_hand_size == 0 ? 2 : 1);
                game_over = 1;
            } else if (c->value == 10) {
                // Reverse turn order
                printf("Order reversed!\n");
                current_player = current_player == 1 ? 2 : 1;
            } else if (c->value == 12) {
                // Skip next player
                printf("Next player skipped!\n");
                current_player = current_player == 1 ? 2 : 1;
            } else {
                current_player = current_player == 1 ? 2 : 1;
            }
        } else {
            // Invalid move
            printf("Invalid move! Please Try again. \n");
            }
        } else {
        // Invalid input
        printf("Invalid input! Please try again.\n");
        }
       
        turn_count++;
    }

          // fin //
    printf("The game is over! Thanks for playing\n");
    return 0;
}
