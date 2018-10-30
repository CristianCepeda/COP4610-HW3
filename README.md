# COP4610-HW3



## Developers
- Marcial Cabrera -- mcabr122@fiu.edu
- Cristian Cepeda -- ccepe014@fiu.edu

## Compile

Option 1
```
> gcc -c -fpic mem.c
> gcc -shared -o libmem.so mem.o
> gcc testmem.c -lmem -L. -o myprogram
> setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:.
```
Option 2
```
> make
```
## Run

Step 1
```
Before you run "./myprogram"
​You will need to set the environment variable,
LD_LIBRARY_PATH, so that the system can find your library at run-time. Assuming you always
run myprogram from this same directory, you can use the command:

setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:.
```

Step 2
```
./myprogram 
```
