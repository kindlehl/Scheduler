# Scheduler
Program for time management and task prioritization  

Displays a list of "items" that have a title, description, estimate for time to complete, due date, and a path to a script. 
Sorts the list of "items" based on the time_to_complete:time_until_due ratio. This essentially causes the most pressing "items"
to float to the top of the list, because they are more important to complete. 

## __Dependencies__  
* ncurses
## __Controls__  
* j,k - Down, Up
* a - Add Item
* r - Remove Item
* v - View Item
* escape - Exit menu

## Current Features

Adding, removing or viewing items in ncurses frontend

## Future Features

Daemon program to execute scripts when an item reaches its due date with support for variable substitution (Description, title,
etc.).
