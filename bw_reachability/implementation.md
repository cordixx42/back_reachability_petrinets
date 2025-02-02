## implementation choices

## should empty state and sigma state be constructed in constructor or in make

### SELF handling
* in make remove SELF and set it to Id
    * makes things easier in union and inter, no need to check self case explicitly
    * but also make has to be adapted for case id:x [x,a] is equivalent to [SELF,a]

### pre using sets instead of vectors
* easier to compare equality and ensure no duplicates 
    * with vector, need to sort and look out for duplicates


### extra case loop in pre => same state 
* assume weakly acyclic input

### create case a*aL(r) and aa*L(r) is equivalent

### if successors and b same, then same language 
* case no self loops, clear 
* case self loop 

