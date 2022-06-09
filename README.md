# YmirDB
A key-value based data store using dynamic data structures. A simple entry stores purely integer values,  
whereas a general entry can store references to keys of a simple or general entry, in addition to integers.

Run using command line.

BYE   clear database and exit.  
HELP  display this help message.  

LIST KEYS       displays all keys in current state.  
LIST ENTRIES    displays all entries in current state.  
LIST SNAPSHOTS  displays all snapshots in the database.  

GET <key> displays entry values.  
DEL <key> deletes entry from current state.   
PURGE <key> deletes entry from current state and snapshots.   

SET <key> <value ...> sets entry values.  
PUSH <key> <value ...> pushes values to the front.     
APPEND <key> <value ...> appends values to the back. 

PICK <key> <index> displays value at index.  
PLUCK <key> <index> displays and removes value at index.  
POP <key> displays and removes the front value. 

DROP <id> deletes snapshot.   
ROLLBACK <id> restores to snapshot and deletes newer snapshots.  
CHECKOUT <id> replaces current state with a copy of snapshot.  
SNAPSHOT saves the current state as a snapshot.  

MIN <key> displays minimum value.   
MAX <key> displays maximum value.   
SUM <key> displays sum of values.  
LEN <key> displays number of values.   
  
REV <key> sorts values in ascending order (simple entry only).   
UNIQ <key> removes repeated adjacent values (simple entry only).    
SORT <key>  sorts values in ascending order (simple entry only).     
 
FORWARD <key> lists all the forward references of this key.  
BACKWARD <key> lists all the backward references of this key.  
TYPE <key> displays if the entry of this key is simple or general.  
