/* 
//   Program:             TBD Chat Server
//   File Name:           linked_list.c
//   Authors:             Matthew Owens, Michael Geitz, Shayne Wierbowski
*/

#include "linked_list.h"

extern int numRooms;

int insertNode(Node **head, Node *new_node, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   Node *temp = *head;
   if(*head == NULL) {
      new_node->next = NULL;
      *head = new_node;
      pthread_mutex_unlock(&mutex);
      return 1;
   }

   while(temp->next != NULL) { temp = temp->next; }

   temp->next = new_node;
   new_node->next = NULL;
   pthread_mutex_unlock(&mutex);
   return 1;
}

int removeNode(Node **head, Node *to_remove, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   if(*head == NULL) {
      printf("Cannot remove from an empty list\n");
      pthread_mutex_unlock(&mutex);
      return 0;
   
   }

   Node *temp = *head;

   if(temp->data == to_remove->data) {
      *head = temp->next;
      pthread_mutex_unlock(&mutex);
      return 1;
   }

   while(temp->next != NULL) {
      if(temp->next->data ==  to_remove->data) {
         temp->next = temp->next->next;
         pthread_mutex_unlock(&mutex);
         return 1;
      }
   }

   printf("Specified node not found\n");
   pthread_mutex_unlock(&mutex);
   return 0;

   
}


/* Insert a new user node into the list over user nodes passed in */
int insertUser(Node **head, User *new_user, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   printf("Inserting %s\n", new_user->username);
   User *temp = *head;
   if (*head == NULL) {
      new_user->next = NULL;
      *head = new_user;
      printf("Insert Success\n");
      pthread_mutex_unlock(&mutex);
      return 1;
   } 
   
   if (strcmp(temp->username, new_user->username) == 0) {
      printf("Insert Failure\n");
      pthread_mutex_unlock(&mutex);
      return 0;
   }
   
   while (temp->next != NULL) {
      temp = temp->next;
      if (strcmp(temp->username, new_user->username) == 0) {
         printf("Insert Failure\n");
         pthread_mutex_unlock(&mutex);
         return 0;
      }
   }
   temp->next = new_user;
   new_user->next = NULL;
   printf("Insert Success\n");
   pthread_mutex_unlock(&mutex);
   return 1;
}


/* Remove a user node from the list of user nodes passed in */
int removeUser(Node **head, User *user, pthread_mutex_t mutex) {
   printf("Removing user: %s\n", user->username);
   pthread_mutex_lock(&mutex);
   printList(head, NULL);
   User *current = *head;
   if (*head == NULL) {
      printf("Can't remove from empty list.\n");
      pthread_mutex_unlock(&mutex);
      return 0;
   }
   
   if (strcmp(current->username, user->username) == 0) {
      *head = user->next;
      //current->next = user->next;
      //user->next = NULL;
      printf("Potentially removed a user from a list.\n");
      printList(head, NULL);
      pthread_mutex_unlock(&mutex);
      return 1;
   }
   while (current->next != NULL) {
      if (strcmp(current->next->username, user->username) == 0) {
         current->next = user->next;
         //user->next = NULL;
         printf("Potentially removed a user from a list.\n");
         printList(head, NULL);
         pthread_mutex_unlock(&mutex);
         return 1;
      }
      current = current->next;
   }
   //temp->next = new_user;
   //new_user->next = NULL;
   printf("User not found in list, nothing removed.\n");
   pthread_mutex_unlock(&mutex);
   return 0;
}


/* Return the display name for given user name in the list */
char *get_real_name(Node **head, char *user, pthread_mutex_t mutex) {
   char *error = "ERROR";
   pthread_mutex_lock(&mutex);
   User *temp = *head;
   
   if (*head == NULL) {
      pthread_mutex_unlock(&mutex);
      return error;
   }

   while (strcmp(user, temp->username) != 0) {
      if (temp->next == NULL) { 
         pthread_mutex_unlock(&mutex);
         return error;
      }
      temp=temp->next;
   }
   
   pthread_mutex_unlock(&mutex);
   return temp->real_name;
}


