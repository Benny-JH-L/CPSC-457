

./fatsim < tests/fat1.txt
FAT has 7 entries.
blocks: 3 6
elapsed time: 0.00s


./fatsim < tests/fat2.txt
FAT has 11 entries.
blocks: 0 3 9 10
elapsed time: 0.00s


./fatsim < tests/fat3.txt
FAT has 20 entries.
blocks: 2 3
elapsed time: 0.00s


./fatsim < tests/fat4.txt
FAT has 12 entries.
blocks: 0 1 2 3 4 5 6 7 8 9 10 11
elapsed time: 0.00s


./fatsim < tests/fat5.txt
FAT has 16 entries.
blocks:
elapsed time: 0.00s


./fatsim < tests/fat6.txt
FAT has 16 entries.
blocks:
elapsed time: 0.00s


./fatsim < tests/fat7.txt
FAT has 100 entries.
blocks: 6 19 23 26 70 77 81 94
elapsed time: 0.00s


./fatsim < tests/fat8.txt
FAT has 306 entries.
blocks: 219 221 253
elapsed time: 0.00s


./fatsim < tests/fat9.txt
FAT has 32 entries.
blocks: 15 16 19 24
elapsed time: 0.00s


./fatsim < tests/fat10.txt
FAT has 45001 entries.
blocks:
elapsed time: 0.0s


./fatsim < tests/fat11.txt
FAT has 45001 entries.
blocks: 14602
elapsed time: 0.0s


--Examples of medium sized inputs:
./utils/rngfat.py 40000 19 | ./fatsim
FAT has 40000 entries.
blocks: 20800 31675
elapsed time: 0.0s

./utils/rngfat.py 40000 54 | ./fatsim
FAT has 40000 entries.
blocks: 4392
elapsed time: 0.0s

./utils/rngfat.py 100000 76 | ./fatsim
FAT has 100000 entries.
blocks: 5844
elapsed time: 0.01s

--A very large input example:

./utils/rngfat.py 10000000 31 | ./fatsim
FAT has 10000000 entries.
blocks: 2537445 9902857
elapsed time: 1.13s
me: ~6.75

./utils/rngfat.py 9999999 764 | ./fatsim
FAT has 9999999 entries.
blocks: 8173155
elapsed time: 1.14s
me: ~6.75s

-- Python

python utils/fatsim.py < tests/fat5.txt

python utils/fatsim.py < tests/fat6.txt


--Examples of medium sized inputs:

./utils/rngfat.py 40000 19 | ./utils/fatsim.py
FAT has 40000 entries.
blocks: 20800 31675
elapsed time: 0.25s

./utils/rngfat.py 40000 54 | ./utils/fatsim.py
FAT has 40000 entries.
blocks: 4392
elapsed time: 161.59s


./fatsim < tests/9999999_764.txt (Note equivalent to: ./utils/rngfat.py 9999999 764 | ./fatsim)
FAT has 9999999 entries.
blocks: 8173155
elapsed time: 1.14s
me: ~6.75s
