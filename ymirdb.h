#ifndef YMIRDB_H
#define YMIRDB_H

#define MAX_KEY 17 // 1 for null terminator
#define MAX_LINE 1024
#define DEFAULT_SIZE 5

#include <stddef.h>
#include <sys/types.h>
#include <stdbool.h>


struct dyn_array{
	int capacity;
	int size;
	char** array;
};

enum item_type {
    INTEGER=0,
    ENTRY=1
};

enum add_elem_mode {
    NEW_ENTRY = 0,
    TO_BACK = 1,
    TO_FRONT = 2
};


typedef struct element element;
typedef struct entry entry;
typedef struct snapshot snapshot;

struct element {
  enum item_type type;
  union {
    int value;
    struct entry *entry;
  };
  element* next;
  element* prev;
};

struct entry {
  char key[MAX_KEY];
  bool is_simple;
  element** elem_head_ptr;
  size_t length;

  entry* next;
  entry* prev;
  
  size_t forward_size; 
  entry** forward;  // this entry depends on these
  size_t backward_size; 
  entry** backward; // these entries depend on this
  
  int num_entries;

};

struct snapshot {
  int id;
  entry** list_entries_head_ptr;
  snapshot* next;
  snapshot* prev;
  int num_snapshots;
};



int command_bye(entry** list_entries_head_ptr, snapshot** list_snapshots_head_ptr);
void free_entries_ptr(entry** list_entries_head_ptr);
void free_snapshots_ptr(snapshot** list_snapshots_head_ptr);
void del_snapshot(snapshot** list_snapshots_head_ptr, snapshot* del_snapshot);

void command_help();

void command_list_keys(entry** list_entries_head_ptr);
void command_list_entries(entry** list_entries_head_ptr);
void command_list_snapshots(snapshot** list_snapshots_head_ptr);


void command_get(char key[MAX_KEY], entry** list_entries_head_ptr);
entry* get_entry(char key[MAX_KEY], entry** list_entries_head_ptr);
int get_entry_elems(char key[MAX_KEY], entry** list_entries_head_ptr);


void command_del(entry** list_entries_head_ptr, char key[MAX_KEY]);
bool check_state_valid_if_delete(entry** list_entries_head_ptr, char key[MAX_KEY]);
int del_entry(entry** list_entries_head_ptr, char key[MAX_KEY]);
void del_elems(entry* entry);
void free_forward_references(entry* del_entry);
void free_backward_references(entry* del_entry);
void remove_from_pointers(entry* my_entry, entry** list_entries_head_ptr);
void remove_forward_ref(char key_del_from[MAX_KEY], char key_to_del[MAX_KEY], entry** list_entries_head_ptr);
void remove_backward_ref(char key_del_from[MAX_KEY], char key_to_del[MAX_KEY], entry** list_entries_head_ptr);
void remove_forward_ref(char key_del_from[MAX_KEY], char key_to_del[MAX_KEY], entry** list_entries_head_ptr);
void remove_forward_refs_from_backward(entry* del_from, entry* my_entry, entry** list_entries_head_ptr);
void remove_backward_refs_from_forward(entry* delFrom, entry* my_entry, entry** list_entries_head_ptr);

void command_set(char key[MAX_KEY], char values[MAX_LINE][MAX_KEY], entry** list_entries_head_ptr);
int set_entry(char key[MAX_KEY], char values[MAX_LINE][MAX_KEY], entry** list_entries_head_ptr);
bool key_valid(char value[MAX_KEY]);
bool elem_valid(entry** list_entries_head_ptr, char value[MAX_KEY], char my_key[MAX_KEY]);
entry* create_entry(entry* new_entry, char key[MAX_KEY], entry** list_entries_head_ptr, char values[MAX_LINE][MAX_KEY] );
element* create_elem(bool append_mode, char value[MAX_KEY], entry** list_entries_head_ptr, entry* my_entry);
element* entry_elem_init(entry* my_entry, element* new_elem, entry** list_entries_head_ptr,  char value[MAX_KEY]);
void add_backward_reference(entry* my_entry, entry* dependent_entry);
void add_forward_reference(entry* my_entry, entry* dependent_entry);
void add_indirect_references(entry* my_entry, entry** list_entries_head_ptr);
void get_forward_pointers(entry* my_entry, struct dyn_array* arr);
void update_entry_in_references(entry* my_entry, entry** list_entries_head_ptr);

struct dyn_array* dyn_array_init();
void dyn_array_add(struct dyn_array* dyn, char value[MAX_KEY]);
char* dyn_array_get(struct dyn_array* dyn, int index);
bool dyn_array_contains(struct dyn_array* dyn, char value[MAX_KEY]);
void dyn_array_free(struct dyn_array* dyn);


