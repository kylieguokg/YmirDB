/**
 * comp2017 - assignment 2
 * Kylie Guo
 * kguo5959
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#include "ymirdb.h"
#include <ctype.h>



/*
	BYE   clear database and exit

*/

int command_bye(entry** list_entries_head_ptr, snapshot** list_snapshots_head_ptr) {

	// Frees all allocated memory used in the program - in the entries and the snapshots
	free_entries_ptr(list_entries_head_ptr);

	if (list_entries_head_ptr != NULL){
		free(*list_entries_head_ptr);
	} 

	free_snapshots_ptr(list_snapshots_head_ptr);
	
	if (list_snapshots_head_ptr != NULL){
		free(*list_snapshots_head_ptr);
	} 

	printf("bye\n");

	return -1;
}


// Helper function to free the memory from all the entries
void free_entries_ptr(entry** list_entries_head_ptr){

	entry* head = *list_entries_head_ptr;
	
	if ((head -> num_entries) > 0){

		// start from 2nd entry
		entry* cursor = (*list_entries_head_ptr) -> next; 

		entry* next_to_free;

		while (true){

			next_to_free = cursor -> next;

			//if we've reached the head, delete the head and end
			if (cursor == *list_entries_head_ptr){ 
				del_entry(list_entries_head_ptr, (*list_entries_head_ptr) -> key);
				break;
			}

			del_entry(list_entries_head_ptr, cursor -> key);

			cursor = next_to_free;

		}

	} 

}

// Helper function to free the memory from all the entries
void free_snapshots_ptr(snapshot** list_snapshots_head_ptr){

	snapshot* snapshot_head = *list_snapshots_head_ptr;
	
	if (snapshot_head -> num_snapshots > 0){

		// start from 2nd entry
		snapshot* cursor = (*list_snapshots_head_ptr) -> next; 

		snapshot* next_to_free = NULL;

		while (cursor){

			next_to_free = cursor -> next;


			//if we've reached the head, delete the head and end
			if (cursor == *list_snapshots_head_ptr){ 
				del_snapshot(list_snapshots_head_ptr, cursor);
				break;
			}

			del_snapshot(list_snapshots_head_ptr, cursor);
			(*list_snapshots_head_ptr) -> num_snapshots -=1 ;

			cursor = next_to_free;

		}

	}

}


/*
	HELP  display this help message
*/ 
void command_help() {
	printf("%s\n", HELP);
}



/* 
	LIST KEYS       displays all keys in current state
*/

void command_list_keys(entry** list_entries_head_ptr){

	entry* list_entries_head = *list_entries_head_ptr;

	if ((list_entries_head -> num_entries) == 0){
		printf("no keys\n");
		printf("\n");
		return;
	}

	// start from most recently added
	entry* start = list_entries_head -> prev; 

	entry* cursor = start;

	while (cursor){

		// print each key of entry 
		printf("%s\n", cursor -> key); 


		// if we get back to the start, stop
		if (cursor -> prev != start){ 
			cursor = cursor -> prev;
		} else {
			break;
		}
	}

	printf("\n");

}


/*
	LIST ENTRIES    displays all entries in current state
*/

void command_list_entries(entry** list_entries_head_ptr){

	entry* list_entries_head = *list_entries_head_ptr;

	if ((list_entries_head -> num_entries) == 0){
		printf("no entries\n");
		printf("\n");
		return;
	}

	entry* start = list_entries_head -> prev; // start from most recently added

	entry* cursor = start;

	while (cursor){

		printf("%s ", cursor -> key);

		get_entry_elems(cursor -> key, list_entries_head_ptr);

		if (cursor -> prev != start){ // if we get back to the start, stop
			cursor = cursor -> prev;
		} else {
			break;
		}

	}

	printf("\n");

}



/*

	LIST SNAPSHOTS  displays all snapshots in the database
*/

void command_list_snapshots(snapshot** list_snapshots_head_ptr){


    snapshot* snapshot_head = *(list_snapshots_head_ptr);

	if (snapshot_head -> num_snapshots == 0){
		printf("no snapshots\n");
		printf("\n");
		return;
	}
  	snapshot* start = snapshot_head -> prev; // start from most recently added

	snapshot* cursor = start;

	while (cursor){

		printf("%d\n", cursor -> id);

		// if we get back to the start, stop
		if (cursor -> prev != start){ 
			cursor = cursor -> prev;
		} else {
			break;
		}
	}

	printf("\n");

}



/*
	GET <key>	displays entry values

*/

void command_get(char key[MAX_KEY], entry** list_entries_head_ptr){

	if (get_entry_elems(key, list_entries_head_ptr) == -1){
		printf("no such key\n");
	}
	printf("\n");
}



// Helper function to get entry without print statements
entry* get_entry(char key[MAX_KEY], entry** list_entries_head_ptr){

	if (list_entries_head_ptr == NULL || *list_entries_head_ptr == NULL){
		return NULL;
	}

	entry* list_entries_head = *list_entries_head_ptr; // start from the head

	if ((list_entries_head -> key)[0]  == 0){
		return NULL;
	}

	entry* cursor = list_entries_head;

	while (cursor){


		// if the key matches, entry found
		if (strcmp(cursor -> key, key) == 0){
			return cursor;
		}

		// if we get back to the start, stop
		if (cursor -> next == list_entries_head){ 
			break;
		} 

		cursor = cursor -> next;

	}

	return NULL;

}




// Prints out all the elements in an entry
int get_entry_elems(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* entry;

	// if entry doesn't exist
	if ( (entry = get_entry(key, list_entries_head_ptr)) == NULL){
		return -1; 
	}

	if (entry -> length == 0){
		printf("[]");
		printf("\n");
		return 1;
	}

	element* elem_head = *(entry -> elem_head_ptr); // start from the head
	element* cursor_elem = elem_head;

	printf("[");

	while (cursor_elem){

		if (cursor_elem -> type == 0) { // int
			printf("%d", cursor_elem -> value);
		} else { // entry
			printf("%s", (cursor_elem -> entry) -> key);  
		}

		// if this is the final elem, final bracket
		if (cursor_elem -> next == elem_head){
			printf("]\n");
			break;
		} else {
			printf(" ");
			cursor_elem = cursor_elem -> next;
		}

	}

	return 1;
}



