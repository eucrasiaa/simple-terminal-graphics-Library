## Shared memory notes
this shit is scary i think im sure valgrind and address sanitizer be nightmarish 
### Will need:
- figure out locking mechanisms?
    - prob will only be 1 way, and use locks + flags to indicate whats there and when to read
    - maybe should ALSO use the like socket to pass info on reading?
        - troublesome as is, so idk if i want MORE overhead (or would it reduce?)
- setup a few structures and define flags to identify
- should the debugger be live or based on commands?
    - live could be init'd on declaring a screenbuffer, and passing that buffer.
    - every call to draw doeos contain a window deep copy anyways so could add wrapper functions that additionally write into 


use #include <pthread.h> use mutex, never release until reading is ready?
try to aquire and itll end up storing copies of all the variables
on every variable or modification, write, give up mutex, then on reaquire (meaning the watcher finished copying), resume code



