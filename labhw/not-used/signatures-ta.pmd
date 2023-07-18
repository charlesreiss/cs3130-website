---
title: Signatures lab - TA hints
...

The goals of this lab are to help students understand

1. how signatures work
2. why hash collisions can cause problems
3. how you think about ways around code, as to create hash collisions

As such, brute-force solutions that simply try all 1.8 billion possible signatures until they find the right one, while possible (I was able to find all 7 for an arbitrary message in less than 30 seconds with a brute-force search), is not desirable.

When giving help, help students towards understanding how to create solutions. I'd much rather a student have a deep understanding of how to force collisions in Hashes 2 and 3 than that they get to Hash 6 without deep understanding.

The following are specific suggestions to on how to respond to "I'm stuck on hash *X*"-type questions.

Hash 0
:   Not all characters matter. Which ones do? How could you figure that out? How can you get your computing ID into a message without changing the characters that impact the hash?

Hash 1
:   see *Hash 0*

Hash 2
:   Are there any bytes that don't matter?
    How are bytes combined?
    What is the inverse of xor?
    
    Depending on their approach, you might go down either of these paths:
    
    -   What's the hash of the message you want to send?
        What's the hash of the message the website signed?
        How much would you have to add to your message to make them hash the to the same value?
    
    -   What's the hash of `"x"`?
        What's the hash of `"88x"`?
        How about `"845 84 x5"`?
        What if we wanted a string that contained at least one substring `"win!"` and hashed to the same thing as `"x"`?
        For this lab, what do you want instead of `win!` and instead of `x`?
    
Hash 3
:   Much like *Hash 2*, except they'll need the additive inverse instead of the xor inverse.
    
    What's the additive inverse of the byte 23? OK, yes, -23 is, but what about unsigned? Think two's complement.
    
    The additive inverse of `"mst3k"` is `"\x93\x8d\x8c\xcd\x95"` -- why? How did I come up with that?

Hash 4
:   Much like *Hash 2*, except

    How many bytes are used in the hash?
    Which bytes of the input contribute to each byte of the hash?
    How can you line up your string with those hashed-to bytes?

Hash 5
:   This one's quite a bit trickier. Drawing a series of pictures can help a great deal.
    
    What would the hash look like if the message was just `m`?
    
          00000000 00000000 00000000 mmmmmmmm
    
    What would the hash look like if the message was just `ms`?
    
          00000000 00000000 00000mmm mmmmm000
        ^ 00000000 00000000 00000000 ssssssss
    
    What would the hash look like if the message was just `mst`?
    
          00000000 00000000 00mmmmmm mm000000
        ^ 00000000 00000000 00000sss sssss000
        ^ 00000000 00000000 00000000 tttttttt
    
    Notice that the last letter is in fixed position, but the others move. So for the full reference message `mst3k passes` we have (starting at the end and using `.` instead of `0` for better visual clarity)
        
          ........ ........ ........ ssssssss
        ^ ........ ........ .....eee eeeee...
        ^ ........ ........ ..ssssss ss......
        ^ ........ .......s sssssss. ........
        ^ ........ ....aaaa aaaa.... ........
        ^ ........ .ppppppp p....... ........
        ^ ......         .. ........ ........
        ^ ...kkkkk kkk..... ........ ........
        ^ 33333333 ........ ........ ........
        ^ ttttt... ........ ........ .....ttt
        ^ ss...... ........ ........ ..ssssss
        ^ ........ ........ .......m mmmmmmm.
    
    If your message has the wrong hash in bit-index 0, which bit(s) of your message can you change to fix that?
    Is one of those bits in a part of your message that is not your computing ID?
    If not, how do you fix that?
    
    Is you message long enough that there is a non-compid byte for every bit of the hash?

Hash 6
:   What part of a byte controls the rotation size?
    What part contributes to the hash?
    What rotation size would make your life easiest?
    What are several characters that rotate that much?
    In what bits do those characters differ?
    Can you add a mix of those characters to the end of your string to get the hash you want?

