// File: main.c

#include <stdio.h>
#include <stddef.h>
#include "data.h"
#include "file.h"
#include "interface.h"
#include "sys.h"

int main(){
	Data *data = NULL;
	option_t option;
	for(;;){
		option = menuMain(&data);
		switch(option){
			case 1: // Display the whole database
				menuDisplayData(data);
				break;
			case 2: // Query a record
				menuQueryRecord(data);
				break;
			case 3: // Create a new record
				menuNewRecord(data);
				break;
			case 4: // Change an existing record
				menuUpdateRecord(data);
				break;
			case 5: // Delete a record
				menuDeleteRecord(data);
				break;
			case 6: // Save to file
				menuSaveData(data, "vault.dat");
				break;
			case 0: // Quit
				destroyData(data);
				return 0;
			default:
				clearInputBuffer();
				continue;
				break;
		}
	}
	return 0;
}
