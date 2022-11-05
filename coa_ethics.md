---
title: COA Ethics lessons
author: Luther Tychonievich
...

This page is targeted to faculty leading discussions in COA1 and COA2, rather than to students.

# Back Doors

This topic should be addressed in two different lessons

## On trusting trust

Outcomes
:   The students should be able to

    1. articulate how nefarious behaviors can be hidden in computer systems
    2. appreciate the difficulty of bootstrapping trust
    3. evaluate security claims of computer systems critically

Scheduling
:   This lesson should follow the students' first exploration of how instructions can be coded into an ISA and parsed with muxes

Flow
:   1. Show the instruction parsing code for an already-discussed ISA

    2. Add in a chain of logic that checks successive memory reads for some special sequence of bytes
    
    {.example ...}
    One way to do this is to add a special hidden multi-bit register $R$.
    Use $R$ as the selection input to a mux with fixed constants as the value inputs, and use the output of that mux as the input to an equality comparison circuit.
    Set $R$'s new value to be 0 if the equality is false, or $R+1$ if it is true.
    {/}
    
    3. Add a mux with $R = 16$ (or however many bytes you used) as the selection and two inputs: "normal, documented behavior" and "special secret behavior"

    4. Lead a discussion. Example questions to seed discussion include
        
        - Called a "back door": undocumented way to bypass usual rules
        - How could you know if the chip in your laptop had this kind of extra logic?
        - How might the military make use of this kind of technique? Who would have to be on board to do that?
        - If you ran a chip design company for a billion-transistor chip, how could you ensure some engineer somewhere didn't add this kind of back door?
        - If you were building a new smartphone, what *legitimate* practice might lead you to add a back door? What *illegitimate* business demands might be benefited by creating one?
    
    5. Reference (or assign reading of and discuss) "On Trusting Trust"^[Ken Thompson, "Reflections on Trusting Trust", August 1984, *Communications of the ACM* 27(8), pp. 761--763. doi>[10.1145/358198.358210](https://doi.org/10.1145/358198.358210)] or any of the many related papers.

    6. Leave a hook by telling students this topic will be revisited as they learn more.


## Debugging hooks and backdoors

Outcomes
:   The students should be able to

    1. articulate the trade-offs intrinsic in an ethical dilemma
    2. take an ethical principle from statement of principle through suggested practical activity
    3. understand how engineering considerations may be at odds with ethical considerations such as privacy and social good
    4. feel prepared to address ethical issues with supervisors and managers

Scheduling
:   This lesson should follow the students' learning C and attempting to implement a program that is not trivially testable. Ideally they should have had to add hooks outside the API to verify some required behaviors.
    
    A simple example is a program that is required to invoke `malloc` and `free` a particular number of times in a particular order. This lesson will work more effectively if you have had a conversation about how to add hooks to help.

Flow
:   1. Review the idea of debugging hooks, and their prevalence in real systems
    
    2. Pose the following question: "should you remove debugging hooks before releasing software?" Encourage a debate, supporting the less-defended side:
        - what if you need to fix a bug after release?
        - but these have performance costs
        - but some bugs depend on timing, so you need live timing
        - but hooks are rarely debugged and likely to have exploits
        - ...
    
    3. Suggest that sometimes you need more than a simple hook; how do you respond when someone says
    
        - "I forgot my password and can't get into my system"
        - They call up and ask you to log in remotely and fix something
        - ...
   
        Solution: maintenance backdoor
    
    4. What percentage of your fellow students would you trust to have access to a maintenance backdoor?
        
        - do you trust yourself?
        - is there a way to get the advantages without the risk?
    
Assignment
:   Write a draft email replying to whichever of the following you object to:

    - Your boss has instructed you to include a secret master password in every copy of the product so you can provide high-value support to those who pay for such. You are assured that by company policy no one will use it unless they request it use.
        
        Draft an email expressing your objection to having such a master password included. Assume your boss is a decent person with experience (but not formal education) with coding but mostly focused on business.

    - Your boss's boss has instructed you to remove all hooks and backdoors, even the ones you added after spending fifteen hours trying to help clients who misused the system in a way you could have diagnosed in five seconds if you had already had the backdoor in place.
        
        Draft an email *either* to your boss's boss explaining why this policy should have an exception for your backdoor *or* to your boss explaining why your leaving the backdoor in place is something your boss should not tell your boss's boss.

Post-assignment activity
:   Discuss the down-side potential of anonymized excerpt from various student-submitted emails. Help emphasize the dilemma: both options are giving something up.

    Review ACM code of ethics^[<https://ethics.acm.org/>], article 2.8. Discuss why this is not a black-and-white rule ("without a reasonable belief...") but still so strongly worded ("extraordinary precautions must be taken...")



# On-time or Safe?

Outcomes
:   The students should be able to

    1. understand how business considerations may be at odds with ethical considerations such as security and trust
    2. recognize that engineers often prioritize ease over correctness, and that such have consequences
    3. picture how they might be tempted to cut corners and leave exploits in their own code in the future

Scheduling
:   This lesson should follow an assignment where the students are required to make a program robust to incorrect input.
    Ideally this should be a two-part assignment:
    first the functionality part submitted, tested, and graded;
    and then a second assignment with the same specification but robustness required.

Flow
:   1. Ask "what percentage of your time was spent on corner cases and making the code robust?"
    
    2. Explain that this increases when tasks are more complicated. If you have discussed these topics, mention safeguarding against timing attacks, ensuring only hashes of private data are stored, handling recovery after crashes, supporting infinite undo without other users being able to use it for snooping, etc.
    
    3. Ask "is it possible to write 100% correct, robust code?" In the discussion, mention proof-carrying code and a report discussing implementing a full system this way, such as seL4^[<https://sel4.systems/>; Klein, Elphinstone, Heiser, Andronick, Cock, Derrin, Elkaduwe, Engelhardt, Kolanski, Norrish, Sewell, Tuch, Winwood (2009) "seL4: Formal Verification of an OS Kernel" *ACM Symposium on Operating Systems Principles* pp. 207--220.] which took 2.2 person-years to implement and 20 person-years to prove correct.
    
    4. How long has it been since you last had a program crash, website not respond, etc.?
        
        If any other industry had similar failure rates...
    
    5. "If we can write provably-correct code, why don't we?"
        
        - Which would you buy: a 20-year-old program that never crashes or a new program that does now and again?
        - We have trained you to expect crashes
        - It is hard to market what a product does *not* do
    
    6. Review ACM code of ethics^[<https://ethics.acm.org/>], article 2.9. Discuss how "usably" is very often seen as at odds with "secure" in part because no one uses a system that cannot be marketed and sold.

{.note ...}
Missing component of this lesson
:   I do not yet have a good "and therefore what" closer for this discussion.
    I tried "your boss will likely be on the other side of you; how should you react?" but that didn't engage their attention.
{/}


# Reporting procedures

Outcomes
:   The students should be able to

    1. know how to use responsible reporting procedures if they discover a security exploit
    2. know how to look up vulnerabilities in centralized databases and understand the severity levels listed there
    3. understand the importance of updating software they use, both end-user products and dependent libraries

Scheduling
:   This lesson should follow a discussion of exploitable code, such as C programs vulnerable to stack overflow attacks; how at least one such exploits works; and how to recognize exploitable vs non-exploitable code for that exploit. Ideally it should also follow an exploration of assembly, what those exploits look like in assembly, and exposure to a disassembler like `objdump`.

Flow
:   1. "Suppose one day you are trying to solve a particularly difficult bug in a system and are beginning to think maybe a third-party library might not be doing exactly what you thought. You disassemble the binary and are reading the code to understand what it is actually doing, and as you do you notice that several places in that code are vulnerable to *recently discussed exploit*. What should you do next?"
    
    2. Lead a discussion. If the students do not bring these up themselves, raise the following options:
        
        - Write an exploit to prove to the library authors that they have a bug
        - Report the vulnerability to the company in secret so they can fix it before it is exploited
        - Report the vulnerability publicly to put pressure on the company to fix it
        - Switch to a different library and say no more about it, giving yourself a competitive edge over other companies using the exploitable code
        - Make a note, so that if later someone attacks your product through that exploit, you can blame the library provider, and then move on
        - Publish the exploit; you can get famous finding a big vulnerability
    
    3. Briefly explain industry best practice:
        
        a. inform those who should fix the problem privately
        b. after a suitable time has elapsed, publish the vulnerability and (if it has been fixed) the first version of the software to fix it
        
    4. Visit the [CVE List](https://cve.mitre.org/cve/) and/or the [NVD](https://nvd.nist.gov/) and discuss the meaning of each part of that database, including the idea of [vulnerability metrics](https://nvd.nist.gov/vuln-metrics/cvss). Help make this concrete by discussing the maximum downside potential of a selection of recent vulnerabilities.
    
    5. Emphasize the importance of frequent security-patch updates to all software.


# Predicting secondary impact

Outcomes
:   The students should be able to

    1. explain how coding decisions impact resource usage
    2. understand power consumption as a metric of software quality
    3. compare the business cost to the social cost of resource usage priorities

Scheduling
:   This lesson should follow a discussion of power use in processors.

Flow
:   1. "What percentage of world power output goes to run computers?"
        After taking guesses, pull up a recent article on this (e.g., [LBL report from 2016](https://eta.lbl.gov/publications/united-states-data-center-energy), [Forbes article from 2017](https://www.forbes.com/sites/forbestechcouncil/2017/12/15/why-energy-is-a-big-and-rapidly-growing-problem-for-data-centers/), [Data Economy prediction from 2017](https://data-economy.com/data-centres-world-will-consume-1-5-earths-power-2025/), etc)
    
    2. Discuss how design decisions impact this
        
        - Hardware decisions clearly
        - Patience -- in general, slower clock cycle = more efficient
        - Algorithmic and implementation efficiency -- including language choice; interpreted fast enough = good business sense, but ≠ good energy sense
        - Sometimes much more significant...
    
    3. Discuss bitcoin as example
        
        - (see [any](https://arstechnica.com/tech-policy/2017/12/bitcoins-insane-energy-consumption-explained/) [of](https://digiconomist.net/bitcoin-energy-consumption) [the](https://www.theguardian.com/technology/2018/jan/17/bitcoin-electricity-usage-huge-climate-cryptocurrency) [many](https://motherboard.vice.com/en_us/article/ywbbpm/bitcoin-mining-electricity-consumption-ethereum-energy-climate-change) [articles](https://www.usatoday.com/story/news/world/2017/12/21/bitcoins-sky-rocketing-energy-use-viral-story-we-checked-math/972485001/) [for](https://engaging-data.com/bitcoin-energy-consumption/) [more](https://www.cnbc.com/2017/12/21/no-bitcoin-is-likely-not-going-to-consume-all-the-worlds-energy-in-2020.html))
        - Cryptocurrencies rely on consensus, which relies on limiting how many votes each player has; combined with anonymity, that means we need algorithmic limitations
        - Bitcoin chose computational power (i.e., access to hardware and power to run it) as a limiting factor
        - Exact numbers are hard to know (but easy to find people guessing about), but this design intrinsically and intentionally causes bitcoin transactions to have a high energy cost
    
    4. Energy is not the only predictable resource impact of technology. Discuss a few examples, such as
        
        - How many outdated electronics components are produced as waste? How does picking a minimum hardware profile to support in your latest and greatest game impact that?
        
        - How much more secure does the next way of handling credit card transactions need to be before it justifies replacing all cards and card readers?

    5. Run a real experiment showing actual power usage
    
        {.example ...}
        Using a native-linux laptop with an Intel processor,
        
        Show total micro-Joules consumed since boot
        :   `cat /sys/class/powercap/intel-rapl/*/energy_uj`{.bash}
        
        Create similar programs in C and Python and compare usage
        :   See full code in footnote[^watt-code]
        {/}

    6. Lead a discussion around the question "Under what conditions should software engineers prioritize resource efficiency over business costs like developer time and time to market?"

[^watt-code]:
    ````bash
    #!/bin/bash
    
    cat > prime_map.py <<EOF
    cnt=0
    for x in range(1001,10000,2):
        prime = True
        for j in range(3, int(x**0.5)+1, 2):
           if x%j == 0:
                prime = False
                break
        cnt += prime
    exit(cnt)
    EOF
    
    cat > prime_map.c <<EOF
    #include <math.h>
    
    int main() {
        int cnt = 0;
        for(int x=1001; x<10000; x+=2) {
            char prime = 1;
            int cap = (int)sqrt(x)+1;
            for(int j=3; j<cap; j+=2) {
                if (x % j == 0) {
                    prime = 0;
                    break;
                }
            }
            cnt += prime;
        }
        return cnt;
    }
    EOF
    
    clang -O3 -lm prime_map.c -o prime_map
    
    python3 prime_map.py # warm up loader
    w1=$(cat /sys/class/powercap/intel-rapl/*/energy_uj)
    python3 prime_map.py
    w2=$(cat /sys/class/powercap/intel-rapl/*/energy_uj)
    pywatt=$((w2-w1))
    
    ./prime_map # warm up loader
    w1=$(cat /sys/class/powercap/intel-rapl/*/energy_uj)
    ./prime_map
    w2=$(cat /sys/class/powercap/intel-rapl/*/energy_uj)
    cwatt=$((w2-w1))
    
    printf 'Python: %12d µW\n' $pywatt
    printf 'C:      %12d µW\n' $cwatt
    printf 'Difference: %5.2f%%\n' $(echo $pywatt*100/$cwatt | bc -l)
    ````


# Power allocation

This topic should be addressed in two different lessons

{.note ...}
I added this topic late in Spring 2019 after reading the March 2019 CACM article by Sepehr Vakil and Jennifer Higgs, "[It's About Power](https://cacm.acm.org/magazines/2019/3/234921-its-about-power/fulltext)". The discussion went very well and was referenced several times later in the term by the students. I hope to expand my coverage in future semesters.

That said, several components of this I have used in other contexts, hence some of my "I usually" notes.
{/}

## Who is empowered?

Outcomes
:   The students should be able to

    1. Articulate how they hope their software is not used
    2. Compare design decisions based on the impedance they offer certain users
    3. Identify populations who will be disproportionately left behind by a new software feature or toll

Scheduling
:   This should be placed after students have explored a system or two in some detail, but early enough that this lesson can be revisited as future technologies are discussed.

Flow
:   1. Explain that tools make aspects of life easier only for some people; give two non-computing examples, one based on *knowledge* and the other on *accessibility*; for example,
        
        - chopsticks only make eating easier for those who've learned to use them
        - stairs only make changing elevation easier for those with functioning legs
        
        Point out that these two cases are different in kind: one is about knowledge, the other about accessibility.
    
    2. Bring up a recently-discussed enabling technology, such as bash autocompletion, Linux manual pages, the address sanitizer, etc.
    
    3. Explain that this technology "makes $X$ easier for some people, but not for others, just like we saw with _chopsticks or other previous example_. With your elbow partners, identify similar people who are differently enabled by this technology, both by knowledge and by accessibility."
    
        {.note} that script needs work
    
    4. After they've had some time to discuss in small groups, ask some groups to share. Lead a discussion on if the identified limitations are intrinsic or mitigable, and if mitigable ask for suggestions on how to mitigate them.
    
    5. Point out that all technologies create power, and distribute it unequally. This distribution can at times either mitigate or reinforce existing inequities.
    
    6. Discuss some examples of "anyone could use it except group X," such as
    
        - Almost by definition, all new software increases the power divide between those with reliable access to computing resources and those without. But applications that rely on consistent high-speed internet, recent hardware, or large amounts of computing power do more to reinforce this divide than do other applications.
        
        - Ad-hoc user interfaces often exacerbate the disadvantage of the visually challenged.
        
    7. Discuss some examples of "it's powerful if you know how to use it," such as bad or missing documentation, idiosyncratic user interfaces, etc.
        
        {.note} I do not have a good example here. I tried getfacl/setfacl vs chmod but it did not go over well. I tried showing some of the tools I created for class with incomplete UI, but the overhead was to great.
    
    8. Discuss some examples of "guns don't kill people, people kill people, but they find it a lot easier with guns than without," such as
    
        - jailbreaks and root kits
        - building missile guidance software
        - taking a job for any company whose mission does no align with your values
        
        Emphasize that individual feelings on each case may vary, but that everyone can find the dubious case: "either you support writing better DRM software or writing DRM-cracking software, but few rational people support both"; etc.
    
    9. Discuss the impedance of tedium: people are much more likely to do what is easy to do even if they know how to do the harder thing too.
    
        {.note} I do not have a good example here. I once tried showing the dozens of steps needed to put one quiz question into collab, but because it was tedious I lost attention and in the end they didn't relate. I've considered the default permissions of new files and directories in Linux, but don't feel it is ideal.

    10. Conclude with a summary[^toolong] and an encouragement to consider who is (not) empowered by what they do in the future.


[^toolong]:
    {.note} There are too many steps in this lesson if it needs a summary.

## "Power-user" interfaces

Outcomes
:   The students should be able to
    
    1. Appreciate that they are becoming a privileged class by learning CS
    2. Explain the inequities of access created by both (a) two-interface systems and (b) only power-user-interface systems
    3. Explain why power-user-interfaces tend to have opaque-to-novices UX design
    4. Understand that learning curves create imbalanced usability impedance

Scheduling
:   This should be placed around the time that students are engaged with learning command-line interfaces.

Flow
:   1. Ask the question "why are we teaching you the command line?"
        
        Expect a lot of correct-ish answers like "sometimes you only have it" and "some people expect you to use it" and "some tasks require it." Accept and reinforce each, but do not suggest they are the one true answer.
    
    2. Explain that the Linux command line is an example of an interface aimed at the "power user." Define [power user](https://en.wikipedia.org/wiki/Power_user) as one who (a) uses a system extensively enough to want to learn all it can do and (b) does things with it that most cannot.
        
    3. Most power-user interfaces make use of options that are not displayed in any way, such as
            
        - keyboard shortcuts, like typing "F6" in a web browser
        - invisible state machines, like typing "Ctrl+U 2 6 3 A Enter" in a GTK-based system or switching between insertion and control mode in VI
        - type any of a giant list of commands in a command line or console
        
        Many power-user interfaces are actually programming languages; the linux command line is commonly bash, for example[^webconsole].

    4. We teach you CLI so you can be power users.
        
        Power users want more options that good GUI design can represent. Consider the 4000+ programs listed by `ls /usr/bin | wc -l`, and the dozens of options each has as `ls --help` exemplifies, and try to imagine a menu system that got at all of that...
    
    5. Explain that power user interfaces *create* new class systems within the digital world: the cans and cannots. This is true whether they are the only interface (how many MacOS users ever open the terminal?) or a secondary interface (how many browser users ever interact with the console?).
    
    6. Ask for solutions to this problem of creating inequity, and discuss replies. I found it good to be prepared to discuss the following kinds of answers:
        
        - We need mandatory power-user training, CS education, etc.
        
            {.note ...}
            In reply I mention the idea of learning curves: it takes time to learn, time not used for other things, so what are we sacrificing?
            
            I also sometimes discuss the lack of qualified teachers and thus the long-term character of such solutions.
            {/}
        
        - We should not make power-user interfaces; make all programs simple.

            {.note} In reply I ask how many people, given power, will willingly give it up to become equal with the unempowered.
        
        - This isn't a problem; the casual user doesn't want that power and wouldn't know what to do with it if they had it.
            
            {.note} I don't have a single standard reply, but often can do some kind of echo-and-confirm "just to be clear, you're saying that people who don't know how to do something must not want the power to do it---power they don't even know they don't have?"
            
        - Once AI masters understanding what we want this problem will go away.
            
            {.note} I usually reply with something like "AI is just another class of applications, which themselves have power-user and casual-user interfaces. I can't predict the future, but at least on their current trajectory they are making the separation of cans and cannots more, not less, extreme."
            
        - Better help guides, UI design, etc, can make the separation go away.
            
            {.note} In reply I often summarize "Good UI is intuitive, but intuition comes from subconscious learning by repeated exposure to a pattern. When we are offering more power than most users even realize can exist, there is no intuition to use, and hence no good UI---until people become accustomed to whatever UI we make and start to develop an "intuitive" expectation for that design." It's not a perfect response, but it does the job.
    

[^webconsole]:
    I like to show other power-user PL-CLI interfaces they won't expect, such as the web browser javascript console. For example, I have done enough web scraping in my day that I know I can go to cs.virginia.edu and type
    
    ```javascript
    a = ''
    document.querySelectorAll('div.image')
        .forEach(x => a += x
            .style
            .backgroundImage
            .replace('url(','<img src=')
            .replace(')','/>')
        )
    document.head.innerHTML = ''; document.body.innerHTML = a;
    ```
    
    to get a page of images about the latest news story or 
    
    ```javascript
    a=''; 
    document.querySelectorAll('figure img')
        .forEach(x => a+='<img src="'+x.src+'"/>');
    document.head.innerHTML = ''; document.body.innerHTML = a;
    ```
    
    to get a page of faculty photos, so I sometimes will create something like this live for the students.