/*
	
	DEL <key> deletes entry from current state

*/
void command_del(entry** list_entries_head_ptr, char key[MAX_KEY]){

	entry* my_entry = get_entry(key, list_entries_head_ptr);

	if (my_entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	// Before deleting the entry, check if the state will be valid
	if (check_state_valid_if_delete(list_entries_head_ptr, key) == false){
		printf("not permitted\n");
		printf("\n");
		return;
	}


	// remove the presence of the entry from other entries
	remove_from_pointers(my_entry, list_entries_head_ptr);

	// delete the entry
	del_entry(list_entries_head_ptr, key);


	printf("ok\n");
	printf("\n");

}


// Helper function to check if the resulting state is valid if an entry is deleted
bool check_state_valid_if_delete(entry** list_entries_head_ptr, char key[MAX_KEY]){

	entry* del_entry = get_entry(key, list_entries_head_ptr);


	// if the entry doesn't exist it will be fine 
	if (del_entry == NULL){ 
		return true;
	}

	// if the entry is referenced in other entries, it cannot be deleted
	if (del_entry -> backward_size > 0){
		return false;
	}

	return true;


}



// Helper function to delete entry and free memory
int del_entry(entry** list_entries_head_ptr, char key[MAX_KEY]){

	entry* del_entry = get_entry(key, list_entries_head_ptr);

	if (list_entries_head_ptr == NULL || *list_entries_head_ptr == NULL || del_entry == NULL){
		return -1;
	}

	/* 

		Then, delete the references of the entry

	*/


	// remove its references
	free_forward_references(del_entry);
	free(*(del_entry -> forward));
	free(del_entry -> forward);

	free_backward_references(del_entry);
	free(*(del_entry -> backward));
	free(del_entry -> backward);

	


	/* 

		First, delete the elements and free all the memory for the elems

	*/
	
	del_elems(del_entry);



	/* 

		Then, delete the entry and remove from list of entries

	*/

	entry* head = *list_entries_head_ptr;


	// delete the head (only element)
	if ((head -> num_entries) == 1){

		free(del_entry);

		// reset new head
		*list_entries_head_ptr = (entry*) malloc(sizeof(entry)); 

		(*list_entries_head_ptr) -> num_entries = 0;
		((*list_entries_head_ptr) -> key)[0] = 0; 
		(*list_entries_head_ptr) -> forward_size = 0;
		(*list_entries_head_ptr) -> backward_size = 0;

	} else {

		entry* prev = del_entry -> prev;
		entry* new_next = del_entry -> next;

		// connect the prev and next entries of the deleted entry
		prev -> next = new_next;
		new_next -> prev = prev;

		
		// if deleted entry was head, assign a new head
		if (del_entry == head){

			entry* new_head = head -> next;
			*list_entries_head_ptr = new_head;
			new_head -> num_entries = del_entry -> num_entries;
			
		}


		// free the entry's memory
		free(del_entry);

		(*list_entries_head_ptr) -> num_entries -= 1;
		
	}


	return 1;

}


// Helper function to delete all the forward references of an entry and clear the memory
void free_forward_references(entry* del_entry){

	if (del_entry -> forward_size == 0){
		return;
	}


	entry** forward_head_ptr = del_entry -> forward;

	entry* forward_head = *forward_head_ptr;

	entry* cursor = forward_head;

	entry* next_to_free = NULL;

	while (true){
		
		next_to_free = cursor -> next;

		// if we've reached the head, reset the head
		if (cursor -> next == forward_head){
			free(cursor);

			// reset a new head for the forward entries
			*(del_entry -> forward) = (entry*) malloc(sizeof(entry));
			entry* new_head = *(del_entry -> forward);
			(new_head -> key)[0] = 0; // default key
			del_entry -> forward_size = 0;
			break;
		}

		free(cursor);

		cursor = next_to_free;

	}

}


// Helper function to delete all the backward references of an entry and clear the memory
void free_backward_references(entry* del_entry){

	if(del_entry -> backward_size == 0){
		return;
	}

	entry** backwards_head_ptr = del_entry -> backward;

	entry* backward_head = *backwards_head_ptr;

	entry* cursor = backward_head;

	entry* next_to_free = NULL;

	while (true){
		
		next_to_free = cursor -> next;

		// if we've reached the head, delete the head and end
		if (cursor -> next == backward_head ){
			free(cursor);

			// reset a new head for the backward entries
			*(del_entry -> backward) = (entry*) malloc(sizeof(entry));
			entry* new_head = *(del_entry -> backward);
			(new_head -> key)[0] = 0; // default key
			del_entry -> backward_size = 0;
			break;
		}

		free(cursor);

		cursor = next_to_free;

	}


}




// Helper function to delete all the elements in an entry
void del_elems(entry* entry){

	if (entry != NULL){
		
		if (entry -> length == 0){
			free(entry -> elem_head_ptr);
			return;
		}

		element* elem_head = *(entry -> elem_head_ptr);


		// start from the second elem
		element* cursor_elem = elem_head -> next; 

		element* next_to_free_elem = NULL;

		while (cursor_elem){
			

			// if element is an entry type, have to free the entry too
			next_to_free_elem = cursor_elem -> next;

			// if we've reached the head, delete the head and end
			if (cursor_elem == elem_head){

				if (cursor_elem -> type == 1){ 
					free(cursor_elem -> entry);
				}

				free(elem_head);
				break;
			}


			// if element is an entry type, have to free the entry too
			if (cursor_elem -> type == 1){ 
				free(cursor_elem -> entry);
			}

			free(cursor_elem);
			
			cursor_elem = next_to_free_elem;

		}

		free(entry -> elem_head_ptr);
	}

}








// Helper function to remove an entry's presence from the other entries
void remove_from_pointers(entry* my_entry, entry** list_entries_head_ptr){


	/* 
		First, update its forward entries' backward entries.

	*/

	if (my_entry -> is_simple == false){
		entry** forward_ptr = my_entry -> forward;
		entry* forward_head = *forward_ptr;

		entry* cursor = forward_head;

		while (cursor){


			// delete this entry from its forward entries
			remove_backward_ref(cursor -> key, my_entry -> key, list_entries_head_ptr);

			if (my_entry -> backward_size > 0){

				// then, remove the entry's presence from each of its forward entries' back entries
				remove_backward_refs_from_forward(cursor, my_entry, list_entries_head_ptr);

			}

			if (cursor -> next == forward_head){
			break;
		}

		cursor = cursor -> next;
	}




	}

	/* 
		Then, update its backward entries' forward entries to remove the entry's forward entries
		but keep the entry itself (used for setting an entry with new elements)

	*/

	if ((my_entry -> backward_size > 0)){

		entry** backward_ptr = my_entry -> backward;
		entry* backward_head = *backward_ptr;

		entry* cursor = backward_head;

		while (cursor){

			if (my_entry -> forward_size > 0){

				// remove the entry's forward references from its back entries
				remove_forward_refs_from_backward(cursor, my_entry, list_entries_head_ptr);

			}


		if (cursor -> next == backward_head){
			break;
		}

		cursor = cursor -> next;
	}

	
	}

}



// Helper function to remove a specific entry's presence from its forward references' backward entries
void remove_forward_refs_from_backward(entry* del_from, entry* my_entry, entry** list_entries_head_ptr){

	entry* my_forward = *(my_entry -> forward);

	entry* cursor = my_forward;

	while (cursor){
		
		remove_forward_ref(del_from -> key, cursor -> key, list_entries_head_ptr);

		if (cursor -> next == my_forward){
			break;
		}

		cursor = cursor -> next;

	}
}

// Helper function to remove a specific entry's presence from its backward references' forward entries
void remove_backward_refs_from_forward(entry* delFrom, entry* my_entry, entry** list_entries_head_ptr){

	entry* my_backward = *(my_entry -> backward);

	entry* cursor = my_backward;

	while (cursor){
		
		remove_backward_ref(delFrom -> key, cursor -> key, list_entries_head_ptr);

		if (cursor -> next == my_backward){
			break;
		}

		cursor = cursor -> next;

	}
}


// Helper function to remove a specific entry from an entry's forward references
void remove_forward_ref(char key_del_from[MAX_KEY], char key_to_del[MAX_KEY], entry** list_entries_head_ptr){

	// get the entry to delete from from the main entries list
	entry* del_from = get_entry(key_del_from, list_entries_head_ptr); 

	entry** head_ptr = del_from -> forward;

	// get the pointer to the entry in its backward list
	entry* entry_to_del = get_entry(key_to_del, head_ptr); // delete the pointer to the entry in its forward list


	if ((del_from -> forward_size) == 1){

		// free the entry
		free(entry_to_del);

		// reset new head
		entry* new_head;

		*(del_from -> forward) =(entry*) malloc(sizeof(entry)); 

		new_head = *(del_from -> forward);  

		new_head -> next = new_head;
		new_head -> prev = new_head;

		del_from -> forward_size = 0; 
		(new_head -> key)[0] = 0; // default

	} else {

		entry* prev = entry_to_del -> prev;
		entry* new_next = entry_to_del -> next;

		// connect the prev and next entries of the deleted entry
		prev -> next = new_next;
		new_next -> prev = prev;

		if (entry_to_del == *(head_ptr)){
			entry* new_head = (*head_ptr) -> next;
			*head_ptr = new_head;
		}

		// free the entry's memory
		free(entry_to_del);

		del_from -> forward_size -= 1;
		
	}

}


// Helper function to remove a specific entry from an entry's backward references
void remove_backward_ref(char key_del_from[MAX_KEY], char key_to_del[MAX_KEY], entry** list_entries_head_ptr){


	// get the entry to delete from from the main entries list
	entry* del_from = get_entry(key_del_from, list_entries_head_ptr); 

	entry** head_ptr = del_from -> backward;

	// get the pointer to the entry in its backward list
	entry* entry_to_del = get_entry(key_to_del, head_ptr); 

	
	if ((del_from -> backward_size) == 1){

		// delete the head
		free(entry_to_del); 


		// reset new head
		entry* new_head;

		*(del_from -> backward) = (entry*) malloc(sizeof(entry)); 

		new_head = *(del_from -> backward);
		new_head -> next = new_head;
		new_head -> prev = new_head;

		del_from -> backward_size = 0; 
		(new_head  -> key)[0] = 0; // default keuy

	} else {

		entry* prev = entry_to_del -> prev;
		entry* new_next = entry_to_del -> next;

		// connect the prev and next entries of the deleted entry
		prev -> next = new_next;
		new_next -> prev = prev;

		if (entry_to_del == (*head_ptr)){
			entry* new_head = (*head_ptr) -> next;
			*head_ptr = new_head;
		}

		// free the entry's memory
		free(entry_to_del);

		del_from -> backward_size -= 1;

	}

}




/*
	
	PURGE <key> deletes entry from current state and snapshots

*/

void command_purge(char key[MAX_KEY], 
				   entry** list_entries_head_ptr, 
				   snapshot** list_snapshots_head_ptr){


	entry* my_entry = get_entry(key, list_entries_head_ptr);


	// check that it is valid to delete it from the current state
	if (check_state_valid_if_delete(list_entries_head_ptr, key) == false){
		printf("not permitted\n");
		printf("\n");
		return;
	}

	snapshot* snapshot_head = *(list_snapshots_head_ptr);

	// check that it is valid to delete from the snapshots

	if (snapshot_head -> num_snapshots > 0){

		snapshot* snapshot_cursor = snapshot_head;

		while (snapshot_cursor){


			entry** cur_snapshot_list_entries_head_ptr = snapshot_cursor -> list_entries_head_ptr;


			// check that it is valid to delete it from the snapshot
			if (check_state_valid_if_delete(cur_snapshot_list_entries_head_ptr, key) == false){
				printf("not permitted\n");
				printf("\n");
				return;
			}


			//if we've reached the head, delete the head and end
			if (snapshot_cursor -> next == *(list_snapshots_head_ptr)){ 
				break;
			}

			snapshot_cursor = snapshot_cursor -> next;


		}


		// if the resulting states of all snapshots and the current state are valid


		// remove the entry from each snapshot
		snapshot_cursor = *(list_snapshots_head_ptr);

	

		while (snapshot_cursor){

			entry** cur_snapshot_list_entries_head_ptr = snapshot_cursor -> list_entries_head_ptr;

			// get the snapshot version of the entry
			entry* snapshot_entry = get_entry(key, cur_snapshot_list_entries_head_ptr);

			if (snapshot_entry != NULL){


				// remove the presence of the entry from all the other entries
				remove_from_pointers(snapshot_entry, cur_snapshot_list_entries_head_ptr);

				// remove the entry
				del_entry(cur_snapshot_list_entries_head_ptr, key);
			}


			//if we've reached the head, delete the head and end
			if (snapshot_cursor -> next == *(list_snapshots_head_ptr)){ 
				break;
			}

			snapshot_cursor = snapshot_cursor -> next;


		}
	}



	// if the entry exists in the current state, remove it
	if (my_entry != NULL){

		// Remove the entry and its presence from the current state
		remove_from_pointers(my_entry, list_entries_head_ptr);
		del_entry(list_entries_head_ptr, key);

	}

	
	printf("ok\n");
	printf("\n");


	
}


/*
	SET <key> <value ...> sets entry values
*/
void command_set(char key[MAX_KEY], char values[MAX_LINE][MAX_KEY], entry** list_entries_head_ptr){

	int result = set_entry(key, values, list_entries_head_ptr);

	if (result == -1 ){
		printf("invalid key\n");
		printf("\n");
		return;
	} else if (result == -2){
		return;
	} else {
		printf("ok\n");
		printf("\n");
	}

}


int set_entry(char key[MAX_KEY], char values[MAX_LINE][MAX_KEY], entry** list_entries_head_ptr){
	
	if (key_valid(key) == false){
		return -1;
	}

	entry* seek_entry = get_entry(key, list_entries_head_ptr);

	// check if the elems are valid before setting the entry
	int i = 2; 

	// if the char is 0, there is nothing there
	while (values[i][0] != 0){

		if (elem_valid(list_entries_head_ptr, values[i], key) == false){
			return -2;
		}

		i += 1;

	}

	entry* list_entries_head = *list_entries_head_ptr;

	// if key doesn't exist 
	if (seek_entry == NULL){ 

		/// if this is the first entry, use the head entry
		if ((list_entries_head -> num_entries) == 0){

			list_entries_head = *list_entries_head_ptr;

			// Circular list set up for entries list
			list_entries_head -> next = list_entries_head;
			list_entries_head -> prev = list_entries_head;


			// Set up the forward and backward entries list for it 
			list_entries_head -> forward_size = 0;
			list_entries_head -> forward = (entry**) malloc(sizeof(entry*));
			*(list_entries_head -> forward) = (entry*) malloc(sizeof(entry));
			entry* forward_head = *(list_entries_head -> forward);
			(forward_head -> key)[0] = 0; // default key
			

			list_entries_head -> backward_size = 0;
			list_entries_head -> backward = (entry**) malloc(sizeof(entry*));
			*(list_entries_head -> backward) = (entry*) malloc(sizeof(entry));
			entry* backward_head = *(list_entries_head -> backward);
			(backward_head -> key)[0] = 0; // default key
			
			(list_entries_head -> num_entries) += 1;

			// add all the elems
			create_entry(list_entries_head, key, list_entries_head_ptr, values);


		} else {


			entry* new_entry = (entry*) malloc(sizeof(entry));

			// add the entry to the end of the list of entries
			entry* prev_end = list_entries_head -> prev;
			new_entry -> prev = prev_end; 
			new_entry -> next = list_entries_head; 
			list_entries_head -> prev = new_entry;
			prev_end -> next = new_entry;

			// Set up the forward and backward entries list for it 
			new_entry -> forward_size = 0;
			new_entry -> forward = (entry**) malloc(sizeof(entry*));
			*(new_entry -> forward) = (entry*) malloc(sizeof(entry));
			entry* forward_head = *(new_entry -> forward);
			(forward_head -> key)[0] = 0; // default key

			new_entry -> backward_size = 0;
			new_entry -> backward = (entry**) malloc(sizeof(entry*));
			*(new_entry -> backward) = (entry*) malloc(sizeof(entry));
			entry* backward_head = *(new_entry -> forward);
			(backward_head -> key)[0] = 0; // default key


			(list_entries_head -> num_entries) += 1;

			// add all the elems
			create_entry(new_entry, key, list_entries_head_ptr, values);


		}

	} else { // if an entry with the key already exists


		// Delete that entry and replace it with a new entry with that key
		// it retains its original entry position
		remove_from_pointers(seek_entry, list_entries_head_ptr);
		del_elems(seek_entry);
		free_forward_references(seek_entry);

		create_entry(seek_entry, key, list_entries_head_ptr, values);

	}

	return 1;


}


// Helper function to check the validity of a key - has to start with a letter
bool key_valid(char value[MAX_KEY]){

	if ((value[0] >= 'a' && value[0] <= 'z') || (value[0] >= 'A' && value[0] <= 'z')){
		return true;
	}

	return false;

}


// Helper function to check the validity of an element
bool elem_valid(entry** list_entries_head_ptr, char value[MAX_KEY], char my_key[MAX_KEY]){

	// special case for 0
	if (strcmp(value, "0") == 0){
		return true;
	}


	// Keys must begin with a letter
	// and atoi will return a 0 if the first char is a letter
	// so atoi will always return 0 for for keys (assuming valid)
	// note: will also return 0 if the value is 0 so special case needed

	// if an element is an entry
	else if (atoi(value) == 0){

		// Check that the first char is [a - z] or [A - Z]
		if ((value[0] >= 'a' && value[0] <= 'z') || (value[0] >= 'A' && value[0] <= 'z')){

			entry* dependent_entry = get_entry(value, list_entries_head_ptr);

			// if the key of the entry is the same as the the key of the elem's entry
			if (strcmp(my_key, value) == 0) {
				printf("not permitted\n");
				printf("\n");
				return false;
			} else if (dependent_entry == NULL){
				printf("no such key\n");
				printf("\n");
				return false;
			}

			return true;
		}

	} else if (atoi(value) != 0){ // if an element is an integer
		return true;
	} else {
		printf("invalid value");
		return false;
	}


	return false;

}



// Helper function to manage the creation of the elemenets in the entry
entry* create_entry(entry* new_entry, char key[MAX_KEY], entry** list_entries_head_ptr, char values[MAX_LINE][MAX_KEY] ){


	memmove(new_entry -> key, key, MAX_KEY);
	new_entry -> length = 0;

	// check the return value for malloc
	if (new_entry != NULL){

		new_entry -> is_simple = true; // default is simple
		
		// if no elements were given
		if (values[2][0] == 0){ 
			element** elem_head_ptr = (element**) malloc(sizeof(element*));
			new_entry -> elem_head_ptr = elem_head_ptr;
			return new_entry;
		}


		// create a pointer to the head of the elems for the elems for this entry
		element* elem_head = (element*) malloc(sizeof(element));
		element** elem_head_ptr = (element**) malloc(sizeof(element*));
		*elem_head_ptr = elem_head;
		new_entry -> elem_head_ptr = elem_head_ptr;

		// if cannot malloc
		if (elem_head == NULL || elem_head_ptr == NULL){ 
			return NULL;
		}

		// create the new element
		element* new_elem = entry_elem_init(new_entry, elem_head, list_entries_head_ptr, values[2]);

		// if any elem is invalid do not set the entry
		if (new_elem == NULL){ 
			return NULL;
		}

		// circular list set up
		elem_head -> next = elem_head;
		elem_head -> prev = elem_head;


		new_entry -> length += 1;

		int i = 3; // start from SET <key> <head> <value>


		// if the char is 0, there is nothing there
		while (values[i][0] != 0){


			// create the element
			create_elem(true, values[i], list_entries_head_ptr, new_entry);

			i +=1;

		}



	} else {

		printf("There is no memory for another entry");
		return NULL;
	}


	return new_entry;



}


// Helper function to manage the creation of an element and connect it to the list of elements for the entry
element* create_elem(bool append_mode, char value[MAX_KEY], entry** list_entries_head_ptr, entry* my_entry){

	element* new_elem;


	// allocate space for the new element
	new_elem = (element*) malloc(sizeof(element)); 

	// set up the data for the new element
	element* out = entry_elem_init(my_entry, new_elem, list_entries_head_ptr, value);

	if (out == NULL){
		free(new_elem);
		return NULL;
	}

	element* elem_head;
	
	// if this is the head element
	if (my_entry -> length == 0){
		
		*(my_entry -> elem_head_ptr) = new_elem;
		elem_head = *(my_entry -> elem_head_ptr);
		elem_head -> next = elem_head;
		elem_head -> prev = elem_head;
	} else {
		elem_head = *(my_entry -> elem_head_ptr);
	}


	if (append_mode){


		// add the element to the end of the circular list
		element* end = elem_head -> prev;


		new_elem -> next = elem_head;
		new_elem -> prev = end;

		end -> next = new_elem;
		elem_head -> prev = new_elem;


		// head pointer should stay the same
		*(my_entry -> elem_head_ptr) = elem_head; 
	
	} else {

		// push the elemnt - add the element to the front of the list

		element* prev_end = elem_head -> prev;
	
		new_elem -> prev = prev_end;
		new_elem -> next = elem_head;
		elem_head -> prev = new_elem;
		prev_end -> next = new_elem;

		// head pointer should point to the new element
		*(my_entry -> elem_head_ptr) = new_elem; 


	}

	my_entry -> length += 1;


	// update this entry in the relevant forward and backward lists
	update_entry_in_references(my_entry, list_entries_head_ptr);

	return new_elem;


}



// Helper function to set up a new element's data
element* entry_elem_init(entry* my_entry, element* new_elem, entry** list_entries_head_ptr,  char value[MAX_KEY]){


	if (new_elem != NULL){


		// special case for 0
		if (strcmp(value, "0") == 0){
			new_elem -> type = 0; // INTEGER
			new_elem -> value = 0;
		}

		// Keys must begin with a letter
		// and atoi will return a 0 if the first char is a letter
		// so atoi will always return 0 for keys 
		// note: will also return 0 if the value is 0 so special case needed

		else if (atoi(value) == 0){

			// Check that the first elem is [a - z] or [A - Z]

			if ((value[0] >= 'a' && value[0] <= 'z') || (value[0] >= 'A' && value[0] <= 'z')){

				entry* dependent_entry = get_entry(value, list_entries_head_ptr);

				if (dependent_entry != NULL){

					new_elem -> type = 1; // ENTRY
					my_entry -> is_simple = false;

					// add the entry to my entry's forward references
					add_forward_reference(my_entry, dependent_entry);

					// add my entry to the dependent entry's backward references
					add_backward_reference(my_entry, dependent_entry);

					// add any indirect references caused by the inclusion of this entry
					add_indirect_references(my_entry, list_entries_head_ptr);

					new_elem -> entry = (entry*) malloc(sizeof(entry));

					// put the entry at new_elem (shallow copy)
					*(new_elem -> entry) = *(dependent_entry);

				} else {
					return NULL;
				}

			}

		} else if (atoi(value) != 0){ // if the element is an integer
			
			new_elem -> type = 0; 
			new_elem -> value = atoi(value);


		} else {
			printf("invalid value");
			return NULL;
		}


		return new_elem;

	}

	return NULL;

}

// Helper function - Add a forward reference to an entry
void add_forward_reference(entry* my_entry, entry* dependent_entry){

	entry** my_forward = my_entry -> forward;

	entry* my_forward_head = *(my_entry -> forward);


	// if this is the first forward, set up the circular list 
	if (my_entry -> forward_size == 0){

		// add a shallow copy of the entry to the forward list
		*my_forward_head = *dependent_entry;
		my_forward_head -> next = my_forward_head;
		my_forward_head -> prev = my_forward_head;

		my_entry -> forward_size += 1; 

		// do not add the entry again if it's already there
	} else if (get_entry(dependent_entry -> key, my_forward) != NULL){ 

	} else {

		// allocate memory for this entry in the forward list
		entry* new_entry = (entry*) malloc(sizeof(entry));

		// add a shallow copy of the entry to the forward list
		*new_entry = *dependent_entry;

		// add the entry to the end of the list of entries
		entry* prev_end = my_forward_head -> prev;

		new_entry -> prev = prev_end; 
		new_entry -> next = my_forward_head;

		my_forward_head -> prev = new_entry;
		prev_end -> next = new_entry;


		my_entry -> forward_size += 1; 


	}

}


// Helper function - Add a backward reference to an entry
void add_backward_reference(entry* my_entry, entry* dependent_entry){

	entry** dependent_backward = dependent_entry -> backward;

	entry* dependent_head = *(dependent_entry -> backward);	

	// if this is the first backward, set up the circular list 
	if (dependent_entry -> backward_size == 0){

		*dependent_head = *my_entry;
		dependent_head -> next = dependent_head;
		dependent_head -> prev = dependent_head;

		dependent_entry -> backward_size += 1;

	// do not add the entry again if it's already there
	} else if (get_entry(my_entry -> key, dependent_backward) != NULL){

	} else {

		// allocate memory for this entry in the backward list
		entry* new_entry = (entry*) malloc(sizeof(entry));

		// add a shallow copy of the entry to the backward list
		*new_entry = *my_entry;

		
		entry* prev_end = dependent_head -> prev;

		// add the entry to the end of the list of entries
		new_entry -> prev = prev_end; 
		new_entry -> next = dependent_head;
		dependent_head -> prev = new_entry;
		prev_end -> next = new_entry;

		dependent_entry -> backward_size += 1; 

	}


}

// Helper function - adds indirect references caused by this entry
void add_indirect_references(entry* my_entry, entry** list_entries_head_ptr){


	/*
		Add this entry to its forward entries' backward entries

	*/
	
	if (my_entry -> is_simple != true){

		struct dyn_array* forward_arr = dyn_array_init();

		get_forward_pointers(my_entry, forward_arr);


		for (int i = 0; i < (forward_arr -> size); i++){

			char key[MAX_KEY];

			strcpy(key, dyn_array_get(forward_arr, i));

			// add the additional forward entries to this entry
			add_forward_reference(my_entry, get_entry(key, list_entries_head_ptr));

			// for each of the forward references, add this entry to their backward list
			add_backward_reference(my_entry, get_entry(key, list_entries_head_ptr)); 

		}

		dyn_array_free(forward_arr);
		
	}


}


// Helper function - to update the copy of the entry held 
// in its backward entries forward list
void update_entry_in_references(entry* my_entry, entry** list_entries_head_ptr){

	// Its backward entries' forward list  need to be updated
	if (my_entry -> backward_size > 0){

		entry** backward_ptr = my_entry -> backward;
		entry* backward_head = *backward_ptr;

		entry* cursor = backward_head;

		while (cursor){


			// remove the forward reference from the entry 
			remove_forward_ref(cursor -> key, my_entry -> key, list_entries_head_ptr);

			// get the updated copy of this entry in the central entries list
			entry* update_entry = get_entry(cursor -> key, list_entries_head_ptr); 

			// add it back
			add_forward_reference(update_entry, my_entry);

			// add back indirect references
			add_indirect_references(update_entry, list_entries_head_ptr);


			if (cursor -> next == backward_head){
					break;
			}

			cursor = cursor -> next;

		}
	}



}

// Helper function - sets the forward and backward references for an entry
void set_up_references(entry* my_entry, entry** list_entries_head_ptr){

	element* elem_head = *(my_entry -> elem_head_ptr);

	element* cursor = elem_head;

	while (cursor){

		if (cursor -> type == 1){ // if entry element

			// add the entry to my entry's forward references
			add_forward_reference(my_entry, cursor -> entry);

			// add my entry to the dependent entry's backward references
			add_backward_reference(my_entry, cursor -> entry);

			// add any indirect references caused by the inclusion of this entry
			add_indirect_references(my_entry, list_entries_head_ptr);

		}

		if (cursor -> next == elem_head){ // if we get back to the start, stop
			break;
		} 

	}



}



// Helper function - to attain all the forward pointers (including indirect ones) of an entry
// uses a pre order traversal
void get_forward_pointers(entry* my_entry, struct dyn_array* arr){

	if (my_entry -> is_simple){
		return;
	}

	entry** forward_ptr = my_entry -> forward;

	entry* forward_head = *forward_ptr;

	entry* cursor = forward_head;

	while (cursor){

		get_forward_pointers(cursor, arr);

		if (dyn_array_contains(arr, cursor -> key) == false){
			dyn_array_add(arr, cursor -> key);
		}
	
		if (cursor -> next == forward_head){
			break;
		}

		cursor = cursor -> next;
	}

}


// Helper function - creates a dynamic array of cstrings
struct dyn_array* dyn_array_init(){

	struct dyn_array* out = (struct dyn_array*) malloc(sizeof(struct dyn_array));

	out -> size = 0;
	out -> capacity = DEFAULT_SIZE;
	out -> array = (char **) malloc(DEFAULT_SIZE * sizeof(char) * MAX_KEY);

	return out;
}


// Helper function - adds a new cstring to the dynamic array
void dyn_array_add(struct dyn_array* dyn, char value[MAX_KEY]){

	// if over capacity, double the size of the array
	if ((dyn -> size + 1) > dyn -> capacity){
		
		// double size
		dyn -> capacity *= 2;

		(dyn -> array) = (char**) realloc(dyn -> array, sizeof(char) * MAX_KEY * (dyn -> capacity));

		if ((dyn -> array) != NULL){
            (dyn -> array)[dyn -> size] = (char *) malloc(sizeof(char) * MAX_KEY);
			strcpy(((dyn -> array)[dyn -> size]), value);
			dyn -> size += 1; // add the elem
			return;
		} else{
			printf("Cannot add element");
			return;
		}


	}


	// otherwise just add it 
    (dyn -> array)[dyn -> size] = (char *) malloc(sizeof(char) * MAX_KEY);
	strcpy((dyn -> array)[dyn -> size], value);
	dyn -> size += 1;

}




// Helper function -  Returns a pointer to the cstring in the dynamic array
char* dyn_array_get(struct dyn_array* dyn, int index){
	if (index > ((dyn -> size) - 1) || index < 0){
		return NULL;
	}

	return *((dyn -> array) + index);

}

// Helper function - checks if a cstring is in the dynamic array
bool dyn_array_contains(struct dyn_array* dyn, char value[MAX_KEY]){

	for (int i = 0; i < (dyn -> size); i++){
		if (strcmp((dyn -> array)[i], value) == 0){ // if the same
			return true;
		}
	}

	return false;

}



// Helper function - Frees the current array allocation
void dyn_array_free(struct dyn_array* dyn){
    if (dyn == NULL){
        return;
    }

    // free each element in the array
	for (int i = (dyn -> size) - 1; i >= 0; i --){
		free((dyn -> array)[i]);
	}

	// free the array
    free(dyn->array);

    // free the struct holding the array
    free(dyn);

    return;

}



/*
	PUSH <key> <value ...> pushes values to the front
*/
void command_push(char key[MAX_KEY], entry** list_entries_head_ptr, char values[MAX_LINE][MAX_KEY]){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	} 

	
	// check if the elems are valid before pushing 
	// - start fom 2 PUSH <entry> <elems...>
	int i = 2; 

	// if the char is 0, there is nothing there

	while (values[i][0] != 0){

		if (elem_valid(list_entries_head_ptr, values[i], entry -> key) == false){
			return;
		}

		i += 1;

	}

	// Add the elements now
	int j = 2; 

	while (values[j][0] != 0){ // if the char is 0, there is nothing there

		// since pushing, adding to the front so it becomes the new head
		element* new_elem = create_elem(false, values[j], list_entries_head_ptr, entry);

		*(entry -> elem_head_ptr) = new_elem;

		// if there is a reference, the entry is not simple
		if (new_elem -> type == 1){
			entry -> is_simple = false;
		}

		j +=1;

	}

	printf("ok\n");
	printf("\n");

}


