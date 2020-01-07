---
title: User Accounts
...

From the hardware's perspective, permissions are fairly simple:
either the processor is operating in a privileged mode
and can access any memory and all instructions,
or else it is operating in user mode and is limited to unprivileged instructions and memory.

This simple model allows the operating system to implement
a much more involved set of permissions and restrictions.

# Per-process permissions

Suppose you invoke `open("file.txt", O_RDONLY)`{.c} in your code.
This C wrapper function does little more than setting `%rax` to a system call number and then using the assembly operation `syscall` to trigger the exception handler for system calls.
The handler, which is part of the operating system and is running in privileged mode, then has to decide if it wants to open that file for your program or not.

The operating system could do anything to make that determination.
It could say that you can only open files with names ending with a `t` if there are an even number of files on the disk or anything else it wants.
In practice, though, most operating systems compare two data structures:
one representing the permissions of the current process
and another representing the restrictions on the requested resource.

The operating system maintains information for all running processes.
This information necessarily includes the processor state each had when it was last suspended by an exception and the base address of its page table.
It also has a per-preocess list of open file descriptors
and bookkeeping information
such as the processes' name, how long it has been running,
and its priority for getting scheduled to run again.
And most operating systems maintain the user account under which it is running.

# User accounts

User accounts are a common abstraction provided by operating systems.
They typically include

- A login name, like `mst3k`.
- A real name, like `M. S. Theater`.
- A home directory, like `/home/mst3k`, where many per-user preferences like desktop backgrounds and so on can be stored as ordinary files.
- The cryptographic hash of a password. Most operating systems never store your password anywhere, instead hashing it and storing only the hash, so that even if someone breaks into the OS and reads everything it has stored they still won't learn your password.
- A set of groups this user belongs to. Groups are often nothing more than a unique integer and string each.

Notably absent from that list is any mention of permissions.
Instead of permissions being listed per account,
it is more typical to list, for each resource, which user accounts and groups may access it.

{.example ...} Suppose as part of the results of an `ls -l`{.bash}
invocation you see the line

	-rwxr-x--- 4 mst3k coa     371 Jan 18  2019 file.txt

Let's take this line apart bit by bit:

`-`
:	This is a normal file, not a directory

`rwx` and `mst3k`
:	Processes running under user account `mst3k` are allowed to read, write, and execute this file.

`r-x` and `coa`
:	Processes running under any user account that belongs to group `coa` are allowed to read and execute this file, but not write to it.

`---`
:	Processes running under any user account other than `mst3k` and not belonging to the group `wheel` are not allowed to read, write, or execute it.

`371`
:	The file contains 371 bytes of data.
	This does not include its metadata: the filename, permissions, etc, listed here.

`Jan 18  2019`
:	The file claims to have been last modified on `2019-01-18`.

`file.txt`
:	The name of the file is `file.txt`

Every piece of this is metadata stored on the disk with the file.
When the operating system is asked to access this file,
it first reads the metadata.
It then compares the user account and groups of the running process
to the user accounts and groups that the metadata asserts are allowed to access the file
and only proceeds with the access if they match for the kind of activity
the operating system is being asked to perform.
{/}

Because user accounts are enforced only by the operating system,
not the hardware itself,
they can all by bypassed if you can boot the computer
using a different operating system.
Since hardware almost always allows booting from removable media
as part of how it permits updates and installations of operating systems,
physical access to a computer, coupled with the right resources and patience,
can bypass all user-account based protections.

Being logged in as a user
simply means that the software you see as the user interface,
weather that's a terminal or a visual display,
is running with that user in the operating system's bookkeeping for that process.
Since a process typically opens up other processes using the same account,
that means that all the processes you run from this session will be under that same user.

