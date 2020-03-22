MAKEFLAGS += --no-print-directory
CC = gcc -Wall


all: start static shared dynamic clean

start:
	rm -f results3a.txt

static:
	$(CC) -c difflib.c
	ar rcs libdifflib.a difflib.o
	$(CC) tests.c -static -L. -ldifflib -o tests
	@echo "---------------------\nSTATIC" >> results3a.txt
	make test >> results3a.txt

shared:
	$(CC) -c -fPIC difflib.c
	$(CC) -shared -o libdifflib.so difflib.o
	$(CC) tests.c -L. -Wl,-rpath=. -ldifflib -o tests
	@echo "---------------------\nSHARED" >> results3a.txt
	make test >> results3a.txt

dynamic:
	$(CC) -fPIC -c difflib.c
	$(CC) -shared -fPIC -o libdifflib.so difflib.o 
	$(CC) tests_dyn.c -ldl -o tests
	@echo "---------------------\nDYNAMIC" >> results3a.txt
	make test >> results3a.txt


c11 = compare_pairs 5 files/shuge/shuge1 files/shuge/shuge2 files/shuge/shuge1 files/shuge/shuge3 files/shuge/shuge1 files/shuge/shuge4 files/shuge/shuge1 files/shuge/shuge5 files/shuge/shuge1 files/shuge/shuge6

c12 = compare_pairs 15 files/shuge/shuge1 files/shuge/shuge2 files/shuge/shuge1 files/shuge/shuge3 files/shuge/shuge1 files/shuge/shuge4 files/shuge/shuge1 files/shuge/shuge5 files/shuge/shuge1 files/shuge/shuge6 files/shuge/shuge1 files/shuge/shuge7 files/shuge/shuge1 files/shuge/shuge8 files/shuge/shuge1 files/shuge/shuge9  files/shuge/shuge2 files/shuge/shuge3 files/shuge/shuge2 files/shuge/shuge4 files/shuge/shuge2 files/shuge/shuge5 files/shuge/shuge2 files/shuge/shuge6 files/shuge/shuge2 files/shuge/shuge7 files/shuge/shuge2 files/shuge/shuge8 files/shuge/shuge2 files/shuge/shuge9

c13 = compare_pairs 30 files/shuge/shuge1 files/shuge/shuge2 files/shuge/shuge1 files/shuge/shuge3 files/shuge/shuge1 files/shuge/shuge4 files/shuge/shuge1 files/shuge/shuge5 files/shuge/shuge1 files/shuge/shuge6 files/shuge/shuge1 files/shuge/shuge7 files/shuge/shuge1 files/shuge/shuge8 files/shuge/shuge1 files/shuge/shuge9  files/shuge/shuge2 files/shuge/shuge3 files/shuge/shuge2 files/shuge/shuge4 files/shuge/shuge2 files/shuge/shuge5 files/shuge/shuge2 files/shuge/shuge6 files/shuge/shuge2 files/shuge/shuge7 files/shuge/shuge2 files/shuge/shuge8 files/shuge/shuge2 files/shuge/shuge9  files/shuge/shuge3 files/shuge/shuge4 files/shuge/shuge3 files/shuge/shuge5 files/shuge/shuge3 files/shuge/shuge6 files/shuge/shuge3 files/shuge/shuge7 files/shuge/shuge3 files/shuge/shuge8 files/shuge/shuge3 files/shuge/shuge9  files/shuge/shuge4 files/shuge/shuge5 files/shuge/shuge4 files/shuge/shuge6 files/shuge/shuge4 files/shuge/shuge7 files/shuge/shuge4 files/shuge/shuge8 files/shuge/shuge4 files/shuge/shuge9 files/shuge/shuge5 files/shuge/shuge6 files/shuge/shuge5 files/shuge/shuge7 files/shuge/shuge5 files/shuge/shuge8 files/shuge/shuge5 files/shuge/shuge9

c21 = compare_pairs 5 files/smed/smed1 files/smed/smed2 files/smed/smed1 files/smed/smed3 files/smed/smed1 files/smed/smed4 files/smed/smed1 files/smed/smed5 files/smed/smed1 files/smed/smed6

