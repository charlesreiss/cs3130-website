---
title: Accessing remote servers
...

If you are running a POSIX-compatible system, most of the coding you'll be asked to do this semester should work fine on your own machine.
If not, consider the following options.

# NX with GUI editor

The NX system has some GUI editors.

The easiest GUI editor for people used to Windows or MacOS to use and setup
is gedit, which the NX build calls simply "Text Editor" in title bars, etc.

1. Run gedit by clicking the "Activities" button and typing `gedit`
2. To set up gedit for code editing
    a. Click the "Text Editor" item on the task switcher toolbar, then "Preferences" in the drop-down menu
        - In the *View* tab, check at least
            - Display line numbers
            - Enable text wrapping
            - Do not split words over two lines
            - Highlight matching brackets
        - In the "Editor" tab, check at least "Enable automatic indentation"
        - In the "Plugins" tab, check at least "External Tools"
    b. Click the "Text Editor" item on the task switcher toolbar, then "Manage External Tools..." in the drop-down menu
        
        Here you can write keyboard-shortcut commands to simplify common tasks.
        A few I suggest (and might be present by default) are
        
        Make (F8)
        :   
            ````bash
            #!/bin/sh
            
            make
            ````
        
        Run (F5)
        :   ````bash
            #!/bin/sh
            
            make run
            ````
    
        Test (F6)
        :   ````bash
            #!/bin/sh
            
            make test
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

# ssh with X-forwarding

If your computer has an X11 client installed (which is sadly uncommon),
you might be able to use something far more flexible than NX:

Try running `ssh -XC mst3k@portal.cs.virginia.edu` and then typing `gedit`.
If this works, you'll see a gedit window appearing that is running on the server
but being rendered on your computer.
You can then make use of full integration between your computer and the server
without relying on NX, Nomachine, etc.
If you see an error message, this option is not supported by your OS.

# Run a POSIX-compliant OS with clang

The `clang` compiler (part of the LLVM project) is desigend to be mostly cross-platform.
Not everything we do in class will work on all OS's, but most should.
If you take notes on any installation nuances and post them on Piazza,
I'm sure your fellow students would apprecite that.

Windows
:   - Clang via <http://releases.llvm.org/download.html>
 
    - Make via <http://gnuwin32.sourceforge.net/packages/make.htm>
 
    - I didn't find a clean-looking LLDB installation guide

MacOS
:   - Clang via `xcode-select --install`
  
    - I didn't find a clean-looking Make installation guide, but it might come with clang?
  
    - I didn't find a clean-looking LLDB installation guide, but it might come with clang?

Windows, with WSL
:   Windows ships with the ability to enable some POSIX-compliant features
    and then install an entire open-source OS to run inside Windows.
    
    See <https://docs.microsoft.com/en-us/windows/wsl/install-win10> for instructions.

# ssh without X-forwarding

Try running `ssh mst3k@portal.cs.virginia.edu` and then using command-line tools like `nano`.
This is likely to have many little issues if you run Windows, as `ssh` is designed assuming POSIX-compliance and Windows is not POSIX-compliant.
It should basically work, but you might have to put up with things like backspace not working, etc.