/* Return stored password for user */
char *get_password(Node  **head, char *user, pthread_mutex_t mutex) {
   char *error = "ERROR";
   pthread_mutex_lock(&mutex);
   User *temp = *head;
   
   if(*head == NULL) {
      pthread_mutex_unlock(&mutex);
      return error;
   }
   while(strcmp(user, temp->username) != 0) {
      if(temp->next == NULL) {
         pthread_mutex_unlock(&mutex);
         return error;
      }
      temp=temp->next;
   }
   pthread_mutex_unlock(&mutex);
   return temp->password;
}


User *get_user(Node **head, char *user, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   User *temp = *head;
   
   if(*head == NULL) {
      pthread_mutex_unlock(&mutex);
      return NULL;
   }

   while(strcmp(user, temp->username) != 0) {
      if(temp->next == NULL) {
         pthread_mutex_unlock(&mutex);
         return NULL;
      }
      temp = temp->next;
   }
   
   pthread_mutex_unlock(&mutex);
   return temp;
}

User *clone_user(User *user, pthread_mutex_t mutex) {
   printf("Cloning User\n");
   User *new_user = (User *)malloc(sizeof(User));
   pthread_mutex_lock(&mutex);
   printf("Old User: %s, %s, %s\n", user->username, user->real_name,
           user->password);
   strcpy(new_user->username, user->username);
   strcpy(new_user->real_name, user->real_name);
   strcpy(new_user->password, user->password);
   new_user->sock = user->sock;
   pthread_mutex_unlock(&mutex);
   new_user-> next = NULL;
   printf("New User: %s, %s, %s\n", new_user->username, new_user->real_name,
           new_user->password);
   return new_user;
}


/* Populate user list from Users.bin */
void readUserFile(Node **head, char *filename, pthread_mutex_t mutex) {
   int fd = open(filename, O_RDONLY);
   int n;
   int i;
   struct stat st;
   User *temp;
   
   *head = NULL;
   if(fd == -1) {
      close(fd);
      return;
   }
   else {
      fstat(fd, &st);
      n = (st.st_size / sizeof(User));
      //printf("n: %d\n", n);
      for(i = 0; i < n; i++) {
         temp = (User *)malloc(sizeof(User));
         read(fd, temp, sizeof(User));
         temp->next = NULL;
         //printf("%s, %s, %s\n", temp->username, temp->real_name, temp->password);
         insertUser(head, temp, mutex);
      }
   }
   close(fd);
}


/* Write user list to Users.bin */
void writeUserFile(Node  **head, char *filename, pthread_mutex_t mutex) {
   int fd = open(filename, O_WRONLY | O_CREAT, S_IRWXU);
   pthread_mutex_lock(&mutex);
   User *temp = *head;
   
   if(*head == NULL) {
      pthread_mutex_unlock(&mutex);
      close(fd);
      return;
   }
   
   //printf("wrote: %s, %s, %s\n", temp->username, temp->real_name, temp->password);
   write(fd, temp, sizeof(User));
   while(temp->next != NULL) {
      temp = temp->next;
      //printf("wrote: %s, %s, %s\n", temp->username, temp->real_name, temp->password);
      write(fd, temp, sizeof(User));
   }
   pthread_mutex_unlock(&mutex);
}


/* Print contents of list */
void printList(Node **head, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   User *temp = *head;
   printf("Printing User List\n");
   if(*head == NULL) {
      printf("NULL\n");
      pthread_mutex_unlock(&mutex);
      return;
   }
   
   printf("%s, %s, %s, %d\n", temp->username, temp->real_name, temp->password,
          temp->sock);
   while(temp->next != NULL) {
      temp = temp->next;
      printf("%s, %s, %s, %d\n", temp->username, temp->real_name, temp->password,
             temp->sock);
   }
   pthread_mutex_unlock(&mutex);
   printf("End User List\n");
}


// ROOM METHODS