/*
	APPEND <key> <value ...> appends values to the back
*/
void command_append(char key[MAX_KEY], entry** list_entries_head_ptr, char values[MAX_LINE][MAX_KEY]){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	// check if the elems are valid before setting the entry
	int j = 2; 

	// if the char is 0, there is nothing there

	while (values[j][0] != 0){

		if (elem_valid(list_entries_head_ptr, values[j], entry -> key) == false){
			return;
		}

		j += 1;

	}


	int i = 2; // elems start from index 2 (after APPEND <key>)

	while (values[i][0] != 0){ // if the char is 0, there is nothing there


		element* new_elem = create_elem(true, values[i], list_entries_head_ptr, entry);
		
		// if there is a reference, the entry is not simple
		if (new_elem -> type == 1){
			entry -> is_simple = false;
		}

		i +=1;

	}

	printf("ok\n");
	printf("\n");


}


/*
	PICK <key> <index>	displays value at index

*/
void command_pick(char key[MAX_KEY], entry** list_entries_head_ptr, int index){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}


	if ((index > (entry -> length)) || (index < 1)){
		printf("index out of range\n");
		printf("\n");
		return;
	}

	get_elem_at_index(key, list_entries_head_ptr, index);

}


// Helper function to get element at index
element* get_elem_at_index(char key[MAX_KEY], entry** list_entries_head_ptr, int index){

	entry* entry = get_entry(key, list_entries_head_ptr);

	element* elem_head = *(entry -> elem_head_ptr);

	element* cursor = elem_head;


	// since index begins at 1 and list begins from 0
	int i = 1;

	while (cursor){


		if (i == index){

			if (cursor -> type == 0){ // if integer element
				printf("%d\n", cursor -> value);

			} else { // if entry element
				printf("%s\n", (cursor -> entry) -> key);
			}

			printf("\n");
			break;

		}


		if (cursor -> next != elem_head){ // if we get back to the start, stop
			cursor = cursor -> next;
		} else {
			break;
		}

		i += 1;


	}

	return cursor;



}