void command_push(char key[MAX_KEY], entry** list_entries_head_ptr, char values[MAX_LINE][MAX_KEY]);
void command_append(char key[MAX_KEY], entry** list_entries_head_ptr, char values[MAX_LINE][MAX_KEY]);


void command_pick(char key[MAX_KEY], entry** list_entries_head_ptr, int index);
element* get_elem_at_index(char key[MAX_KEY], entry** list_entries_head_ptr, int index);
void remove_elem_at_index(char key[MAX_KEY], entry** list_entries_head_ptr, int index);
void del_elem(entry* my_entry, element* del_elem, entry** list_entries_head_ptr);
void update_entry_type(entry* entry);
void command_pop(char key[MAX_KEY], entry** list_entries_head_ptr);


void command_drop(int index, snapshot** list_snapshots_head_ptr);
snapshot* get_snapshot(int index, snapshot** list_snapshots_head_ptr);
void del_snapshot(snapshot** list_snapshots_head_ptr, snapshot* del_snapshot);
void command_rollback(int index, snapshot** list_snapshots_head_ptr, entry** list_entries_head_ptr);
snapshot* command_checkout(int index, snapshot** list_snapshots_head_ptr, entry** list_entries_head_ptr);
void command_snapshot(entry** list_entries_head_ptr, snapshot** list_snapshots_head_ptr);
void create_snapshot(snapshot* new_snapshot, entry** list_entries_head_ptr);
void copy_entry(entry* dest, entry* src);
void set_forward(entry* dest, entry* src, entry** snapshot_entries_ptr);
void set_backward(entry* dest, entry* src, entry** snapshot_entries_ptr);

void command_min(char key[MAX_KEY], entry** list_entries_head_ptr);
int get_min(char key[MAX_KEY], entry** list_entries_head_ptr);
void command_max(char key[MAX_KEY], entry** list_entries_head_ptr);
int get_max(char key[MAX_KEY], entry** list_entries_head_ptr);


void command_sum(char key[MAX_KEY], entry** list_entries_head_ptr);
int get_sum(char key[MAX_KEY], entry** list_entries_head_ptr);

void command_len(char key[MAX_KEY], entry** list_entries_head_ptr);
int get_len(entry* my_entry, entry** list_entries_head_ptr);

void get_entry_values(int* values, entry* entry);
void get_entry_char_values(char values [MAX_LINE][MAX_KEY], entry* entry);

void command_rev(char key[MAX_KEY], entry** list_entries_head_ptr);
void command_uniq(char key[MAX_KEY], entry** list_entries_head_ptr);
int ascending(const void* a, const void* b);
void command_sort(char key[MAX_KEY], entry** list_entries_head_ptr);
void get_entry_values(int* values, entry* entry);

void command_forward(char key[MAX_KEY], entry** list_entries_head_ptr);
void get_keys_forward(char* values[MAX_KEY], entry* my_entry);
int lexicographical(const void *x, const void *y );
void command_backward(char key[MAX_KEY], entry** list_entries_head_ptr );
void get_keys_backward(char* values[MAX_KEY], entry* my_entry);


const char* HELP =
	"BYE   clear database and exit\n"
	"HELP  display this help message\n"
	"\n"
	"LIST KEYS       displays all keys in current state\n"
	"LIST ENTRIES    displays all entries in current state\n"
	"LIST SNAPSHOTS  displays all snapshots in the database\n"
	"\n"
	"GET <key>    displays entry values\n"
	"DEL <key>    deletes entry from current state\n"
	"PURGE <key>  deletes entry from current state and snapshots\n"
	"\n"
	"SET <key> <value ...>     sets entry values\n"
	"PUSH <key> <value ...>    pushes values to the front\n"
	"APPEND <key> <value ...>  appends values to the back\n"
	"\n"
	"PICK <key> <index>   displays value at index\n"
	"PLUCK <key> <index>  displays and removes value at index\n"
	"POP <key>            displays and removes the front value\n"
	"\n"
	"DROP <id>      deletes snapshot\n"
	"ROLLBACK <id>  restores to snapshot and deletes newer snapshots\n"
	"CHECKOUT <id>  replaces current state with a copy of snapshot\n"
	"SNAPSHOT       saves the current state as a snapshot\n"
	"\n"
	"MIN <key>  displays minimum value\n"
	"MAX <key>  displays maximum value\n"
	"SUM <key>  displays sum of values\n"
	"LEN <key>  displays number of values\n"
	"\n"
	"REV <key>   reverses order of values (simple entry only)\n"
	"UNIQ <key>  removes repeated adjacent values (simple entry only)\n"
	"SORT <key>  sorts values in ascending order (simple entry only)\n"
	"\n"
	"FORWARD <key> lists all the forward references of this key\n"
	"BACKWARD <key> lists all the backward references of this key\n"
	"TYPE <key> displays if the entry of this key is simple or general\n";

#endif