/* Insert new room node to room list */
int insertRoom(Node **head, Room *new_room, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   Room *temp = *head;
   if(*head == NULL) {
      new_room->next = NULL;
      *head = new_room;
      pthread_mutex_unlock(&mutex);
      return 1;
   }
   
   if(strcmp(temp->name, new_room->name) == 0 || temp->ID == new_room->ID) {
      pthread_mutex_unlock(&mutex);
      return 0;
   }
   
   while(temp->next != NULL) {
      temp = temp->next;
      if(strcmp(temp->name, new_room->name) == 0 || temp->ID == new_room->ID) {
         pthread_mutex_unlock(&mutex);
         return 0;
      }
   }
   temp->next = new_room;
   new_room->next = NULL;
   pthread_mutex_unlock(&mutex);
   return 1;
}


int createRoom(Node **head, int ID, char *name, pthread_mutex_t mutex) {
   printf("Creating room %d %s\n", ID, name);
   Room *newRoom = (Room *) malloc(sizeof(Room));
   newRoom->ID = ID;
   newRoom->user_list_mutex = PTHREAD_MUTEX_INITIALIZER;
   strncpy(newRoom->name, name, sizeof(newRoom->name));
   newRoom->user_list = NULL;
   numRooms++;
   return insertRoom(head, newRoom, mutex);
}

/* Return ID of room node from its name*/
int Rget_ID(Node **head, char *name, pthread_mutex_t mutex) {
   int error = -1;
   pthread_mutex_lock(&mutex);
   Room *temp = *head;
   
   if(*head == NULL) {
      pthread_mutex_unlock(&mutex);
      return error;
   }
   while(strcmp(name, temp->name) != 0) {
      if(temp->next == NULL) {
         pthread_mutex_unlock(&mutex);
         return error;
      }
      temp=temp->next;
   }
   pthread_mutex_unlock(&mutex);
   return temp->ID;
}


/* Return name of room node from ID */
char *Rget_name(Node **head, int ID, pthread_mutex_t mutex) {
   char *error = "ERROR";
   pthread_mutex_lock(&mutex);
   Room *temp = *head;
   
   if(*head == NULL) {
      pthread_mutex_unlock(&mutex);
      return error;
   }
   while(ID != temp->ID) {
      if(temp->next == NULL) {
         pthread_mutex_unlock(&mutex);
         return error;
      }
      temp=temp->next;
   }
   pthread_mutex_unlock(&mutex);
   return temp->name;
}


/* Print contents of room list */
void RprintList(Node **head, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   Room *temp = *head;
   printf("Printing Room List\n");
   if(*head == NULL) {
      printf("NULL\n");
      pthread_mutex_unlock(&mutex);
      return;
   }
   
   printf("Room ID: %d, Room Name: %s,\n", temp->ID, temp->name);
   printf("Contains Users...\n");
   //printList(&(temp->user_list));
   while(temp->next != NULL) {
      temp = temp->next;
      printf("Room ID: %d, Room Name: %s,\n", temp->ID, temp->name);
      printf("Contains Users...\n");
      printList(&(temp->user_list), temp->user_list_mutex);
   }
   printf("End Room List\n");
   pthread_mutex_unlock(&mutex);
}


/*  */
Room *Rget_roomFID(Node **head, int ID, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   Room *temp = *head;
   
   if(*head == NULL) {
      pthread_mutex_unlock(&mutex);
      return NULL;
   }
   while(ID != temp->ID) {
      if(temp->next == NULL) {
         pthread_mutex_unlock(&mutex);
         return NULL;
      }
      temp = temp->next;
   }
   pthread_mutex_unlock(&mutex);
   return temp;
}


/*  */
Room *Rget_roomFNAME(Node **head, char *name, pthread_mutex_t mutex) {
   pthread_mutex_lock(&mutex);
   Room *temp = *head;
   
   if(*head == NULL) {
      pthread_mutex_unlock(&mutex);
      return NULL;
   }
   while(strcmp(name, temp->name) != 0) {
      if(temp->next == NULL) {
         pthread_mutex_unlock(&mutex);
         return NULL;
      }
      temp = temp->next;
   }
   
   pthread_mutex_unlock(&mutex);
   return temp;
}
