# POSIX_Threads
Demonstrating the usage of mutexes and threads.
Program description:

A multithreaded program pct which is designed to manage in-memory table ofcharacters.
Program is called with 3 arguments n, m, both in range [1, 100] and k inrange [3, 100].
he program creates 2-dimensional array filled with random lowercase characters (a-z) with n rows and m columns.
Each row is protected by a separate mutex.
The program creates a k worker threads waiting for signals.
The main thread reads commands from the standard input. There are three commands supported: print, replace and exit.
When user enters print, the main thread prints the table contents.
When user enters replace n (i.e. replace 3), the main thread sends SIGUSR1 signal to the process itself n times. The main thread blocks this signal. When worker thread receives SIGUSR1 it shall pick a random character from the whole array and replace it with a new random character printing an informative message.
When user enters  exit, the main thread sends SIGINT signal to all worker threads separately. In response worker threads end, main thread joins them and the program exits cleanly.
Every access to the row data has to lock corresponding mutex.
Each thread shall safely generate a different pseudo-random sequence.
