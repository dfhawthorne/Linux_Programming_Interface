# Chapter 19: Monitoring File Events

## Exercise 19-1

To run the full test suite:,
```bash
make
```

This will remove the main program, `mon_dir_evt`, to force a recompilation of the binary. All the test directories, `test_dir` and `test_other`, along with the other artefacts `test.log`, `test.err`, and `test.pid`.

The test suite is driven through a script called `run_test.sh`. The test suite does not restore the test directories to their original states.

### Test Cases

The test cases are stored in `case.dat`. Because of the use of the `shuf` command, each of these test cases should be independent.

### Sample Output

The folowing listing shows sample output for a test run. Because of the `shuf` command, the order may vary with each run:

```text
2023-06-03 00:13:19: 'test_dir/testd/test1/test7.dat' moved in from outside the monitored directories
2023-06-03 00:13:21: 'test_dir/testa/test1/test.dat' moved to 'test_dir/testa/test2/test2.dat'
2023-06-03 00:13:21: 'test_dir/teste/test3/test.dat' deleted
2023-06-03 00:13:21: 'test_dir/teste/test3' deleted
2023-06-03 00:13:24: 'test_dir/testb/test3/test2.dat' created
2023-06-03 00:13:28: 'test_dir/testb/test5' created
2023-06-03 00:13:31: 'test_dir/testa/test3' moved to 'test_dir/testa/test4'
2023-06-03 00:13:40: 'test_dir/teste/test2/test.dat' deleted
2023-06-03 00:13:41: 'test_dir/testc/test1' moved outside of monitored directories.
2023-06-03 00:13:43: 'test_dir/testd/test6' moved in from outside the monitored directories
2023-06-03 00:13:49: 'test_dir/testc/test2/test.dat' moved outside of monitored directories.
```

### Debug Output

To get the debugging output, run `make all-v`.