/*
	
	PLUCK <key> <index>	displays and removes value at index

*/
void command_pluck(char key[MAX_KEY], entry** list_entries_head_ptr, int index){


	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	// index begins at 1
	if (index > (entry -> length) || index < 1){
		printf("index out of range\n");
		printf("\n");
		return;
	}


	remove_elem_at_index(key, list_entries_head_ptr, index);
	
}


// Helper function - removes element at an index
void remove_elem_at_index(char key[MAX_KEY], entry** list_entries_head_ptr, int index){
	
	element* elem = get_elem_at_index(key, list_entries_head_ptr, index);
	entry* entry = get_entry(key, list_entries_head_ptr);
	
	if (entry == NULL){
		return;
	}

	del_elem(entry, elem, list_entries_head_ptr);

	return;
}


// Helper function - to retrieve all the elements in a key's char values
void get_entry_char_values(char values [MAX_LINE][MAX_KEY], entry* entry){

	if (entry -> length < 1){
		return;
	}

	element* elem_head = *(entry -> elem_head_ptr);

	element* cursor = elem_head;

	int i = 2; // starts from index 2, <SET> <KEY> <elements...>

	// store all the values in a list
	while (cursor){

		if (cursor -> type == 0){
			// convert int to string
			sprintf(values[i], "%d" , cursor -> value); 
		} else {
			strcpy(values[i], (cursor -> entry) -> key);
		}

		if (cursor -> next == elem_head){
			break;
		}

		cursor = cursor -> next;
		i += 1;
	}	

}