{.aside ...}
There are alternatives to having one account per user.
For example, Android, which is based on the Linux kernel and thus has the same basic user account mechanisms as we describe here,
creates [a different user account for each app](https://source.android.com/security/app-sandbox).
Thus the user interface user can become any app's user when launching the app,
and different apps cannot (by default) access files created for one another.
When apps are installed, Android not only installs its files
but also creates a user account for it and provides a list of permissions for that account.
{/}

# One account to rule them all

Typically, operating systems have one special account
or group
that is allowed to do anything, bypassing all normal account rules.
On many UNIX-based systems this account is called `root`;
it is also often referred as "the superuser", "the administrator", or "the administrator account"
(or, if a group, "an administrator account").

Among the operations that are restricted by user account
is the ability to change user accounts.
It is common for OSs to limit this ability in two ways:

- The superuser may become any other user freely

- There is a set of accounts that may become other users,
	but only after the operating system forces them to type a password

This has some parallel to how privileged mode operation works on hardware:
if you are already privileged, you can loose that freely;
but to gain privilege you have to pass through a special piece of OS-owned software---in this case, an OS-owned identity confirmation process to make sure the human who owns this account actually wants this process to become the superuser.

For a combination of historical reasons and the annoyance of having to type passwords to install things,
there remains^[as of 2019] a trend for users of some operating systems
to run their computers directly from the administrator account.
Doing this is dangerous: it gives each and every process
explicit permission to ignore all protections not enforced by the hardware
and do anything it wants to any part of your system.
Major OS vendors have worked steadily to disincentive this behavior,
but these efforts have been limited by users' lack of understanding
about what protections non-admin accounts provide.

# Daemons

It is common for a running computer to have well over a hundred processes running
that don't seem to be doing anything.
They sit in the background, waiting for some event to occur.
When it occurs the operating system gives them CPU time,
but they often use only a few microseconds before they again go quiet,
waiting for another event.

{.exercise ...}
Try running `ps -A u` to list all current processes with their owning users,
names, and various statistics about their running.

How many processes are there?

How many of them have been running for at least a day but used 0:00 minutes of CPU time?
{/}

These processes are daemons, and can be thought of as the software parallel
to an interrupt handler.
Like an interrupt handler, they take up some memory at all times
but only use the CPU when something happens.
That "something", however, is not an external even like a keystroke
but rather an internal event OS-moderated like data appearing on a socket.
There are various purposes to having a daemon,
but one of them is to provide controlled, limited access to a different user account.

{.example ...}
Most Linux systems use CUPS^[formerly "the Common UNIX Printing System" but officially changes to just CUPS due to trademark disputes about the use of the word UNIX] to manage printing;
CUPS runs primarily^[see [the CUPS design overview](https://www.cups.org/doc/spec-design.html) for how `cupsd` this works with the other components of CUPS] as a daemon `cupsd`.
Because printing can involve large amounts of data being fed slowly though a mechanical device^[among other reasons...],
print jobs are "spooled" into files temporarily while being printed.
Since we don't want other programs to be able to see the contents of the print jobs we have pending, we want those files to be locked down to a special user account.
On a default installation, that account is `root`,
meaning `cupsd` needs to run as the `root` user.

When a program wants to print, it contacts `cupsd`
(typically via a socket)
and sends it the information about the print job in question.
Because `cupsd` is running as `root`, it can spool that information to a file owned by `root`, and thus inaccessible to non-`root` users,
and send it to a printer by accessing system calls
that the OS restricts to the `root` account.
{/}

# What is "the operating system"?

The operating system is in fact a system:
many different inter-operating components.
When people refer to an operating system they might mean
many different things, including:

a.  The code that resides in the privileged memory of every process
	and runs in privileged mode:
	the list of processes, exception handlers, system call code, etc.

b.  All of a., plus the supplied wrapper functions like `open`
	and supplied headers and libraries that interface with a.

c.  All of a., plus all of the processes that run as `root`
	and are automatically started during boot-up,
	plus all of the user account and file permission bookkeeping that helps provide user separation

d.  The subset of c. that was shipped together as a named system

e.  All of d., plus all the applications that came bundled with it

f.  The subset of d. that cannot be easily swapped out for other components

There are other definitions used too, but these provide a feel for the kinds of uncertainty and disagreements that this term entails.
