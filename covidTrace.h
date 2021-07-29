#ifndef COVIDTRACE_H
#define COVIDTRACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

#define MAX_CLOSE_CONTACTS 1000
#define ACCEL_COEF 100 //acceleration coefficient TODO reset to 100
#define POSITIVE_PROB_INV 20 //inverse of me ti pithanothta na vgainei thetiko to test

typedef uint64_t macaddress;
typedef uint8_t bool;

struct recent_contacts{
	macaddress contacts[120];
	uint64_t time_sec[120];
	
	//shows where to write
	int current_line;
};

struct close_contacts{
	macaddress contacts[MAX_CLOSE_CONTACTS];
	uint64_t time_sec[MAX_CLOSE_CONTACTS];
	
	//to use when inserting contacts, and when deleting
	//last shows to the first emepty line
	int line;
};

//in main.c
void handler_10_seconds(int signum);
void exit_handler(int signum);
void print_time_in_file(struct timeval time);

//in covidTrace.c
void register_rec_contact(macaddress contact, uint64_t sec);

bool is_close_contact(macaddress contact, uint64_t curr_time_sec);
void delete_close_contact(int which, int how_many);
void register_close_contact(macaddress contact, uint64_t sec);

macaddress BTnearMe();
bool testCovid();
void uploadContacts(uint64_t actual_time_sec);

#endif
