---
title: Permissions
...

Creating, renaming, removing, opening, reading from, writing to, and executing files can only be done the operating system.
As such, the OS gets to decide when to approve a request to do one of these things and when to reject it.
Although not all OSs use the same system, the POSIX standard defines a set of file permissions that are commonly used.


<div class="exercise long">
Work with a partner to achieve the following:

1. If the TA gives
    - a numeric permission, convert to letters
    - a letter permission, convert to a number
1. Create a directory `dir` with a file `foo` and a file `baz` where you can `ls` and `cat` freely, but your partner finds that
    a. `ls dir`{.bash} refuses to run
    a. `cat dir/foo`{.bash} works
    a. `cat dir/baz`{.bash} refuses to run
1. Create a file and set its access control list so that
    a. you can read and write it
    a. your partner can read it
    a. other users (including TAs) cannot do either one
1. Make a file `simple.sh` which can be run both by `bash simple.sh`{.bash} and `./simple.sh`{.bash} containing bash commands that
    a. appends a line containing the current day and time to file `simple_runs`
    a. reports how many lines `simple_runs` contains
1. Determine which of the following are possible to achieve:
    a. `cat xyxxy`{.bash} works but `echo 'hi' >> xyxxy`{.bash} does not
    a. `echo hi >> xyxxy`{.bash} works but `cat xyxxy`{.bash} does not
    a. `./xyxxy`{.bash} works but `cat xyxxy`{.bash} does not
    a. `./xyxxy`{.bash} works but `echo hi >> xyxxy`{.bash} does not

</div>


The information needed to achieve these goals is explained below.
We recommend you read it in full, discussing it with a partner and asking clarifying questions of TAs as you go, then return to the tasks above.

# Accounts

Every program runs as some **user**.
This is true even of the user interface, a program that allows you to run other programs.
When one program runs another, the new program runs as the same user as the old except in a few special circumstances.
Any example special circumstance is the "log in" program, which itself runs as `root` but after you log in runs your user interface as your user account.

You can find out what user you are running as via the command `whoami`{.bash}.

Every user can belong to any number of **groups**.
You can find out what groups a user belongs to by running the command `groups mst3k`{.bash};
if you omit the user ID, it will list your own group memberships.

Every file and directory also has a user and a group---but just one group, not a whole set.
The user of a file does not need to belong to the group of the file.

# File permissions

Permissions are split into three kinds, with different meanings for :

r
:   for files
    :   permission to read its contents (e.g., `cat file` works)
    
    for directories
    :   permission to list its contents (e.g., `ls directory/` works)

w
:   for files
    :   permission to write its contents (e.g., `echo whatnot >> file` works)
    
    for directories
    :   permission to add and remove its contents (e.g., `rm directory/whatever` works)

x
:   for files
    :   permission to execute its contents (e.g., `./file` works)
    
    for directories
    :   permission to access its contents (e.g., `directory/whatever` works)

Every file has these permissions for 3 classes of users, stored as a nine-bit number; from high- to low-order, those bits are

u
:   owning user may read/write/execute

g
:   members of owning group may read/write/execute

o
:   others may read/write/execute

`ls -l`{.bash} includes the nine permission bits in letter form: a subset of rwxrwxrwx (e.g., `rw-rw----` means the owner and all in the owning group have r and w permission, but not x permission; and other users have no permissions).
Commands that change these often use octal (base-8) representation of the number (e.g. 660 for `rw-rw----`).


<div class="aside long">
Using `chmod`

The most commonly used permission tool is `chmod`, used as `chmod [permission] file`.
As noted in `man chmod`{.bash}, the permission argument can be either letters or octal

- `chmod 644 file` means "change the permission bits to exactly 644~8~ = 110100100~2~ = rw-r--r--
- `chmod ug+rw file` means "add r and w permissions for the owning **u**sers and the owning **g**roup"

</div>


## Defaults: the `umask`

