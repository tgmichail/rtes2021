#include "covidTrace.h"

//Recent contacts
struct recent_contacts rec_conts;

void register_rec_contact(macaddress contact, uint64_t sec){
	int pointer = rec_conts.current_line;
	
	rec_conts.contacts[pointer] = contact;
	rec_conts.time_sec[pointer] = sec;
	
	//increase where the pointer shows
	//set to 0, if end of matrix
	rec_conts.current_line++;
	if (rec_conts.current_line == 120)
		rec_conts.current_line = 0;
}

//Close contacts
struct close_contacts close_conts;

bool is_close_contact(macaddress contact, uint64_t curr_time_sec){
	uint64_t time_diff;
	
	//scan from older entries to newer ones
	for (int i = rec_conts.current_line ; i<120; i++){
		time_diff = curr_time_sec - rec_conts.time_sec[i];
		if ((rec_conts.contacts[i] == contact)&&
							(time_diff > 4*60)&&
							(time_diff < 20*60))
			return 1;
	}
	for (int i = 0; i < rec_conts.current_line ; i++){
		time_diff = curr_time_sec - rec_conts.time_sec[i];
		if ((rec_conts.contacts[i] == contact)&&
							(time_diff > 4*60)&&
							(time_diff < 20*60))
			return 1;
	}

	return 0;
};

void delete_close_contact(int which, int how_many){
	//how_many elements behind which element and including it, will be deleted
	//case deleting old contacts: which = 3, how_many = 4
	//case deleting duplicate contact: which = 3, how_many = 1
	
	for (int i = which + 1; i < close_conts.line; i++){
		close_conts.contacts[i - how_many] = close_conts.contacts[i];
		close_conts.time_sec[i - how_many] = close_conts.time_sec[i];
	} 
	
	close_conts.line = close_conts.line - how_many;
};

void register_close_contact(macaddress contact, uint64_t sec){

	//detect existing entries and delete them
	for (int i=0; i < close_conts.line; i++){
		if (close_conts.contacts[i] == contact){
			delete_close_contact(i, 1);
		}
	}
	
	//register new close contact
	close_conts.contacts[close_conts.line] = contact;
	close_conts.time_sec[close_conts.line] = sec;
	close_conts.line++;
	//printf("Close Contact: %lld\n", contact);
		
}

//Other functions
macaddress BTnearMe(){
	macaddress rc;
	
	//me pithanothta 1/30 epistrefei apo to set twn 150 kai me 29/30 complete random
	if (rand()%30)
		rc = (macaddress)0xA0AE<<32 | rand();
	else
		rc = 0xCE391016B116 + rand()%150;
		
	//printf("Contact: %lld\n", rc);
	
	return rc;
}

bool testCovid(){

	return !(rand()%POSITIVE_PROB_INV);
}

void uploadContacts(uint64_t actual_time_sec){	//orismata: macaddress* close_c, int contacts_count
	//delete old contacts, >14 days
	int how_many_to_delete = 0, which = 0;
	for (int i=0; i < close_conts.line; i++){
		//find the first element inside the 14 day interval
		if (actual_time_sec - close_conts.time_sec[i] < 14*24*60*60){
			how_many_to_delete = i;	//arrays start from 0, thus 3 means 4th element and the previous 3 to be deleted
			which = i-1;
			break;
		}
	}
	if (how_many_to_delete)
		delete_close_contact(which, how_many_to_delete);
	
	//upload contacts in file
	//printf("Uploading contacts...\n");
	
	FILE* contacts_file = fopen("contacts.txt", "a");
	if (contacts_file == NULL) {
		printf("Error, contacts_file couldn't be opened.");
		kill(getpid(), SIGTERM);
	}
	
	for(int i = 0; i < close_conts.line; i++)
		fprintf(contacts_file, "%.12llX\n", close_conts.contacts[i]);
	fprintf(contacts_file, "______\n");
	
	fclose(contacts_file);
	
}

