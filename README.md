<!-- omit in toc -->
# Project

Now that you've familiarized yourself with the Linux environment throughout the semester, you will have the opportunity to build your own simple shell! Don't fret - when we say simple, we mean simple. The shell specified in this assignment is merely capable of changing directories and executing system programs such as `pwd` and `ls`. The goal of this project is to familiarize you with system-related library functions and to give you the pride of knowing that something you've been working with all semester is something that you could have built all along.

In this project, you will

1. Become more familiar with how the OS provides **process control** services that can be invoked by a user application. Specifically, your shell will use the three system calls [fork](https://man7.org/linux/man-pages/man2/fork.2.html), [exec](https://man7.org/linux/man-pages/man3/exec.3.html), and [wait](https://man7.org/linux/man-pages/man2/wait.2.html).
2. Become familiar with how a program can access system **environment variables** using the [getenv](https://man7.org/linux/man-pages/man3/getenv.3.html) standard library function. In particular, we'll access the `PATH` environment variable to determine if a program (i.e., executable object file) is located in a directory (defined in the PATH) on the file system.

<details open>
    <summary>Contents</summary>

- [Pre-lab knowledge](#pre-lab-knowledge)
    - [Background reading](#background-reading)
    - [Structure](#structure)
    - [Command representation](#command-representation)
    - [Running](#running)
    - [Testing](#testing)
- [Part 0: Setup](#part-0-setup)
- [Part 1: Allocating and freeing memory](#part-1-allocating-and-freeing-memory)
- [Part 2: Parsing](#part-2-parsing)
- [Part 3: Find full path](#part-3-find-full-path)
- [Part 4: Executing](#part-4-executing)
    - [Testing](#testing-1)
- [Part 5: Memory leaks](#part-5-memory-leaks)
- [Submit your assignment](#submit-your-assignment)

</details>

## Pre-lab knowledge

### Background reading

* Process control
    * [fork](https://man7.org/linux/man-pages/man2/fork.2.html) man page
    * [exec](https://man7.org/linux/man-pages/man3/exec.3.html) man page
    * [wait](https://linux.die.net/man/2/waitpid) man page
    * [Process API (fork, exec, and wait)](https://uncch.instructure.com/users/9947/files/5935051?verifier=M06tlEP40KumzfyUerV3IWr45LYRNlVSqEXielyw&wrap=1) lecture slides
    * [Process API](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf) OSTEP textbook
* Environment variables
    * [getenv](https://man7.org/linux/man-pages/man3/getenv.3.html) man page
* C
    * [Arrays and Strings](https://uncch.instructure.com/users/9947/files/6519658?verifier=M0hh4K5swK8NjlG2Q16PYnZT1wjAGIQip1QKAfCf&wrap=1) lecture slides
    * [Pointers](https://uncch.instructure.com/users/9947/files/6556130?verifier=RyBHGAvl1trUgASDcCpkt0RwKvxSnCxkC93ETtid&wrap=1) lecture slides
    * [Memory Allocation](https://uncch.instructure.com/users/9947/files/6650292?verifier=Ck555bz4rVkIUYSwscINfbZEJulLQL2N0gV09VUc&wrap=1) lecture slides
* Valgrind
    * [Valgrind Quick Start](https://valgrind.org/docs/manual/quick-start.html)

### Structure

Similar to previous labs, you will implement a set of functions to be called by an external program. Here are the files and what you are expected to do (if anything) in each:

```text
.
├── Makefile
├── README.md
├── main.c - Do not modify. Please read - only a few simple lines of code that will show you how your functions will be called.
├── shell.c - Implement the functions in this file.
├── shell.h - Please read carefully. Contains the definition of command_t, function prototypes and docstrings, and useful constants.
└── tests - Test inputs and outputs.
```

There are several docstrings in `shell.h` that will guide you through function implementation. Please read carefully.

Finally, you **may not** add or remove global variables, alter the given function signatures, or add additional header files. If you have any questions regarding this stipulation, ask your cohort leader for clarification.

### Command representation

Shell commands are comprised of arguments. For example, the command `ls -la` has two space-separated arguments `ls` and `-la`. In `shell.h`, you'll find the definition of our `command_t` struct:

```c
typedef struct {
    int argc;
    char** argv;
} command_t;
```

This representation should be self-explanatory, and the members are similar to `int argc` and `char** argv` of the `main` function in C.

### Running

As in previous labs, a [Makefile](Makefile) is provided. Run `make all` to compile and `./main` to run.

Without any changes, `./main` works like so:

```text
learncli$ ./main
thsh$ ls  # nothing prints because nothing is implemented yet
thsh$ 
```

Notice that the shell prompt is `thsh$ ` to indicate that we are within the COMP 211 Tar Heel SHell.

### Testing

As you write your code, you can add print statements (which will show up when you run `./main`) to test your code. We strongly recommend testing incrementally and often, as opposed to testing only when you think you're done with everything.

Our Gradescope tests for Parts 1-3 of this lab unit test the functions in `shell.c`, so you can submit to Gradescope after completing each of these parts to get feedback before continuing on. Parts 4 and 5 are tested simply by running the `main` executable on various inputs, some of which are provided in [tests/](tests).

## Part 0: Setup

Please read through `main.c` (roughly 20 LoC) to see how your functions will be called and what they should do.

Then, read through the function prototypes in `shell.h`. Then, read the docstrings for the functions, and you may skip implementation details in the docstrings for now.

## Part 1: Allocating and freeing memory

In `main.c`, after getting user input, `parse` is called. `parse` allocates heap memory for `p_cmd->argv` using `alloc_mem_for_argv`. After the command is executed, `cleanup` is called to free the memory.

In `shell.c`, implement `alloc_mem_for_argv` and `cleanup`. Docstrings with instructions are provided in `shell.h`.

Note that `p_cmd->argv` is a `char**` (array of strings). To review how to allocate memory for a `char**`, you may look at the final slide of the Memory Allocation slides in [Background reading](#background-reading).

You should submit to Gradescope after implementing `alloc_mem_for_argv` and `cleanup` to confirm that they are correct. If these two functions are not implemented properly, you may get a "autograder failed to execute" message. The Gradescope tests do not explicitly test `cleanup` in this part, but `cleanup` will be tested later when we check for memory leaks in the entire program.

## Part 2: Parsing

When `main.c` gets the user's string input, it calls `parse` to parse the string into a `command_t` struct. Implement this functionality in the `parse` function. A docstring is provided in `shell.h`.

In particular, read example 4 in the docstring carefully. In our shell's grammar, extra whitespace is insignificant, so `ls -la` and `   ls   -la   ` should result in `command_t` structs with identical values. Note the additional whitespace in the latter.

## Part 3: Find full path

If a command is not built-in to the shell (e.g., `cd` or `exit`), its binary lives somewhere on the file system, and that is the external program that runs when the user types the command. For example,

```text
learncli$ ls
project-template
learncli$ which ls  # find full path of ls command
/usr/bin/ls
learncli$ /usr/bin/ls  # same output as ls because this is the program that runs when you type ls
project-template
```

External commands are simply executable programs on the file system, and you can find out where with `which`. If you `ls /usr/bin`, you'll see many of the programs you have been using this semester, such as `ls`, `clang-format`, `gcc`, etc.

After we have parsed the user's input, the `find_full_path` function will attempt to find the full path of their command to determine which program to run, if it exists, during execution.

Implement `find_full_path`, and details are given in its docstring in `shell.h`.

## Part 4: Executing

Finally, the command is ready to be executed. Implement `execute`, and details are in its docstring in `shell.h`.

You will have to use `fork`, a variant of `exec`, and wait. Refer to Process API content in [Background reading](#background-reading) for details.

### Testing

Run `make all` to compile and `./main` to run.

For example,

```text
learncli$ ./main
thsh$ echo hello
hello
thsh$ exit
```

Similar to previous labs, more test inputs and outputs are in [tests/](tests). You can check for differences with `diff`. For example, `./main < tests/in1.txt > out1.txt` and `diff tests/out1.txt out1.txt`.

## Part 5: Memory leaks

Lastly, we'll check our shell for memory leaks using `valgrind`, which checks for memory errors. For example,

```text
learncli$ valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt ./main < tests/in1.txt > /dev/null
learncli$ grep "definitely lost: [0-9,]\+ bytes in [0-9,]\+ blocks" valgrind-out.txt
==14598==    definitely lost: 0 bytes in 0 blocks
```

In this command, `valgrind` inspects the `main` executable (when run with the contents of `tests/in1.txt` redirected to stdin, and `main`'s stdout is redirected to `/dev/null` since we don't care about its output in this part) and outputs statistics to `valgrind-out.txt`. The only statistic we care about in this part is the number of bytes "definitely lost", indicating definite memory leaks, as extracted by `grep`.

The autograder will run this command on your program for all the test inputs in [tests/](tests). We expect to see the string "definitely lost: 0 bytes in 0 blocks" for those inputs. As long as you `free` everything you `malloc`, this will be the case.

If you have memory leaks, make sure you `free` everything you `malloc` in your code. Fortunately, `valgrind` tells us where errors occur (if we compile with debugging information via `-g`, like in our Makefile). For example, if we purposely introduce a memory leak into our program, `make` (remember to re-run `make` before running `valgrind`!), and re-run the `valgrind` command above, we would see something like this in `valgrind-out.txt`:

```text
==62429== 138 bytes in 4 blocks are definitely lost in loss record 3 of 3
==62429==    at 0x4865058: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==62429==    by 0x1092E7: find_full_path (shell.c:72)
==62429==    by 0x109437: execute (shell.c:99)
==62429==    by 0x1096D3: main (main.c:22)
```

This would tell us that a memory leak occurs in `find_full_path`.

If you somehow see no memory leaks for all inputs in [tests/](tests) but you do not pass the autograder test, contact your cohort leader.

## Submit your assignment

1. Use git to push your finished code to this GitHub repository.
2. Go to the COMP 211 course in Gradescope and click on the assignment called **Project**.
3. Click on the option to **Submit Assignment** and choose GitHub as the submission method.
4. You should see a list of your public repositories. Select the one named **project-yourname** and submit it.
5. Your assignment should be autograded within a few seconds and you will receive feedback for the autograded portion.
6. If you receive all the points, then you have completed this project! Otherwise, you are free to keep pushing commits to your GitHub repository and submit for regrading up until the deadline of the project.