c22 = compare_pairs 15 files/smed/smed1 files/smed/smed2 files/smed/smed1 files/smed/smed3 files/smed/smed1 files/smed/smed4 files/smed/smed1 files/smed/smed5 files/smed/smed1 files/smed/smed6 files/smed/smed1 files/smed/smed7 files/smed/smed1 files/smed/smed8 files/smed/smed1 files/smed/smed9  files/smed/smed2 files/smed/smed3 files/smed/smed2 files/smed/smed4 files/smed/smed2 files/smed/smed5 files/smed/smed2 files/smed/smed6 files/smed/smed2 files/smed/smed7 files/smed/smed2 files/smed/smed8 files/smed/smed2 files/smed/smed9

c23 = compare_pairs 30 files/smed/smed1 files/smed/smed2 files/smed/smed1 files/smed/smed3 files/smed/smed1 files/smed/smed4 files/smed/smed1 files/smed/smed5 files/smed/smed1 files/smed/smed6 files/smed/smed1 files/smed/smed7 files/smed/smed1 files/smed/smed8 files/smed/smed1 files/smed/smed9  files/smed/smed2 files/smed/smed3 files/smed/smed2 files/smed/smed4 files/smed/smed2 files/smed/smed5 files/smed/smed2 files/smed/smed6 files/smed/smed2 files/smed/smed7 files/smed/smed2 files/smed/smed8 files/smed/smed2 files/smed/smed9  files/smed/smed3 files/smed/smed4 files/smed/smed3 files/smed/smed5 files/smed/smed3 files/smed/smed6 files/smed/smed3 files/smed/smed7 files/smed/smed3 files/smed/smed8 files/smed/smed3 files/smed/smed9  files/smed/smed4 files/smed/smed5 files/smed/smed4 files/smed/smed6 files/smed/smed4 files/smed/smed7 files/smed/smed4 files/smed/smed8 files/smed/smed4 files/smed/smed9 files/smed/smed5 files/smed/smed6 files/smed/smed5 files/smed/smed7 files/smed/smed5 files/smed/smed8 files/smed/smed5 files/smed/smed9

c31 = compare_pairs 5 files/slow/slow1 files/slow/slow2 files/slow/slow1 files/slow/slow3 files/slow/slow1 files/slow/slow4 files/slow/slow1 files/slow/slow5 files/slow/slow1 files/slow/slow6

c32 = compare_pairs 15 files/slow/slow1 files/slow/slow2 files/slow/slow1 files/slow/slow3 files/slow/slow1 files/slow/slow4 files/slow/slow1 files/slow/slow5 files/slow/slow1 files/slow/slow6 files/slow/slow1 files/slow/slow7 files/slow/slow1 files/slow/slow8 files/slow/slow1 files/slow/slow9  files/slow/slow2 files/slow/slow3 files/slow/slow2 files/slow/slow4 files/slow/slow2 files/slow/slow5 files/slow/slow2 files/slow/slow6 files/slow/slow2 files/slow/slow7 files/slow/slow2 files/slow/slow8 files/slow/slow2 files/slow/slow9

c33 = compare_pairs 30 files/slow/slow1 files/slow/slow2 files/slow/slow1 files/slow/slow3 files/slow/slow1 files/slow/slow4 files/slow/slow1 files/slow/slow5 files/slow/slow1 files/slow/slow6 files/slow/slow1 files/slow/slow7 files/slow/slow1 files/slow/slow8 files/slow/slow1 files/slow/slow9  files/slow/slow2 files/slow/slow3 files/slow/slow2 files/slow/slow4 files/slow/slow2 files/slow/slow5 files/slow/slow2 files/slow/slow6 files/slow/slow2 files/slow/slow7 files/slow/slow2 files/slow/slow8 files/slow/slow2 files/slow/slow9  files/slow/slow3 files/slow/slow4 files/slow/slow3 files/slow/slow5 files/slow/slow3 files/slow/slow6 files/slow/slow3 files/slow/slow7 files/slow/slow3 files/slow/slow8 files/slow/slow3 files/slow/slow9  files/slow/slow4 files/slow/slow5 files/slow/slow4 files/slow/slow6 files/slow/slow4 files/slow/slow7 files/slow/slow4 files/slow/slow8 files/slow/slow4 files/slow/slow9 files/slow/slow5 files/slow/slow6 files/slow/slow5 files/slow/slow7 files/slow/slow5 files/slow/slow8 files/slow/slow5 files/slow/slow9

