---
title: PA03 -- Page table, continued
...

In the real world, it is uncommon to be able to write code and then walk away from it.
This PA is a set of modifications to make to your finished product from [PA02](pa02-pagetable.html).

# Changes

## Satisfy the testing team

The testing team has created a set of test cases they use to see if your code works properly.
These tests will be automatically re-run every day sometime between 2pm and 3pm,
and will check your most recent submission when they run.
Per typical quality assurance practices,
they will neither tell you their specific test cases
nor promise not to add additional tests without warning.
This withheld information should help you avoid over-fitting your code to their specific tests.

Your code should pass every test they have^[We recognize this will be harder for some than for others based on how far you started from this goal; to avoid double-jeopardy, we'll award some of these points for improvement if your PA02 was not awarded full credit.].
If you cannot pass every test by the deadline,
add a description of what your code cannot do to the `README`.

Note that if you submitted a file you don't want, there is currently no way to remove it. However, you can upload an empty file with the same name to remove it from what happens.

## Prepare for code review

Code reviews will occur after PA03 closes. To prepare for these

- Ensure there are no warnings^[
		...because you resolved the warnings,
		not because you disabled them with warning-ignoring `#pragma`s or the like.
	] when compiling.
	To put this another way, adding `-Wall -Werror` to your `Makefile`'s `CFLAGS` should not prevent compilation.

- Use the following style: 
	- 4-space indentation
	- `{`{.c} on the previous line and `}`{.c} on a line by itself
	- No empty statements
	- Spaces around operators
	- Space before `(` if and only if it is preceded by a control construct
		(e.g., `if (` but `translate(`)
	- No postfix `++`{.c} or `--`{.c} unless part of an expression that uses the return value (such as `a[i++]`).
		Replacing with either prefix `++`{.c} or `+= 1`{.c} is OK.
	- Lower-case identifiers with underscores separating words
	- Upper-case `#define`{.c} constants
	- `typedef`{.c} such that each type name uses a single identifier
		(e.g., no `unsigned char`{.c} except as part of a `typedef`).

- Use variable names that indicate the meaning of the contents of the variable;
	function names that indicate the purpose of the function;
	etc.

- Have a readable code organization; as rough (not strict) guidelines,
	- If a sequence of lines of code depend upon one another more than they depend on the code around them
		an appear together in multiple places, they should be in their own function.
	- Functions should be small enough to look at on one screen
		unless that have a very orderly structure such as a long `switch`{.c} with simple code in the `case`{.c}s.
	- Comments should be present to describe any esoteric code decisions.
	- All code should have a clear purpose.

- Have in-code documentation inside a comment beginning `/**` or `///` immediately before each non-trivial function. Ensure all non-documentation comments use `//` or `/*` instead.

## Prepare for delivery

Prepare your code for delivery. In particular,

- Adjust your `Makefile` to have, as its default target, `libmlpt.a`,
	a library with exported symbols including the three defined in `mlpt.h`
	and not including `main`.
	
	A typical command might look like
	
	````makefile
	libmlpt.a: $(objects)
		ar rcs libmlpt.a $(objects)
	````
	
	The default (what happens if you just type `make`)
	is the first target in the Makefile.
	
	To use a library file like this, you'd add to your LDFLAGS two flags:
	
	- `-L.` means "look for library files in the current directory"
	- `-lmlpt` means "look for library files `libmlpt.a` or `libmplt.so`"

- Write a `README` explaining (at a minimum)
	- How to customize `config.h`, with some guidance on how to pick values for that file.
	- An example use case for this code. Note that this cannot be "to implement virtual memory" since this code relies on `posix_memalign`, which cannot work without virtual memory already being in place.
	- Any known bugs (if you know of none, there is no need to say so).
	
	You are encouraged to add additional detail, such as
	
	- Any limitations, aspects you think are incomplete, or suggestions for future expansion.
	- Big-O analysis (time, space, or both).
	- Description of any testing hooks you added.
	- Code samples for how to use this library.

- Include a `LICENSE` file with the license agreement under which you provide your code to us.
	This should be based on an existing family of license agreements,
	and must be one that requires attribution 
	so that if some future student obtains your code
	it is illegal for them to submit it, or a significant subset of it, as their own.

- If anyone or any resource helped your implementation,
	include an `ACKNOWLEDGEMENTS` file acknowledging those people and/or resources.

## Compare licenses

Include a file named `licenses.txt` in which you list at least three licenses you considered;
a short (sentence or two) summary of what they seemed to say;
and why you chose to or not to use each as your license.

Your review should include at least three licenses, including

- at least one of GPL or LGPL
- at least one of MIT, BSD, Boost, Apache, SQLite

All should be *software* licenses; this excludes licenses
such as the various CC licenses for creative works,
the SIL Open Font License,
operating system licenses,
etc.

## Consider page-table version of "free"

Your boss sent you an email:

> I made the mistake of telling someone there was no de-allocate available
> and now we're supposed to "see about adding it." I tried to say this was
> harder than it sounded, but wasn't able to convince them on the spot...
>
> See if you can add some kind of de-allocate interface. If so, add it to 
> `mlpt.h` and document how it is used. If, as I suspect, it is trickier
> than it sounds, include some explanation of what complicates it to the 
> `README` instead of implementing it.
>
> Sorry about this.

# Not required: create a manual page

If you want to understand the real world a bit better, you might also try writing a manual page.
These are written in a language known as "roff";
to see an example of what this looks like, try running

```sh
cp /usr/share/man/man3/poll.3p.gz ./
gunzip poll.3p.gz
less poll.3p
```

to see the roff source of the manual page `man 3p poll`.
I don't personally know anyone who writes roff by hand;
generally it is output from a source-code processing tool like `doxygen`^[which is installed on department machines; see `man doxygen`{.bash} for more.]
or a converter like `pandoc`^[a very old version (1.3) is installed on the department servers without man support; visit <https://pandoc.org/> to get a copy yourself.] from an easier-to-write format like markdown or reStrucuredText.

Including man page `page_allocate.3.gz` and `translate.3.gz` in your submission will earn you kudos.
To test that these work properly,

1. create a directory `man3` (i.e., `mkdir man3`{.sh})

2. gzip the `roff` file and move it to `man3/term_to_look_up.3.gz` (e.g., `gzip translate.roff; mv translate.roff.gz man3/translate.3.gz`{.sh}) -- note that some authoring tools may do some of this for you

3. use the `-M` flag with `man` pointing to the parent directory of `man3` (i.e., `man -M. 3 translate`{.sh})
