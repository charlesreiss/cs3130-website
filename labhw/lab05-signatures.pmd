---
title: Signatures lab
...

Your task in this lab is to submit signed strings stating that you have passed the lab.

# Signatures

A signature is a hash encrypted with a private key. It is checked by decrypting with a public key and comparing to the hash. That is,

> (message, signature) is valid iff hash(message) = decrypt(signature, publickey)

However, some decryption includes a modulus operation; if so, you need to compare the hash modulo the same divisor.


<div class="example">
If `decrypt` ends with `result %= 0x1234567` then a signature matches if `hash(message) % 0x1234567 == decrypt(signature)`

</div>


# This lab

We provide seven hash functions (none of them secure) and an (also insecure) public key decryption function in [signlab.c](files/signlab.c)^[Using ssh? Try `wget http://www.cs.virginia.edu/luther/COA2/S2020/files/signlab.c`{.bash}]. We also provide the following signed messages. For most messages we provide two signatures, one valid and the other invalid, listed in numerical order.

 Hash   Message              Sig1         Sig2
------  ------------------  ------       ------
0       `"mst3k passes"`    0x03294abe
1       `"mst3k passes"`    0x2839124b
2       `"mst3k passes"`    0x2561405e   0x29639450
3       `"mst3k passes"`    0x5867974a   0x5fe433ca   
4       `"mst3k passes"`    0x5669495c   0x63e1cc3f
5       `"mst3k passes"`    0x2bb17fcf   0x5217fffb
6       `"mst3k passes"`    0x0fb903ef   0x3f8170d7

Your task (alone or with partners) is to forge signed messages containing your computing ID(s). The recommended process is


<div class="exercise long">
For each hash in ascending order (the first is the easiest...)

a. check both signatures to find which is valid
b. forge a message that has the same hash
    - start with your computing ID(s)
    - add more characters to make the message have the hash you want
    - note: two messages with the same hash are called a "hash collision"
c. use the same signature with your new message
d. use the provided `submitSolution` function to submit your answer
e. check that it appeared on the [scoreboard](http://kytos.cs.virginia.edu:25012)

When you have finished all seven, or when lab is coming to a close, show them your work.

</div>


There are other processes that can work too, such as figuring out the private key from the insecure public key embedded in the `decrypt` function and writing your own encrypt function. Somewhere around hash 5 we expect that trying to find the private key might start being easier than finding a hash collision.

We don't care what else your message contains beyond your computing ID(s), so long as it does not contain a `'\0'` character and does not contain the word "fail" anywhere inside it. `"SDAF5YETmst3kDSGS32445"` will be seen as passing off `mst3k` if properly signed. Note, though, that `"mst3kpasses"` won't: a computing ID can't have lower-case letters before or after it or it will confuse out compid locator.

# Understanding `signlab.c`

Our code uses integer datatype type names from `stdint.h` to ensure they have exactly the same bit length on every OS.

Our provided C file has 

- A working public key decryption algorithm, `decrypt`. The keymod is the big hexadecimal constant in that function.

- A copy of each hash function.
    - 0 and 1 use only a substring of the message
    - 2 xors bytes. Recall that xor is its own inverse; that is `x ^ y ^ x == y`. Thus, if your xor result is `x` and you want it to be `y`, xoring in another `x` and a `y` will fix that.
    - 3 adds bytes. Recall that subtraction is an inverse of addition, but so is two's complement addition. If your sum is 13 too big, adding in a `256-13`-byte will fix that.
    - 4 xors 4-byte words, sometimes ignoring a few bytes at the end.
    - 5 xors bytes but also rotates the old result 3 places to the left with each new byte encountered.
        Bit rotation is like bit shifting, except that excess bytes wrap arond instead of being dropped.
    - 6 xors bytes but also rotates the old result a number of places determiend by the low-order 5 bits of the answer.

- A `submitSolution` function that sends your proposed solution to the server. Technically this is the only function you need; the others are just there for your reference.

- A `main` function that shows you how you might use a few of the other functions. Feel free to edit this however you wish.

# Other tips

We've seen the printable ASCII table before:

<div style="columns: 3">

	Hex 	Char
----------  --------------
 	09      TAB (`\t`)
	0A 		LF  (`\n`)
	0D 		CR  (`\r`)
	20 	    Space
	21 	    `!`
	22 	    `"`
	23 	    `#`
	24 	    `$`
	25 	    `%`
	26 	    `&`
	27 	    `'`
	28 	    `(`
	29 	    `)`
	2A 	    `*`
	2B 	    `+`
	2C 	    `,`
	2D 	    `-`
	2E 	    `.`
	2F 	    `/`
	30 	    `0`
	31 	    `1`
	32 	    `2`
	33 	    `3`
	34 	    `4`
	35 	    `5`
	36 	    `6`
	37 	    `7`
	38 	    `8`
	39 	    `9`
	3A 	    `:`
	3B 	    `;`
	3C 	    `<`
	3D 	    `=`
	3E 	    `>`
	3F 	    `?`

	Hex 	Char
----------  --------------
	40 	    `@`
	41 	    `A`
	42 	    `B`
	43 	    `C`
	44 	    `D`
	45 	    `E`
	46 	    `F`
	47 	    `G`
	48 	    `H`
	49 	    `I`
	4A 	    `J`
	4B 	    `K`
	4C 	    `L`
	4D 	    `M`
	4E 	    `N`
	4F 	    `O`
	50 	    `P`
	51 	    `Q`
	52 	    `R`
	53 	    `S`
	54 	    `T`
	55 	    `U`
	56 	    `V`
	57 	    `W`
	58 	    `X`
	59 	    `Y`
	5A 	    `Z`
	5B 	    `[`
	5C 	    `\`
	5D 	    `]`
	5E 	    `^`
	5F 	    `_`

	Hex 	Char
----------  --------------
	60 	    <code>\`</code>
	61 	    `a`
	62 	    `b`
	63 	    `c`
 	64 	    `d`
 	65 	    `e`
 	66 	    `f`
 	67 	    `g`
 	68 	    `h`
 	69 	    `i`
 	6A 	    `j`
 	6B 	    `k`
 	6C 	    `l`
 	6D 	    `m`
 	6E 	    `n`
 	6F 	    `o`
 	70 	    `p`
 	71 	    `q`
 	72 	    `r`
 	73 	    `s`
 	74 	    `t`
 	75 	    `u`
 	76 	    `v`
 	77 	    `w`
 	78 	    `x`
 	79 	    `y`
 	7A 	    `z`
 	7B 	    `{`
 	7C 	    `|`
 	7D 	    `}`
 	7E 	    `~`

</div>

... but what about other bytes?
C has a special syntax for adding non-printable characters: the escape sequence `\x##`, where ## are two hexadecimal digits, will insert that byte directly into a string. Thus, `"\x256\x01"` is an array of 4 `char`s: 

- 37 (i.e., `0x25`)
- 54 (i.e., `0x36` = ASCII digit `6`)
- 1 (i.e., `0x01`), and
- 0 (because every string literal ends with a null byte).




# Passing off

To get full credit for this lab, explain to a TA the process you used to find a hash collision and/or forge a new signature for the most advanced hash you worked with. Note that "I wrote a loop to try 4 billion things and one worked" will *not* get full credit.

