# Scheduler
Program for time management and task prioritization  

Displays a list of "items" that have a title, description, estimate for time to complete, due date, and a path to a script. 
Sorts the list of "items" based on the time_to_complete:time_until_due ratio. This essentially causes the most pressing "items"
to float to the top of the list, because it is important to complete them sooner. 

## __Dependencies__  

* ncurses  

## __Build__  

`$ make -B`  

## __Controls__  

* j,k - Down, Up
* a - Add Item
* r - Remove Item
* v - View Item
* q - Exit current menu context

## Current Features

Adding, removing or viewing items in ncurses frontend

## Future Features

Daemon program to execute scripts when an item reaches its due date with support for variable substitution (Description, title,
etc.)