When you make a new file, its gets all permissions except a few; the omitted few are specified by the `umask`.
A special command, `umask`, can display (and change) these, either in octal or letter format.


<div class="example long">
On the department machines, if you run `umask` you'll see `0022`.
That means that new files get permission `0755`: a nine-bit `~0022`.
What that actually looks like can be see by adding the symbolic flag: `umask -S` shows `u=rwx,g=rx,o=rx`.

</div>



## setuid, setgid, and sticky

File permissions in POSIX-compatible systems also have three more permission bits which we **recommend not using** because of their potential dangers.

setuid
:   executable file
    :   If executable `xyxxy` is owned `mst3k` and has `setuid` permissions,
        then when `tj1a` runs `./xyxxy`, the program runs as `mst3k`, not as `tj1a`.
        This can be a security vulnerability and **should not be used** except when absolutely necessary.
    
    non-executable file
    :   does nothing
    
    directory
    :   does nothing on most POSIX systems

setgid
:   executable file
    :   like setuid, but with group, not user.
        This can be a security vulnerability and **should not be used** except when absolutely necessary.

    non-executable file
    :   does nothing
    
    directory
    :   when a new file is created in the directory, it inherits the directory's group instead of the creating user's group.
        This can be a security vulnerability and **should not be used** unless you are sure you know what you are doing.

sticky
:   file
    :   does nothing in most POSIX systems
    
    directory
    :   the files inside it can only be removed or renamed by the owner of the file, owner of the directory (but `root` is always allowed).

If you use a four-digital octal with `chmod`, these permissions will be the first octal digit.
Sticky may sometimes be worth setting; we recommend never using the other two.

# Access Control Lists

Sometimes we need more fine-grained control than the three-way user-group-other distinction.
This is provided using access control lists, or ACLs; they are a list of user+permission pairs associated each file.
Every OS I've ever used has ACL support, though actual use of them is relatively uncommon; the standard system works in most cases.

The command `getfacl file`{.bash} shows the full permission information for a file.
The command `setfacl` creates and modifies entries in the permissions list.
There are many ways of doing this, as `man setfacl` shows, but three examples will suffice for most common cases:

- `setfacl -m u:mst3k:rx` this `m`odifies `u`ser `mst3k`'s permissions to be `r` and `x`
- `setfacl -m g:lab_fall2019:r` this `m`odifies `g`roup `lab_fall2019`'s permissions to be `r` only (no `x` or `w` permission)
- `setfacl -x u:mst3k` this removes the `u`ser `mst3k`'s entry from the permission list, so `mst3k` will be handled like any other user.

While ACLs are not commonly needed, they are a safe way of handling cases where they are needed.

# Executable text files

What does it mean for a file to be executable?
On POSIX, there are two main cases where this is sensible:

- If the file is an executable binary (typically an ELF executable), then being executable means the loader will agree to give it memory and jump to its starting code.

- If the file is a plain text file, then being executable means that "check for a shebang^["shebang" comes from the colloqial name of the first two characters: `#` is "hash" and `!` is "bang". "Hashbang" was shortened to "shbang", with a vowel added for readability. You can still sometimes hear "hashbang" used to mean the `#!` itself, as in "a shebang begins with a hashbang, then path to the interpreter".] line"
    
    A "shebang" is a line of the form `#!/path/to/program` (or sometimes `#!/usr/bin/env program`).
    One common shebang is `#!/bin/bash`.
    If a file `gloop.sh` begins `#!/bin/bash` and is executable, then running `./gloop.sh` is equivalent to running `/bin/bash ./gloop.sh`


<div class="example long">
A **quine** is a program that, when run, displays its own contents.
The following file is a quine, provided it is executable:

    #!/bin/cat

So is the following:

    #!/bin/cat
    Quines are interesting to create in many languages.
    But if you use shebangs, they are trivial to create
    using the interpreter "cat".


</div>

