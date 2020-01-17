---
title: Accessing remote servers
...

If you are running a POSIX-compatible system, most of the coding you'll be asked to do this semester should work fine on your own machine.
If not, consider the following options.

# Use SSH without passwords

Typing passwords is both less secure (key-sniffers, typos, typing wrong passwords, etc) and more tedious than using a private key.

## Concept

You'll place a file on your computer and a file on the remote computer.
They are matched, and each provides half of the work needed to do a job.
When you log in, the remote computer will do half the work with its file, then send that to your computer to do the other half, then send it back, thus allowing both computers to be confident the other computer is who it says it is^[This is a gross over-simplification, but gets the core idea across. We'll see what's really going on when we discuss digital certificates].

## Setup

The following commands should work on any system with SSH installed[^error],


```bash
ssh-keygen -f ~/.ssh/id_rsa -t rsa -b 2048
```

When prompted for a passphrase by `ssh-keygen`, just press enter without typing anything.
Once `ssh-keygen` is done, type the following[^manual] with appropriate changes to `username@the.server.edu`:

```bash
ssh-copy-id -i ~/.ssh/id_rsa.pub username@the.server.edu
```

When prompted for a passphrase by `ssh-copy-id`, use your UVA CS account password.


[^error]:
    If on Windows, you also may need to use `\` instead of `/` (whether you do or not depends on which command line tool you use).
    
    There is a slight chance that `~/.ssh` will not already exist. In that case `ssh-keygen` will fail; if you see such a failure you can fix it by running 

    ````bash
    mkdir ~/.ssh
    chmod 700 ~/.ssh
    ````

    and then re-run the above commands

[^manual]:
    `ssh-copy-id` is simple enough, some distributions of OpenSSL don't include it.
    If you don't have it, try 
    
    ````bash
    cat "~/.ssh/id_rsa.pub" | ssh username@portal.cs.virginia.edu "cat >> .ssh/authorized_keys"
    ````
    
    On Windows, you might need to use `type` instead of `cat`:
    
    ````
    type "~/.ssh/id_rsa.pub" | ssh username@portal.cs.virginia.edu "cat >> .ssh/authorized_keys"
    ````
    


# Edit local, compile remote

You can edit files on your own machine and send them to the servers just for compiling and running.

You should put each program in its own folder in this course.

To push files from your computer to the servers
:   1. Open the terminal or command prompt your operating system supports

    2. Change to the directory of your code

    3. Run `scp * mst3k@portal.cs.virginia.edu:code/project/directory/`, where

        - `*` means "all files in the current directory"
        - `mst3k` is your computing ID
        - `code/project/directory/` is the path to the directory on the department servers where your code will live.
            You may have to create this directory on the department servers first.

To run commands remotely
:   1. Open the terminal or command prompt your operating system supports

    2. Run `ssh mst3k@portal.cs.virginia.edu "the command you want to run on the server"`, where

        - `mst3k` is your computing ID
        - `the command you want to run on the server` is a valid linux command, which may be several commands separated by `;`
            
            For example, you might do something like `"cd coa2/warmup; make"`

You can even put all of this in a [makefile](lab01-make.html), as e.g.

```makefile
.PHONY: remote

remote:
	scp * mst3k@portal.cs.virginia.edu:code/project/directory/
	ssh mst3k@portal.cs.virginia.edu "cd code/project/directory/; make"
```