// Helper function - to delete an element
void del_elem(entry* my_entry, element* del_elem, entry** list_entries_head_ptr){

	element** head = my_entry -> elem_head_ptr;

	if (head == NULL || *head == NULL || del_elem == NULL){
		return;
	}

	// if head is the only elem
	if (my_entry -> length == 1){ 

		my_entry -> length -= 1;


		// if element is an entry, free the entry as well, 
		// remove forward references
		if (del_elem -> type == 1){ 
			free(del_elem -> entry);
			free(del_elem);

			// reset the copy of the entry so that references are updated correctly
			char values[MAX_LINE][MAX_KEY] = {{0}};
			get_entry_char_values(values, my_entry);
			char key_copy[MAX_KEY];
			strcpy(key_copy, my_entry -> key);
			set_entry(key_copy, values, list_entries_head_ptr);
			
		} else {
			free(del_elem);
			*(my_entry -> elem_head_ptr) = NULL; // default is null
		}



	} else {

		element* prev = del_elem -> prev;
		element* new_next = del_elem -> next;

		// connect the prev and next elements of the deleted element
		prev -> next = new_next;
		new_next -> prev = prev;

		my_entry -> length -= 1;
		
		// if deleted element is the head, set a new head
		if (del_elem == (*head)){
			element* new_head = (*head) -> next;
			*(my_entry -> elem_head_ptr) = new_head;
		}

		// if element is an entry, free the entry as well
		if (del_elem -> type == 1){ 
			free(del_elem -> entry);
			free(del_elem);

			// reset the copy of the entry so that references are updated correctly
			char values[MAX_LINE][MAX_KEY] = {{0}};
			get_entry_char_values(values, my_entry);
			char key_copy[MAX_KEY];
			strcpy(key_copy, my_entry -> key);
			set_entry(key_copy, values, list_entries_head_ptr);
		} else {
			free(del_elem);
		}


	}


	// update the type of the entry after deleting the element
	update_entry_type(my_entry);

	
}


// Helper function - sets the type of the entry (simple or general)
void update_entry_type(entry* entry){

	if (entry -> length == 0){
		entry -> is_simple = true;
		return;
	}

	element* head = *(entry -> elem_head_ptr);

	element* cursor = head;

	entry -> is_simple = true;

	while (true){

		if (cursor -> type == 1){ // entry
			entry -> is_simple = false;
			break;
		}

		if (cursor -> next == head){
			break;
		}

		cursor = cursor -> next;

	}

}


/*
	POP <key> displays and removes the front value

*/
void command_pop(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}


	if (entry -> length == 0){
		printf("nil\n");
		printf("\n");
		return;
	}

	// indexing starts at 1
	remove_elem_at_index(key, list_entries_head_ptr, 1);

}


/*
	DROP <id>	deletes snapshot

*/
void command_drop(int index, snapshot** list_snapshots_head_ptr){

	snapshot* snapshot_head = *(list_snapshots_head_ptr);

	if (index > snapshot_head -> num_snapshots){
		printf("no such snapshot\n");
		printf("\n");
		return;
	}
	
	snapshot* snapshot = get_snapshot(index, list_snapshots_head_ptr);

	if (list_snapshots_head_ptr == NULL || *list_snapshots_head_ptr == NULL || snapshot == NULL){
		printf("no such snapshot\n");
		printf("\n");
		return;
	}


	del_snapshot(list_snapshots_head_ptr, snapshot);

	printf("ok\n");
	printf("\n");

}


// Helper function - gets the snapshot using its id
snapshot* get_snapshot(int index, snapshot** list_snapshots_head_ptr){

	
	snapshot* list_snapshots_head = *list_snapshots_head_ptr;

	if (index < 1){
		return NULL;
	}

	 // if it's uninitialised, it will be 0
	if ((list_snapshots_head -> id) == 0){
		return NULL;
	}

	snapshot* cursor = list_snapshots_head;

	while (cursor){

		if ((cursor -> id) == index){
			return cursor;
		}

		// if we get back to the start, stop
		if (cursor -> next != list_snapshots_head){ 
			cursor = cursor -> next;
		} else {
			break;
		}


	}

	return NULL;

}



// Helper function to delete the snapshot
void del_snapshot(snapshot** list_snapshots_head_ptr, snapshot* del_snapshot){

	/* 
		Delete all the entries for the snapshot
	*/

	snapshot* head_snapshot = *list_snapshots_head_ptr;


	// free the entries ptr stored in that snapshot
	free_entries_ptr(del_snapshot -> list_entries_head_ptr);
	free(*(del_snapshot -> list_entries_head_ptr));
	free(del_snapshot -> list_entries_head_ptr);

	/* 
		Delete the snapshot and remove from list of snapshots
	*/ 
	
	// if this is the only snapshot, delete the head and reset
	if (head_snapshot -> next == head_snapshot){  

		free(*list_snapshots_head_ptr);

		// reset new head
		*list_snapshots_head_ptr = (snapshot*)malloc(sizeof(snapshot)); 
		(*list_snapshots_head_ptr) -> num_snapshots = 0;
		(*list_snapshots_head_ptr) -> next = (*list_snapshots_head_ptr);
		(*list_snapshots_head_ptr) -> prev = (*list_snapshots_head_ptr);


	} else {

		snapshot* prev = del_snapshot -> prev;
		snapshot* new_next = del_snapshot -> next;

		// connect the prev and next snapshots of the deleted snapshot
		prev -> next = new_next;
		new_next -> prev = prev;

		// reset new head if snapshot deleted was head
		if (del_snapshot == head_snapshot){
			int num_snapshots = head_snapshot -> num_snapshots;
			snapshot* new_head = head_snapshot -> next;
			*list_snapshots_head_ptr = new_head;

			// make sure head stores num of snapshots
			new_head -> num_snapshots = num_snapshots; 
		}


		// free the snapshot's memory
		free(del_snapshot);

		
	}



}


/*
	ROLLBACK <id>	restores to snapshot and deletes newer snapshots

*/
void command_rollback(int index, snapshot** list_snapshots_head_ptr, entry** list_entries_head_ptr){

	snapshot* snapshot_head = *(list_snapshots_head_ptr);

	if (index > snapshot_head -> num_snapshots || index < 1){
		printf("no such snapshot\n");
		printf("\n");
		return;
	}

	// Restore to that snapshot
	if (command_checkout(index, list_snapshots_head_ptr, list_entries_head_ptr) == NULL){
		return;
	}


	snapshot* roll_snapshot = get_snapshot(index, list_snapshots_head_ptr);

	if (roll_snapshot -> next != snapshot_head){

		// start deleting from next entry
		snapshot* cursor = roll_snapshot -> next; 

		snapshot* next_to_free = NULL;

		snapshot* head = *(list_snapshots_head_ptr);

		while (cursor){

			next_to_free = cursor -> next;

			if ((cursor -> next) == head){ //if we've reached the end, stop it
				del_snapshot(list_snapshots_head_ptr, cursor);
				break;
			}

			del_snapshot(list_snapshots_head_ptr, cursor);

			cursor = next_to_free;

		}
	}

}

