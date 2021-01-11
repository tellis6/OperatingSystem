Prj2: Exercises in Concurrency and Synchronization

- Harshil Kotamreddy, Tim Ellis, Lynne Tien

Objectives
Learn to:
- Solve inter-process communication problems during concurrent execution of processes.
- Use Posix Pthread library for concurrency.

Instructions and Summary:

- First run Makefile (type 'make' into terminal)

1. Bounded-buffer Producer/Consumer problem:
This program implements a solution to the boundedbuffer producer/consumer problem. The solution uses synchronization primitives (pthread_mutex_t for mutual exclusion and sem_t for minding the buffer size) provided by the POSIX threads. The implementation consists of a control program that (i) initializes the buffer and the synchronization variables and (ii) creates and terminates the threads for the producer and the consumer. The producer generates printable characters and places them into the buffer and the consumer pulls the characters out of the buffer one at a time and prints them out.

Instructions:
in terminal
type './procon' into terminal and press <enter>.

** the program runs for infinity so user needs to press 'cntrl c' to exit. **

2. Mother Hubbard (children, mother, father)
This program simulates days in the life of Mother Hubbard. There are 12 children, and each child needs to be woken up, fed breakfast, sent to school, given dinner, given a bath, read a book, and tucked in bed. The mother applies each task in order, to each child in order, before being able to move on to the next task. Then after all the children have all the tasks done to them, the mother can take a nap break. After a child takes their bath, the father can read the child a book and tuck the child in bed. When all children are in bed, then the father can go to sleep. When the father goes to sleep, he wakes up the mother and the cycle repeats.

Instructions:
in terminal
type 'mh <number of days>' into terminal and press <enter>.
    i.e. './mh 2' will run for 2 cycles/days

3. Airline Passengers
This program simulates an airport terminal where passengers wait to board a flight. In the airport there are P passengers, B baggage handlers, S security screeners, and F flight attendants. Each passenger must give their baggage to a baggage handler, then go have their “security” screened by a security screener, and finally be seated by a flight attendant. When all P passengers have this done, the plane takes off. All P passengers arrive at the same time to the terminal and must wait on a line to be processed, but are processed in no particular order.

Instructions:
in terminal
type ./airline <number of passengers> <number of baggage handlers> <number of security screeners> <number of flight attendants>' and press <enter>. 
    i.e. './airline 100 3 5 2' will launch the program with 100 passengers, 3 baggage handlers, 5 security screeners, 2 flight attendants.