## Build

### C example

```shell
gcc -o entropy entropy.c -lm
chmod a+x entropy
```



### C++ example

```shell
g++ -o entropy entropy.cpp
chmod a+x entropy
```



## Run

1. Generate some random data

   ```shell
   head -c 1M </dev/urandom >test.dat
   ```

2. Run the program

   ```shell
   ./entropy test.dat
   ```

3. By default the program reads files in blocks of 1024 Bytes. This can be changed specifying  the block size

   ```shell
   ./entropy -b 2048 test.dat
   ```