/*
	CHECKOUT <id>	replaces current state with a copy of snapshot
*/
snapshot* command_checkout(int index, snapshot** list_snapshots_head_ptr, entry** list_entries_head_ptr){

	snapshot* snapshot_head = *(list_snapshots_head_ptr);

	if (index > snapshot_head -> num_snapshots){
		printf("no such snapshot\n");
		printf("\n");
		return NULL;
	}


	snapshot* snapshot_to_copy = get_snapshot(index, list_snapshots_head_ptr);

	if (snapshot_to_copy == NULL){
		printf("no such snapshot\n");
		printf("\n");
		return NULL;
	}

	// make a copy of the snapshot
	snapshot* snapshot_copy = (snapshot*)(malloc(sizeof(snapshot)));
	entry** snapshot_to_copy_entries_ptr = snapshot_to_copy -> list_entries_head_ptr;	

	create_snapshot(snapshot_copy, snapshot_to_copy_entries_ptr); 

	// free the entries pointer that is currently in use
	free_entries_ptr(list_entries_head_ptr);
	free(*list_entries_head_ptr);


	// set the liste entries head ptr to a shallow copy of the snapshot's entries ptr
	*list_entries_head_ptr = *(snapshot_copy -> list_entries_head_ptr);

	// free the snapshot copy and its entries head ptr
	free(snapshot_copy -> list_entries_head_ptr);
	free(snapshot_copy);

	printf("ok\n");
	printf("\n");

	return snapshot_to_copy;
}





/*
	SNAPSHOT	saves the current state as a snapshot

*/
void command_snapshot(entry** list_entries_head_ptr, snapshot** list_snapshots_head_ptr){

	snapshot* list_snapshots_head = *list_snapshots_head_ptr;

	if (list_snapshots_head -> num_snapshots == 0){

		// set up circular list for list of snapshots
		list_snapshots_head -> num_snapshots  += 1; // snapshots index from 1
		list_snapshots_head -> prev = list_snapshots_head;
		list_snapshots_head -> next = list_snapshots_head;
		list_snapshots_head -> id = list_snapshots_head -> num_snapshots;
		
		create_snapshot(list_snapshots_head, list_entries_head_ptr);

	} else {

		// allocate memory for new snapshot
		snapshot* new_snapshot = (snapshot*) malloc(sizeof(snapshot));

		list_snapshots_head -> num_snapshots  += 1; // snapshots index from 1

		// add the entry to the end of the list of entries
		snapshot* prev_end = list_snapshots_head -> prev;

		new_snapshot -> prev = prev_end; 
		new_snapshot -> next = list_snapshots_head;

		list_snapshots_head -> prev = new_snapshot;
		prev_end -> next = new_snapshot;


		new_snapshot -> id = list_snapshots_head -> num_snapshots;
		create_snapshot(new_snapshot, list_entries_head_ptr);

	}

	printf("saved as snapshot %d\n", list_snapshots_head -> num_snapshots);
	printf("\n");

}




// Helper function - creates a snapshot
void create_snapshot(snapshot* new_snapshot, entry** list_entries_head_ptr){


	/*
		First, deep copy all the entries
	*/


	entry* src_head_entry = *(list_entries_head_ptr);

	bool first_entry = true;

	entry* src_cursor = src_head_entry -> next;
	entry* dest_head_entry;
	entry* dest_cursor;
	entry* last_entry_added;

	while (true){

		if (first_entry){

			// allocate memory for the snapshot's list of entries
			dest_head_entry = (entry*) malloc(sizeof(entry));
			new_snapshot -> list_entries_head_ptr = (entry**) malloc(sizeof(entry*));
			*(new_snapshot -> list_entries_head_ptr) = dest_head_entry;
		
			// if cannot malloc
			if (dest_head_entry == NULL || (new_snapshot -> list_entries_head_ptr) == NULL){
				return;
			}


			// deep copy the data of entry 
			strcpy(dest_head_entry -> key, src_head_entry -> key);
			dest_head_entry -> is_simple = src_head_entry -> is_simple;
			dest_head_entry -> length = src_head_entry -> length;
			dest_head_entry -> num_entries = src_head_entry -> num_entries;

			if (src_head_entry -> num_entries >= 1){

				// if an entry has no entries, it will have no forwards or backwards
				// can add forwards or backwards using set if needed

				// sets elem_head_ptr and elems
				copy_entry(dest_head_entry, src_head_entry); 
			

				// allocate memory for copying the entries' forwards and backwards list
				dest_head_entry -> forward_size = 0; 
				dest_head_entry -> forward = (entry**) malloc(sizeof(entry*));
				*(dest_head_entry-> forward) = (entry*) malloc(sizeof(entry));
				((*(dest_head_entry-> forward)) -> key)[0] = 0;

				dest_head_entry -> backward_size = 0; 
				dest_head_entry -> backward = (entry**) malloc(sizeof(entry*));
				*(dest_head_entry -> backward) = (entry*) malloc(sizeof(entry));
				((*(dest_head_entry -> backward)) -> key)[0] = 0;
			}


			// circular list set up
			dest_head_entry -> next = dest_head_entry;
			dest_head_entry -> prev = dest_head_entry;

			first_entry = false;

			dest_cursor = dest_head_entry;
			
		}

		last_entry_added = dest_cursor; 

		if ((src_head_entry -> num_entries) > 1){

			dest_cursor -> next = (entry *)malloc(sizeof(entry));

			dest_cursor = dest_cursor -> next; 
			dest_cursor -> prev = last_entry_added;

			// deep copy all the data in the entry
			strcpy(dest_cursor -> key, src_cursor-> key);
			dest_cursor -> is_simple = src_cursor -> is_simple;
			dest_cursor -> length = src_cursor -> length;

			// add to the end of the entries list
			dest_cursor -> next = dest_head_entry;
			dest_head_entry -> prev = dest_head_entry;

			// allocate memory for copying the entries' forwards and backwards list
			dest_cursor -> forward_size = 0; 
			dest_cursor-> forward = (entry**) malloc(sizeof(entry*));
			*(dest_cursor -> forward) = (entry*) malloc(sizeof(entry));
			((*(dest_cursor -> forward)) -> key)[0] = 0;

	
			dest_cursor -> backward_size = 0; 
			dest_cursor -> backward = (entry**) malloc(sizeof(entry*));
			*(dest_cursor -> backward) = (entry*) malloc(sizeof(entry));
			((*(dest_cursor -> backward)) -> key)[0] = 0;


			// deep copy all the data for the snapshot
			copy_entry(dest_cursor, src_cursor); 


			// if back to the start, end
			if (src_cursor -> next == src_head_entry){
				last_entry_added = dest_cursor;
				break;
			}

			src_cursor = src_cursor -> next;

		} else {
			break;
		}


	} 


	// Join the end and start of the entries list 
	entry* entry_head = *(new_snapshot -> list_entries_head_ptr);
	entry_head -> prev = last_entry_added;
	last_entry_added -> next = entry_head;


	/*

		Then, add all the relevant forwards and backwards for each each entry, 
		as we put the references in for elements that are entries

	*/


	entry* cursor = entry_head;

	while (cursor){


		// get the matching source copy of the entry
		entry* src_match = get_entry(cursor -> key, list_entries_head_ptr);

		if (src_match != NULL){

			// using the src copy of the entry, 
			// set the forward and backward entries of the current entry
			set_forward(cursor, src_match, new_snapshot -> list_entries_head_ptr);

			set_backward(cursor, src_match, new_snapshot -> list_entries_head_ptr);

		}

		// back to start done
		if (cursor -> next == entry_head){
			break;
		}	

		cursor = cursor -> next;
	}

}



// Helper function - to deep copy each element in an entry
void copy_entry(entry* dest, entry* src){

	if (src -> length == 0){
		dest -> elem_head_ptr = (element**) malloc(sizeof(element*));
		dest -> length = 0;
		return;
	}

	element* src_head = *(src -> elem_head_ptr);

	bool first_elem = true;
	
	element* src_cursor = src_head -> next;
	element* dest_cursor;
	element* last_elem_added;

	while (src_cursor){

		if (first_elem){

			// allocate memory for the list of elements pointer 
			// and element head in the entry
			element* dest_head = (element*) malloc(sizeof(element));
			element** dest_head_ptr = (element**) malloc(sizeof(element*));
			*dest_head_ptr = dest_head;
			dest -> elem_head_ptr = dest_head_ptr;

			// if cannot malloc
			if (dest_head == NULL || dest_head_ptr == NULL){
				return;
			}

			dest_head -> value = src_head -> value;

			if (src_head -> type == 1){ 

				// if element is not simple, allocate space for the entry
				dest_head -> entry = (entry*) malloc(sizeof (entry));

				// make a shallow copy of the entry
				*(dest_head -> entry) = *(src_head -> entry);
				
			}
			
			// deep copy all the data
			dest_head -> type = src_head -> type;
			dest_head -> next = dest_head;
			dest_head -> prev = dest_head;

			first_elem = false;

			dest_cursor = dest_head;

		}

		last_elem_added = dest_cursor;

		if (src -> length > 1){

			// allocate the memory for the next entry
			dest_cursor -> next = (element *)malloc(sizeof(element)); 
			dest_cursor = dest_cursor -> next; 
			dest_cursor -> prev = last_elem_added;

			// deep copy all the data
			dest_cursor -> value = src_cursor -> value;
			dest_cursor -> type = src_cursor -> type;

			if (src_cursor -> type == 1){ 
				// not simple, will set the entry later
				dest_cursor -> entry = (entry*) malloc(sizeof (entry));
				*(dest_cursor -> entry) = *(src_cursor-> entry);
			}


			// if next elem is head, done
			if (src_cursor -> next == src_head){
				last_elem_added = dest_cursor;
				break;
			}

			src_cursor = src_cursor -> next;

		} else {
			break;
		}

	} 


	// join the head and start of the list
	element* elem_head = *(dest -> elem_head_ptr);
	elem_head -> prev = last_elem_added;
	last_elem_added -> next = elem_head;

}



