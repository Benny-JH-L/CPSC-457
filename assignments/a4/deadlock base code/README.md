**WARNING:** Do not upload any files in this repository to public websites. If you want to clone this repository, please make sure to keep it private.

# deadlock detection - skeleton for Assignment 4

To compile all code, type:
```
$ make
```

To run the resulting code on file test1.txt:
```
$ ./deadlock < test1.txt
```

## IMPORTANT

Only modify and submit the `find_deadlock.cpp` file! Your code will
be marked with different versions of the other files (such as main.cpp) to
compile and test your code.

## Test files

These are the correct results for the test files included in this repo.

| filename   | correct `index` | correct `procs` | hash-table timings     | optimized timings     |
| :---------- | :-------------: | :-----------: | :-----------------: | :--------------: |
| test1.txt | -1            | []          | 0.0s           | 0.0s        |
| test2a.txt| 6             | [4,5,7]     | 0.0s           | 0.0s        |
| test2b.txt| 5             | [5,7]       | 0.0s           | 0.0s        |
| test3a.txt| 3             | [p7,p3]     | 0.0s           | 0.0s        |
| test3b.txt| -1            | []          | 0.0s           | 0.0s        |
| test4.txt | 3             | [12,7]      | 0.0s           | 0.0s        |
| test5.txt | 6             | [2,77]      | 0.0s           | 0.0s        |
| test6.txt | 9903          | [ab,cd,ef]  | 8.0s            | 0.64s        |
| test7.txt | 29941         | [is,this,answer,the,correct]  | 122.2s    | 6.63s        |



