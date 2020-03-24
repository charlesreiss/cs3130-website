---
title: Computer Organization and Architecture 2
...


# Teaching Continuity

As of March 19, this course will be taught online. Here's how we plan to make that work. This may change if we determine better approaches to some elements.

Lectures
:   We'll try doing these via zoom. The time is unchanged from our usual lecture time (MoWeFr 14:00 EDT). The link to join is <https://virginia.zoom.us/j/138570502>. You will be muted upon joining; please un-mute only to ask a question.
    
    I plan to attempt doing "turn to your neighbor" parts of class via a feature called zoom breakout rooms. I have never done these at that scale, so we might have to adjust as we go.
    
    Lectures will still be recorded and posted to the website as usual.
    
    I am uncertain of the efficacy of this model. We may swap to having prerecorded lecture segments instead and use lecture time as additional professor office hours.

Labs
:   Will use the same zoom room as lecture (<https://virginia.zoom.us/j/138570502>). Most will involve some breakout room discussions and activities; some may also involve paired programming or other collaborative coding exercises. We'll post more with each lab writeup.

In-class exams
:   Will be replaced with take-home exams. I don't believe in closed-note take-home exams, so these will be open-note. Which means they have to ask a somewhat different kind of question. More on this as the time of our next test approaches

Online quizzes
:   Unchanged

Professor Office Hours
:   Will occur via zoom. The link to join is <https://virginia.zoom.us/j/847054967>. During scheduled office hours times, anyone who comes in will be in the same zoom room. If you have personal matters to discuss, email me about a different time to meet.

TA Office Hours
:   We'll adapt the current queue system as follows:
    
    - You will create a meeting room via google meetings (<https://meeet.google.com>) or zoom (<https://zoom.us>)
    - You will add yourself to the queue in the usual way (<https://kytos.cs.virginia.edu/ohq/?c=coa2>) but enter the URL of your meeting room as the location, not a seat in stacks.
    - A TA will join your room and offer help
    
    You are welcome to come in groups by inviting other students to your meeting room.

    If a more text-oriented help will suffice, we recommend using Piazza.

Accommodations, grading, etc
:   We expect this transition to be bumpy in various ways. Please feel free to use email, Piazza, or Collab's Anonymous Feedback tool to let me know of any issues. We'll do our best to work through them.
    
    I have a lot of experience teaching in-person, but very little teaching online. As such, I will be watching for signs of decreased instructional effectiveness and may decrease course expectations so that course grades are not decreased by my inexperience with this teaching medium.
    
    If you have a proposal for a different approach to how a component of this course could go, I'd welcome that suggestion.



# Overview 

This is our second offering of a course we hope will, along with several other
courses, replace and update several of our current courses.
More information about the pilot of these courses as a whole may be found at
<http://pilot.cs.virginia.edu/>.

For the sake of conversing with those familiar with our other course offerings,
this course covers much of CS 3330 "Computer Architecture" but at a higher level of detail;
the concurrency and security topics from CS 4414 "Operating Systems";
in addition to having several new topics we felt were under-represented in our
previous set of course offerings.

# Learning Outcomes

As a result of taking this course, you should learn

- core system architecture, including DMA, interrupts, and cache coherency
- real hardware, including concurrency complexities and what makes SPECTRE work
- concurrency, including synchronization, mutual exclusion, coherency, and concurrency bugs
- virtual memory, including process isolation and shared memory
- practical networking, including familiarity with TCP/IP, DNS, snooping, spoofing, and TLS
- security, including unix permissions, privilege escalation, and modern exploits
- ethical and legal issues, including reporting
- a build system and practical command-line tools

# Writeups

I have created several writeups for this course:

- [COA2 overview](intro.html)
- [Minimal makefiles](make.html)
- [Kernels -- Software for the Hardware](kernel.html)
- [User Accounts](accounts.html)
- [Buses (and networks)](bus.html)
- [Networking Protocols](protocols.html)
- [Security protocols with less math](sec.html)
- [Caches -- Keep a finger in it](cache.html)
- [Processes and threads](thread.html)
- [Synchronization primitives](sync.html)
- [Using POSIX Threads](pthreads.html)
- [Deadlock](deadlock.html)
- [Consistency](consistency.html)

We will augment these with various examples, online articles, and other resources.