// Helper function - to add all the forward entries of an entry
// as well as set any entries referenced in its elements list
void set_forward(entry* dest, entry* src, entry** snapshot_entries_ptr){

	if (src -> forward_size < 1){
		return;
	}

	entry** src_forward_head_ptr = src -> forward;

	entry* src_forward_head = *src_forward_head_ptr;

	entry* cursor = src_forward_head;

	while (cursor){

		// get the snapshot matching dependent entry 
		entry* dependent_entry = get_entry(cursor -> key, snapshot_entries_ptr);
		
		// add it to the snapshot
		add_forward_reference(dest, dependent_entry);
		
		if (cursor -> next == src_forward_head){
			break;
		}

		cursor = cursor -> next;

	}


}


// Helper function - to add all the backward entries of an entry
void set_backward(entry* dest, entry* src, entry** snapshot_entries_ptr){

	if (src -> backward_size < 1){
		return;
	}

	entry** src_backward_head_ptr = src -> backward;

	entry* src_backward_head = *src_backward_head_ptr;

	entry* cursor = src_backward_head;

	while (cursor){

		// get the snapshot matching dependent entry 
		entry* my_entry = get_entry(cursor -> key, snapshot_entries_ptr);
		
		// add it to the entry's backward references
		add_backward_reference(my_entry, dest);
		
		if (cursor -> next == src_backward_head){
			break;
		}

		cursor = cursor -> next;

	
	}

	

}



/*
	MIN <key>	displays minimum value
*/
void command_min(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* my_entry = get_entry(key, list_entries_head_ptr);

	if (my_entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (my_entry -> length < 1){
		printf("empty entry\n");
		printf("\n");
		return;
	}


	int min = get_min(key, list_entries_head_ptr);

	printf("%d\n", min);
	printf("\n");
}



// Helper function - to get the min
int get_min(char key[MAX_KEY], entry** list_entries_head_ptr){
	
	entry* entry = get_entry(key, list_entries_head_ptr);

	element* elem_head = *(entry -> elem_head_ptr);

	element* cursor = elem_head -> next;

	int min; // set the min as the value of the first elem

	if (elem_head -> type == 0){ 
		// if integer element
		min = elem_head -> value;
	} else { 
		// if entry element
		min = get_min((elem_head -> entry) -> key, list_entries_head_ptr);
	}

	while (cursor){

		if (cursor -> type == 0){ // if integer element

			if ((cursor -> value) < min){
				min = cursor -> value;
			}
			
		} else { // if entry element

			int cursor_min = get_min((cursor -> entry) -> key, list_entries_head_ptr);

			if (cursor_min < min){
				min = cursor_min;
			}
		}


		// if we get back to the start, stop
		if (cursor -> next != elem_head){ 
			cursor = cursor -> next;
		} else {
			break;
		}


	}

	return min;

}


/*
	MAX <key>	displays maximum value
*/
void command_max(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (entry -> length < 1){
		printf("empty entry\n");
		printf("\n");
		return;
	}

	int max = get_max(key, list_entries_head_ptr);

	printf("%d\n", max);
	printf("\n");

}



// Helper function - to get the max
int get_max(char key[MAX_KEY], entry** list_entries_head_ptr){
	
	entry* entry = get_entry(key, list_entries_head_ptr);

	element* elem_head = *(entry -> elem_head_ptr);

	element* cursor = elem_head;


	// set the max as the value of the first elem
	int max; 

	if (elem_head -> type == 0){ // if integer element
		max = elem_head -> value;
	} else { // if entry element
		max = get_max((elem_head -> entry) -> key, list_entries_head_ptr);
	}


	while (cursor){

		if (cursor -> type == 0){ // if integer element
			if ((cursor -> value) > max){
				max = cursor -> value;
			}
		} else { // if entry element

			int cursor_max = get_max((cursor -> entry) -> key, list_entries_head_ptr);

			if (cursor_max > max){
				max = cursor_max;
			}
		}

		// if we get back to the start, stop
		if (cursor -> next != elem_head){ 
			cursor = cursor -> next;
		} else {
			break;
		}


	}
	return max;


}


/*
	SUM <key>	displays sum of values
*/
void command_sum(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	int sum = get_sum(key, list_entries_head_ptr);

	printf("%d\n", sum);
	printf("\n");

}



// helper function - sums the values in the entry
int get_sum(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* my_entry = get_entry(key, list_entries_head_ptr);
	
	if (my_entry -> length == 0){
		return 0;
	}

	element* elem_head = *(my_entry -> elem_head_ptr);

	element* cursor = elem_head;

	int sum = 0;

	while (cursor){

		if (cursor -> type == 0){ // if integer element
			sum += cursor -> value;
		} else { // if entry element

			// get the main copy of the entry
			entry* main_entry = get_entry((cursor -> entry) -> key, list_entries_head_ptr);
			sum +=  get_sum(main_entry -> key, list_entries_head_ptr);
		}

		// if we get back to the start, stop
		if (cursor -> next != elem_head){ 
			cursor = cursor -> next;
		} else {
			break;
		}

	}

	return sum;
}


/*

	LEN <key>	displays number of values

*/

void command_len(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	int len = get_len(entry, list_entries_head_ptr);

	printf("%d\n", len);
	printf("\n");
}


// Helper function - to get entry
int get_len(entry* my_entry, entry** list_entries_head_ptr){

	int len = 0;


	if (my_entry -> is_simple){
		len = my_entry -> length;
	} else {

		element* elem_head = *(my_entry -> elem_head_ptr);

		element* cursor = elem_head;


		while (cursor){

			if (cursor -> type == 0){ // if integer element
				len += 1;
			} else { // if entry element
				
				// get the main copy of the entry
				entry* main_entry = get_entry((cursor -> entry) -> key, list_entries_head_ptr);
				len +=  get_len(main_entry, list_entries_head_ptr);
			}

			// if we get back to the start, stop
			if (cursor -> next != elem_head){ 
				cursor = cursor -> next;
			} else {
				break;
			}


		}


	}

	return len;

}

/*
	REV <key>	reverses order of values (simple entry only)
*/
void command_rev(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (entry -> is_simple){

		if (entry -> length < 1){
			printf("ok\n");
			printf("\n");
			return;
		}

		element* elem_head = *(entry -> elem_head_ptr);

		// temporarily make it not circular 
		element* end = elem_head -> prev;
		elem_head -> prev = NULL;
		end -> next = NULL;

		element* cursor = elem_head;
		element* new_next;

		while (cursor){

			if (cursor == NULL){
				break;
			}

			// flip prev and next of each elem
			new_next = cursor -> prev;
			cursor -> prev = cursor -> next; 
			cursor -> next = new_next;

			cursor = cursor -> prev;

		}

		// make it a circular linked list again
		element* new_end = elem_head;
		element* new_head = end;

		new_end -> next = new_head;
		new_head -> prev = new_end;

		// set the end as the new head
		*(entry -> elem_head_ptr) = new_head; 


		printf("ok\n");
		printf("\n");

	} else {

		printf("Simple entry only\n");
		printf("\n");
		
	}

}



/*
	UNIQ <key>  removes repeated adjacent values (simple entry only)
*/
void command_uniq(char key[MAX_KEY], entry** list_entries_head_ptr){
		
	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (entry -> is_simple){
		if (entry -> length < 1){

		} else {
			element* elem_head = *(entry -> elem_head_ptr);

			element* cursor = elem_head;

			while (cursor){

				element* next = cursor -> next;

				if (next == elem_head){
					break;
				}

				// repeated adjacent values 
				if ((next -> value) == (cursor -> value)){ 
					del_elem(entry, next, list_entries_head_ptr);
					// cursor should stay the same if we've just deleted the next elem
					continue; 
				} 

				cursor = cursor -> next;

			}
			
		}
		
		printf("ok\n");
		printf("\n");

	} else {

		printf("Simple entry only\n");
		printf("\n");
		
	}

}

// Helper comparison function used for qsort
int ascending(const void* a, const void* b) {
   return ( *(int*)a - *(int*)b );
}


/*
	SORT <key>  sorts values in ascending order (simple entry only)

*/
void command_sort(char key[MAX_KEY], entry** list_entries_head_ptr){
	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (entry -> is_simple){
		if (entry -> length <= 1){

		} else {


			// get just the values of all the elems in a list
			int* values = (int*)malloc((entry -> length) * sizeof(int));
			element* elem_head = *(entry -> elem_head_ptr);
			int len = entry -> length;
			get_entry_values(values, entry);


			// sort the list in ascending order using quick sort
			qsort(values, len, sizeof(int),  ascending);


			element* cursor = elem_head;

			int i = 0;

			// set the values of all the elements in the sorted order
			while (cursor){

				cursor -> value = *(values + i);

				if (cursor -> next == elem_head){
					break;
				}

				cursor = cursor -> next;
				i += 1;
			}

			free(values);

		}

		printf("ok\n");
		printf("\n");

	} else {

		printf("Simple entry only\n");
		printf("\n");
		
	}
}



// Helper function to store the int values of an entry in a list
void get_entry_values(int* values, entry* entry){

	element* elem_head = *(entry -> elem_head_ptr);
	int i = 0;

	element* cursor = elem_head;

	// store all the values in a list
	while (cursor){

		*(values + i) = (cursor -> value);

		if (cursor -> next == elem_head){
			break;
		}

		cursor = cursor -> next;
		i += 1;
	}	


}


/*
	FORWARD <key> lists all the forward references of this key
*/
void command_forward(char key[MAX_KEY], entry** list_entries_head_ptr ){

	struct entry* my_entry = get_entry(key, list_entries_head_ptr);

	if (my_entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (my_entry -> forward_size == 0){
		printf("nil\n");
		printf("\n");
		return;
	}

	// store the keys of all the forward entries in a list of cstrings
	int size = my_entry -> forward_size;
	char** values = (char**) malloc(sizeof(char) * MAX_KEY * size);
	get_keys_forward(values, my_entry);

	qsort(values, size, sizeof(values[0]) , &lexicographical);


	for (int i = 0; i < (size - 1); i++){
		printf("%s, ", *(values + i));
	}

	printf("%s", *(values + size - 1));

	printf("\n");
	printf("\n");

	for (int i = 0; i < size; i++){
		free(*(values + i));
	}

	free(values);

}



// Helper function - get the forward keys of an entry 
// and store it in a list of cstrings
void get_keys_forward(char* values[MAX_KEY], entry* my_entry){

	entry** forward_ptr = my_entry -> forward;

	entry* forward_head = *forward_ptr;

	entry* cursor = forward_head;

	int i = 0;
 
	while (cursor){

		*(values + i) = (char *)malloc(sizeof(char) * MAX_KEY);
		strcpy(*(values + i), cursor -> key);
		
		if (cursor -> next == forward_head){
			break;
		}

		cursor = cursor -> next;
		i += 1;
	}	


}


// Helper function - for qsort, to compare strings lexicographically
int lexicographical(const void *x, const void *y ) {

	return (strcmp(*(const char **)x, *(const char **)y));  

}


/*
	BACKWARD <key> lists all the backward references of this key

*/
void command_backward(char key[MAX_KEY], entry** list_entries_head_ptr ){

	struct entry* my_entry = get_entry(key, list_entries_head_ptr);

	if (my_entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (my_entry -> backward_size == 0){
		printf("nil\n");
		printf("\n");
		return;
	}


	// store the keys of all the backward entries in a list of cstrings
	int size = my_entry -> backward_size;

	char** values = (char**) malloc(sizeof(char) * MAX_KEY * size);

	get_keys_backward(values, my_entry);

	qsort(values, size, sizeof(values[0]) , &lexicographical);


	for (int i = 0; i < (size - 1); i++){
		printf("%s, ", *(values + i));
	}

	printf("%s", *(values + size - 1));

	printf("\n");
	printf("\n");

	for (int i = 0; i < size; i++){
		free(*(values + i));
	}

	free(values);

}


// Helper function - get the backward keys of an entry 
// and store it in a list of cstrings
void get_keys_backward(char* values[MAX_KEY], entry* my_entry){

	entry** backward_ptr = my_entry -> backward;

	entry* backward_head = *backward_ptr;

	entry* cursor = backward_head;

	int i = 0;
 
	while (cursor){

		*(values + i) = (char *)malloc(sizeof(char) * MAX_KEY);
		strcpy(*(values + i), cursor -> key);
		
		if (cursor -> next == backward_head){
			break;
		}

		cursor = cursor -> next;
		i += 1;
	}	

}



/*
	TYPE <key> displays if the entry of this key is simple or general

*/
void command_type(char key[MAX_KEY], entry** list_entries_head_ptr){

	entry* entry = get_entry(key, list_entries_head_ptr);

	if (entry == NULL){
		printf("no such key\n");
		printf("\n");
		return;
	}

	if (entry -> is_simple){
		printf("simple\n");
	} else {
		printf("general\n");
	}

	printf("\n");
	
}



// call the command using the standard input revceived
int call_command(char list_of_commands[MAX_LINE][MAX_KEY], entry** list_entries_head_ptr, snapshot** list_snapshots_head_ptr){

	char key[MAX_KEY] = {0};

	if (strcasecmp(list_of_commands[0], "BYE") == 0){
		return command_bye(list_entries_head_ptr, list_snapshots_head_ptr);
	} else if (strcasecmp(list_of_commands[0], "HELP") == 0){
		command_help();
	} else if (strcasecmp(list_of_commands[0], "LIST") == 0){
		if (strcasecmp(list_of_commands[1], "KEYS") == 0){
			command_list_keys(list_entries_head_ptr);

		} else if (strcasecmp(list_of_commands[1], "ENTRIES") == 0){
			command_list_entries(list_entries_head_ptr);

		} else if (strcasecmp(list_of_commands[1], "SNAPSHOTS") == 0){
			command_list_snapshots(list_snapshots_head_ptr);
		} 
	} else if ((strcasecmp(list_of_commands[0], "GET")) == 0){
		strcpy(key, list_of_commands[1]);
		command_get(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "DEL")) == 0){
		strcpy(key, list_of_commands[1]);
		command_del(list_entries_head_ptr, key);

	} else if ((strcasecmp(list_of_commands[0], "PURGE")) == 0){
		strcpy(key, list_of_commands[1]);
		command_purge(key, list_entries_head_ptr, list_snapshots_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "SET")) == 0){
		strcpy(key, list_of_commands[1]);
		// for ease will pass in the whole list of commands and ignore the first command
		command_set(key, list_of_commands, list_entries_head_ptr); 
	

	} else if ((strcasecmp(list_of_commands[0], "PUSH")) == 0){
		strcpy(key, list_of_commands[1]);
		// for ease will pass in the whole list of commands and ignore the first command
		command_push(key, list_entries_head_ptr, list_of_commands); 

	} else if ((strcasecmp(list_of_commands[0], "APPEND")) == 0){
		strcpy(key, list_of_commands[1]);
		// for ease will pass in the whole list of commands and ignore the first command
		command_append(key, list_entries_head_ptr, list_of_commands); 

	} else if ((strcasecmp(list_of_commands[0], "PICK")) == 0){
		strcpy(key, list_of_commands[1]);
		int index = atoi(list_of_commands[2]); 
		command_pick(key, list_entries_head_ptr, index); 

	} else if ((strcasecmp(list_of_commands[0], "PLUCK")) == 0){
		strcpy(key, list_of_commands[1]);
		int index = atoi(list_of_commands[2]); 
		command_pluck(key, list_entries_head_ptr, index); 

	} else if ((strcasecmp(list_of_commands[0], "POP")) == 0){
		strcpy(key, list_of_commands[1]);
		command_pop(key, list_entries_head_ptr); 

	} else if ((strcasecmp(list_of_commands[0], "DROP")) == 0){
		int index = atoi(list_of_commands[1]); 
		command_drop(index, list_snapshots_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "ROLLBACK")) == 0){
		int index = atoi(list_of_commands[1]); 
		command_rollback(index, list_snapshots_head_ptr, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "CHECKOUT")) == 0){
		int index = atoi(list_of_commands[1]); 
		command_checkout(index, list_snapshots_head_ptr, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "SNAPSHOT")) == 0){
		command_snapshot(list_entries_head_ptr, list_snapshots_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "MIN")) == 0){
		strcpy(key, list_of_commands[1]);
		command_min(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "MAX")) == 0){
		strcpy(key, list_of_commands[1]);
		command_max(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "SUM")) == 0){
		strcpy(key, list_of_commands[1]);
		command_sum(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "LEN")) == 0){
		strcpy(key, list_of_commands[1]);
		command_len(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "REV")) == 0){
		strcpy(key, list_of_commands[1]);
		command_rev(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "UNIQ")) == 0){
		strcpy(key, list_of_commands[1]);
		command_uniq(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "SORT")) == 0){
		strcpy(key, list_of_commands[1]);
		command_sort(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "FORWARD")) == 0){
		strcpy(key, list_of_commands[1]);
		command_forward(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "BACKWARD")) == 0){
		strcpy(key, list_of_commands[1]);
		command_backward(key, list_entries_head_ptr);

	} else if ((strcasecmp(list_of_commands[0], "TYPE")) == 0){
		strcpy(key, list_of_commands[1]);
		command_type(key, list_entries_head_ptr);

	} else{
		printf("Invalid command\n");
		printf("\n");
	}


	return 1;

}


