#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem);

int mssleep(long miliseconds) {
   struct timespec rem;
   struct timespec req= {
       (int)(miliseconds / 1000),     /* secs (Must be Non-Negative) */ 
       (miliseconds % 1000) * 1000000 /* nano (Must be in range of 0 to 999999999) */ 
   };
   return nanosleep(&req , &rem);
}
