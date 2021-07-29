#include <time.h>
#include <sys/resource.h>
#include "covidTrace.h"

//Binary file to keep the timestamps
FILE* file;

int main() {
	//Get program starting time
	struct timeval start_time;
	gettimeofday(&start_time, NULL);
	
	//Delete existing close contacts file
	if (remove("contacts.txt"))
		printf("Error deleting close contacts file");
	
	//Open file and write start_time in binary
	file = fopen("timestamps.dat", "wb");
	if (file == NULL){
		printf("Error, couldn't open timestamps file\n");
		exit(1);
	}
	print_time_in_file(start_time);
	//printf("Start time:\n");
	//printf("%lld %lld\n", start_time.tv_sec, start_time.tv_usec);
	//printf("10 second handler times:\n");

	//srand for BTnearMe
	//srand(time(0));
	
	//Create interval timer
	struct itimerval timer_interv;
	
	//virtual time should be every 10 seconds
	uint32_t secs = 10 / ACCEL_COEF; //akeraio meros
	uint32_t usecs = 1000000/ACCEL_COEF * 10;
	
	timer_interv.it_interval.tv_sec = secs;	/* Interval for periodic timer */
	timer_interv.it_interval.tv_usec = usecs;
	timer_interv.it_value.tv_sec = secs;	/* Time until next expiration */
	timer_interv.it_value.tv_usec = usecs;

	setitimer(ITIMER_REAL, &timer_interv, NULL);
	
	// Register 10 seconds signal handler
	signal(SIGALRM, handler_10_seconds); 
	
	//Register SIGTERM and SIGINT handler
	signal(SIGTERM, exit_handler); 
	signal(SIGINT, exit_handler); 
	
	//Idle time, waiting for interrupts
	while(1)
		sleep(100000);
	
	
	return 0;	
}

void handler_10_seconds(int signum){

	//Get current time and write in bin file
	struct timeval curr_time;
	gettimeofday(&curr_time, NULL);
	print_time_in_file(curr_time);
	//printf("%lld %lld\n", curr_time.tv_sec, curr_time.tv_usec);
	
	//get and register new recent contact
	macaddress mac = BTnearMe();
	
	uint64_t actual_time_sec = ACCEL_COEF * curr_time.tv_sec + (ACCEL_COEF * curr_time.tv_usec) / 1000000;
	
	register_rec_contact(mac, actual_time_sec);

	//check if is and register close contact
	if (is_close_contact(mac, actual_time_sec))
		register_close_contact(mac, actual_time_sec);

	
	//count to 4 hours - time to take a covid test
	static int hours_4_count = 0; //counts 10 second intervals
	static int days_30_count = 0; //counts 4 hour intervals
	
	hours_4_count++;
	if (hours_4_count == (4*60*60/10)){
		if (testCovid())
			uploadContacts(actual_time_sec); //should have &close_conts, close_conts.last
		
		days_30_count++;
		hours_4_count = 0;
	}
	
	//count to 30 days - time to end the program
	if (days_30_count == (30*24/4)){
		//send terminating signal
		kill(getpid(), SIGTERM);
	}
}

void exit_handler(int signum){
	//If it's 30 days or someone closed the program
	
	//Get CPU time and write in file
	struct rusage rus;
	getrusage(RUSAGE_SELF, &rus);
	print_time_in_file(rus.ru_utime); //maybe TODO ru time+su time
	print_time_in_file(rus.ru_stime);
	//printf("CPU time:\n");
	//printf("%lld %lld\n", rus.ru_utime.tv_sec, rus.ru_utime.tv_usec);
	
	//add cpu time calculations with ticks maybe TODO
	
	//Close the file
	fclose(file);
	
	exit(0);
}

void print_time_in_file(struct timeval time){
	fwrite(&(time.tv_sec), sizeof(time.tv_sec), 1, file);
	fwrite(&(time.tv_usec), sizeof(time.tv_usec), 1, file);
}