// Sort the standard input for commands
int get_command(char line[MAX_LINE], char list_of_commands[MAX_LINE][MAX_KEY]){

	int command_count = 0;

	bool first_command = true;

	int i = 0;	

	while(i < MAX_LINE){

		if (line[i] == 0){
			return -1;
		} else if (first_command){

			int char_count = 0;

			while ((!isspace(line[i])) && (i < MAX_LINE) && (line[i] != 0)){
				list_of_commands[command_count][char_count] = line[i];

	      		char_count += 1;
	      		i += 1;
	      	}

	      	command_count += 1;

	      	first_command = false;
			continue;

		} else if (isspace(line[i]) && line[i] != 0){

			i += 1;
			continue;

		}

		int char_count = 0;

		while ((!isspace(line[i])) && (i < MAX_LINE) && (line[i] != 0)){
			list_of_commands[command_count][char_count] = line[i];

			char_count += 1;
			i += 1;
		}

		if ((i + 1) < MAX_LINE){
			if (line[i + 1] != 0){
				command_count += 1;
		
			} 
		} else {
				i += 1;
		}
		

	}

	return 1;

}


int main(void) {

	char line[MAX_LINE] = {0};

	// Circular Linked List setup for list of entries
	entry* list_entries_head = (entry*) malloc(sizeof(entry));
	entry** list_entries_head_ptr = &list_entries_head;

	list_entries_head -> next = list_entries_head;
	list_entries_head -> prev = list_entries_head;
	(list_entries_head -> key)[0] = 0; // default
	list_entries_head -> num_entries = 0;

	// Circular Linked List setup for list of snapshots
	snapshot* list_snapshots_head = (snapshot*) malloc(sizeof(snapshot));
	snapshot** list_snapshots_head_ptr = &list_snapshots_head;
	list_snapshots_head -> next = list_snapshots_head;
	list_snapshots_head -> prev = list_snapshots_head;
	list_snapshots_head -> num_snapshots = 0;

	while (true) {
		printf("> ");

		if (NULL == fgets(line, MAX_LINE, stdin)) {
			command_bye(list_entries_head_ptr, list_snapshots_head_ptr);
			return 0;
		}

		char list_of_commands[MAX_LINE][MAX_KEY] = {{0}}; // initialise to empty 

		get_command(line, list_of_commands);

		if (call_command(list_of_commands, list_entries_head_ptr, list_snapshots_head_ptr) == -1){
			break;
		}

   }

   	return 0;

 }

