// Profeanu Ioana, 333CA
// main source file
#include "perfect_powers.h"

int main(int argc, char *argv[])
{
	// check if enough arguments were given when running
	if (argc != 4)
	{
		cout << "Please provide correct arguments:  ";
		cout << "<number_of_mappers> <number_of_reducers> <input_file>\n";
		return -1;
	}

	// get the name of the input file and the number of mappers and reducers
	string input_file(argv[3]);
	int number_of_mappers = atoi(argv[1]);
	int number_of_reducers = atoi(argv[2]);

	queue<string> files_to_process_queue = read_files_to_process(input_file);
	unordered_map<int, vector<unordered_set<int>>> mappers_map_of_lists;

	// initialise the mapper threads and arguments
	pthread_t *mapper_threads = (pthread_t*)malloc
			(number_of_mappers * sizeof(pthread_t));
	struct info_for_mappers *mappers_arguments = (struct info_for_mappers*)
			malloc(number_of_mappers * sizeof(struct info_for_mappers));
	// create the common barrier for the mappers and reducers
	pthread_barrier_t barrier_common;
	pthread_barrier_init(&barrier_common, NULL,
		number_of_mappers + number_of_reducers);
	// create mutex for reading from a file
	pthread_mutex_t mutex_mappers_get_file;
	pthread_mutex_init(&mutex_mappers_get_file, NULL);
	// create mutex for writing in the map which contains
	// the lists parsed by all the mappers threads
	pthread_mutex_t mutex_mappers_write_map;
	pthread_mutex_init(&mutex_mappers_write_map, NULL);
	
	// initialise the reducer threads and arguments
	pthread_t *reducer_threads = (pthread_t *)malloc
			(number_of_reducers * sizeof(pthread_t));
	struct info_for_reducers *reducer_arguments = (struct info_for_reducers *)
		malloc(number_of_reducers * sizeof(struct info_for_reducers));
	// initialise the barrier and mutex for the reducers
	pthread_barrier_t barrier_reducers;
	pthread_barrier_init(&barrier_reducers, NULL, number_of_reducers);

	// start mappers and reducers threads
	for (int i = 0; i < number_of_mappers + number_of_reducers; i++) {
		if (i < number_of_mappers) {
			// initialize the mapper arguments
			mappers_arguments[i].id = i;
			mappers_arguments[i].number_of_reducers = 
									number_of_reducers;
			mappers_arguments[i].mutex_mappers_get_file = 
									&mutex_mappers_get_file;
			mappers_arguments[i].mutex_mappers_write_map = 
									&mutex_mappers_write_map;
			mappers_arguments[i].barrier_common = &barrier_common;
			mappers_arguments[i].to_process_files_queue = 
									&files_to_process_queue;
			mappers_arguments[i].map_of_lists = &mappers_map_of_lists;
			// create thread
			int thread = pthread_create(&mapper_threads[i], NULL,
							mappers_function, &mappers_arguments[i]);

			if (thread) {
				printf("Error creating thread %d\n", i);
				exit(-1);
			}

		} else {
			// initialize the reducer arguments
			reducer_arguments[i - number_of_mappers].id =
									i - number_of_mappers;
			reducer_arguments[i - number_of_mappers].number_of_mappers =
									number_of_mappers;
			reducer_arguments[i - number_of_mappers].barrier_common =
									&barrier_common;
			reducer_arguments[i - number_of_mappers].barrier_reducers =
									&barrier_reducers;
			reducer_arguments[i - number_of_mappers].map_of_lists =
									&mappers_map_of_lists;
			// create thread
			int thread = pthread_create
					(&reducer_threads[i - number_of_mappers],
					NULL, reducers_function,
					&reducer_arguments[i - number_of_mappers]);

			if (thread) {
				printf("Error creating thread %d\n", i - number_of_mappers);
				exit(-1);
			}
		}
	}

	// join the mappers and reducers threads
	for (int i = 0; i < number_of_mappers + number_of_reducers; i++) {
		if (i < number_of_mappers) {
			void *status;
			int thread = pthread_join(mapper_threads[i], &status);
			if (thread) {
				printf("Error waiting for thread %d\n", i);
				exit(-1);
			}

		} else {
			void *status;
			int thread = pthread_join(reducer_threads[i - number_of_mappers],
									&status);
			if (thread) {
				printf("Error waiting for thread %d\n", i - number_of_mappers);
				exit(-1);
			}
		}
	}

	// destroy mutexes and barriers and free data
	pthread_barrier_destroy(&barrier_common);
	pthread_mutex_destroy(&mutex_mappers_get_file);
	pthread_mutex_destroy(&mutex_mappers_write_map);
	pthread_barrier_destroy(&barrier_reducers);
	free(mapper_threads);
	free(mappers_arguments);
	free(reducer_threads);
	free(reducer_arguments);

	return 0;
}