r1 = remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0

r2 = remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0

r3 = remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0 remove_block 0

test:
	@echo "compare 5  pairs, sim: huge"
	@./tests create_array 5 $(c11)
	@echo "compare 15 pairs, sim: huge"
	@./tests create_array 15 $(c12)
	@echo "compare 30 pairs, sim: huge"
	@./tests create_array 30 $(c13)
	@echo "compare 5  pairs, sim:  med"
	@./tests create_array 5 $(c21)
	@echo "compare 15 pairs, sim:  med"
	@./tests create_array 15 $(c22)
	@echo "compare 30 pairs, sim:  med"
	@./tests create_array 30 $(c23)
	@echo "compare 5  pairs, sim:  low"
	@./tests create_array 5 $(c31)
	@echo "compare 15 pairs, sim:  low"
	@./tests create_array 15 $(c32)
	@echo "compare 30 pairs, sim:  low"
	@./tests create_array 30 $(c33)
	@echo ""
	@echo "compare and remove  5 blocks, sim: huge"
	@./tests create_array 5 $(c11) $(r1)
	@echo "compare and remove 15 blocks, sim: huge"
	@./tests create_array 15 $(c12) $(r2)
	@echo "compare and remove 30 blocks, sim: huge"
	@./tests create_array 30 $(c13) $(r3)
	@echo "compare and remove x5  5 blocks, sim: huge"
	@./tests create_array 5 $(c11) $(r1) $(c11) $(r1) $(c11) $(r1) $(c11) $(r1) $(c11) $(r1)
	@echo "compare and remove x5 15 blocks, sim: huge"
	@./tests create_array 15 $(c12) $(r2) $(c12) $(r2) $(c12) $(r2) $(c12) $(r2) $(c12) $(r2)
	@echo "compare and remove x5 30 blocks, sim: huge"
	@./tests create_array 30 $(c13) $(r3) $(c13) $(r3) $(c13) $(r3) $(c13) $(r3) $(c13) $(r3)

tests_dyn:
	test:
	@echo "compare 5  pairs, sim: huge"
	@./tests_dyn create_array 5 $(c11)
	@echo "compare 15 pairs, sim: huge"
	@./tests_dyn create_array 15 $(c12)
	@echo "compare 30 pairs, sim: huge"
	@./tests_dyn create_array 30 $(c13)
	@echo "compare 5  pairs, sim:  med"
	@./tests_dyn create_array 5 $(c21)
	@echo "compare 15 pairs, sim:  med"
	@./tests_dyn create_array 15 $(c22)
	@echo "compare 30 pairs, sim:  med"
	@./tests_dyn create_array 30 $(c23)
	@echo "compare 5  pairs, sim:  low"
	@./tests_dyn create_array 5 $(c31)
	@echo "compare 15 pairs, sim:  low"
	@./tests_dyn create_array 15 $(c32)
	@echo "compare 30 pairs, sim:  low"
	@./tests_dyn create_array 30 $(c33)
	@echo ""
	@echo "compare and remove  5 blocks, sim: huge"
	@./tests_dyn create_array 5 $(c11) $(r1)
	@echo "compare and remove 15 blocks, sim: huge"
	@./tests_dyn create_array 15 $(c12) $(r2)
	@echo "compare and remove 30 blocks, sim: huge"
	@./tests_dyn create_array 30 $(c13) $(r3)
	@echo "compare and remove x5  5 blocks, sim: huge"
	@./tests_dyn create_array 5 $(c11) $(r1) $(c11) $(r1) $(c11) $(r1) $(c11) $(r1) $(c11) $(r1)
	@echo "compare and remove x5 15 blocks, sim: huge"
	@./tests_dyn create_array 15 $(c12) $(r2) $(c12) $(r2) $(c12) $(r2) $(c12) $(r2) $(c12) $(r2)
	@echo "compare and remove x5 30 blocks, sim: huge"
	@./tests_dyn create_array 30 $(c13) $(r3) $(c13) $(r3) $(c13) $(r3) $(c13) $(r3) $(c13) $(r3)


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f diffTmpFile.txt
	rm tests