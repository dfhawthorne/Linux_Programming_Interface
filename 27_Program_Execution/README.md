# 27: Program Execution

## Exerecise 27-1

Run:

```bash
make test_1
```

Sample output is:

```text
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 t_execlp.c error_functions.c get_num.c -o t_execlp
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 dir1/xyz.c -o dir1/xyz
cc -std=c17 -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -O2 dir2/xyz.c -o dir2/xyz
./Ex_1.sh
/usr/local/bin:/usr/bin:/bin:./dir1:./dir2
dir1:
total 20
-rwxrwxr-x 1 douglas douglas 15968 Jul 21 08:36 xyz
-rw-rw-r-- 1 douglas douglas   124 Jul 21 08:31 xyz.c

dir2:
total 20
-rwxrwxr-x 1 douglas douglas 15968 Jul 21 08:36 xyz
-rw-rw-r-- 1 douglas douglas   124 Jul 21 08:31 xyz.c
Progam xyz executed from dir1/
```
