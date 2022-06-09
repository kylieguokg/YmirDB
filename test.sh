#! /usr/bin/env sh

echo "######################"
echo "### Running tests! ###"
echo "######################"

count=0 # number of test cases run so far

clang -g -std=c11 -Wall -Werror ymirdb.c -o ymirdb
# gcc -fprofile-arcs -ftest-coverage ymirdb.c -o ymirdb
# clang -o ymirdb ymirdb.c -std=c11 -Wall -Werror -fsanitize=address,leak -g -O0 

# valgrind -q --leak-check=full --show-leak-kinds=all --trace-children=yes ./ymirdb

for test in tests/*.in; do
	name=$(basename $test .in)
	echo $name
	expected=tests/$name.out
	valgrind -q --leak-check=full --show-leak-kinds=all --trace-children=yes ./ymirdb < $test | diff - $expected || echo "Test $name: failed!\n"
	count=$((count+1))
done


# gcov ymirdb.c

echo "Finished running $count tests!"



