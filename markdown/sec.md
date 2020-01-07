---
title: Security protocols with less math
...

Three basic primitives provide a great deal of modern security:
hashes, symmetric (or shared-key) ciphers, and asymmetric (or public-key) ciphers.
This writeup is designed to explain the concepts behind these tools
and how they are used to achieve common security objectives
without the algorithmic and mathematical complexity of current implementations.

# Three useful tools

## Hashes

A **hash function** takes an input of any size (often generalized as a byte stream)
and returns an output of a fixed size (often generalized as a large binary number)
such that changing any byte of the input would change the output too.

{.aside} The word "hash" is used to refer to (a) the output of this function, (b) the function itself, and (c) the action of running this function. It is grammatically correct to say "I hashed it with my favorite hash to get the following hash: `f4ca408dc68ca2776e5d294e8f1166b56ee5af9b`."

A **crytographically-secure hash function** is not feasibly invertible.
That is, given the output of the hash function and the code of the function itself, finding an input that would generate that output requires work equivalent to guessing random inputs until one happens to have the desired output.

{.example ...} The following code implements a hash function, in that any sized input will provide a fixed-size output

```c
uint hash(uint *arr, size_t len) {
    uint ans = 0;
    for(size_t i=0; i<len; i+=1) {
        ans = (ans << 1) ^ arr[i] ^ (ans>>31);
    }
    return ans;
}
```

However, that code it not even close to being cryptographically secure;
to get a desired output `x` all that is needed is to input an array containing only `x`.
{/}

It is very difficult to prove that a hash function is crytographically secure, and we have a long history of compromising one hash function (e.g., by figuring out how to rule out 99% of all inputs so we only have to guess 1% as many) and adopting another.

## Symmetric chiphers

A **symmetric cipher** or **symmetric-key cipher** is a pair of functions, one to encrypt and one to decrypt,
each taking two inputs: a message of arbitrary length and a limited-size *secret key*.
They should have the following properties:

- $e(m, k) = c \ne m$ -- that is, encryption results in a **ciphertext** which is unlike the original message
- $d(e(m, k), k) = m$ -- that is, decryption undoes encryption
- recovering $m$ from $e$ and $c$ without $k$ is no easier than trying every possible $k$ until one works

{.example ...} The following code implements a symmetric cipher, albeit not a secure one

```c
void e(uint *arr, size_t len, size_t key) {
    for(size_t i=0; i<len; i+=1)
        arr[i] += key;
}
void d(uint *arr, size_t len, size_t key) {
    for(size_t i=0; i<len; i+=1)
        arr[i] -= key;
}
```

Describing why this is insecure stems from the fact that messages have internal patterns. If I know that you are sending a PDF document, for example, I know that the first four bytes are always 0x25, 0x50, 0x44, 0x46; I can simply subtract that from the first four bytes of the ciphertext and recover the key.
{/}

Getting the first two properties is easy; getting the third is hard, contributing to a history of finding a weakness in one cipher and picking another to replace it.

## Asymmetric ciphers

An **asymmetric cipher** or **public-key cipher**^[Note that "public-key" is an overloaded term; for example, [Diffie-Hellman] does not use the type of asymmetric cipher described here, but is asymmetric in a different way and is also sometimes called a "public-key" protocol.] is a function that can both encrypt and decrypt.
However, keys come in pairs. If $f$ is an asymmetric cipher and $(k_1, k_2)$ is a key pair then

- $f(f(m, k_1), k_2) = m$ -- $k_2$ decrypts what $k_1$ encrypts
- $f(f(m, k_2), k_1) = m$ -- $k_1$ decrypts what $k_2$ encrypts
- $f(f(m, k_i), k_i) \ne m$ -- neither key decrypts what it encrypts

The cipher should also follow the security property:

- knowing $f$, $c$, and $k_1$ should not make determining $k_2$ or $m$ easier than brute-force guessing all possible $k_2$s.

In practice, only a limited set of asymmetric ciphers are known and they do not offer quite as good security as "no better than brute force," but they do have (near) proofs of difficulty to break *if*

- Certain generally-accepted but not-yet-proven theorems hold (notably "**P** ≠ **NP**" and "factorization ∉ **P**")
- You only encrypt small data (generally not to exceed the size of the keys)

{.example ...} The following code implements an asymmetric cipher, albeit not a secure one

```c
int f(int msg, int key) {
    return msg * key;
}
```

A key pair is a pair of integers that multiplies to give `1`,
which is possible because `int` has finite size;
for example `2501 * 3221654797 == 0x75400000001`,
which truncated to 32 bits is just 1; thus (2501, 3221654797) is a key pair for this function.

```c
int message = 0xdeadbeef;
int k1 = 2501, k2 = 3221654797;
printf("-> message: %x; ciphertext: %x; decrypted: %x\n",
    message, f(message, k1), f(f(message, k1), k2));
printf("<- message: %x; ciphertext: %x; decrypted: %x\n",
    message, f(message, k2), f(f(message, k2), k1));
```

displays

    -> message: deadbeef; ciphertext: 776a54eb; decrypted: deadbeef
    <- message: deadbeef; ciphertext: ba965523; decrypted: deadbeef

This cipher is not secure because the [extended Euclid's algorithm](https://en.wikipedia.org/wiki/Modular_multiplicative_inverse#Extended_Euclidean_algorithm) can efficiently derive $k_1$ from $k_2$.
{/}

Typically, keys are generated in pairs and one of them (chosen arbitrarily) is kept secret while the other one is shared publicly.
If you have my public key, you can use it to encrypt messages only I can decrypt (you can't even decrypt them yourself)
and to decrypt messages only I can encrypt.

Unlike [symmetric ciphers], of which there are many,
only a few asymmetric ciphers have ever been popular.
RSA^[RSA is short for Rivest-Shamir-Adleman, named after Ron Rivest, Adi Shamir, and Len Adleman, its inventors. It was previously invented by Clifford Cocks, but Cocks' document describing it was given a top-secret classification by the British government and not made public until 1997.]  is the most common by far,
so much so that "RSA" is sometimes used as a generic noun for all asymmetric ciphers.

## Diffie-Hellman

The **Diffie-Hellman key exchange**^[There is some difference of opinion about the justice of this name. Whitfield Diffie and Martin Hellman published a paper describing it in 1976, but Hellman later stated the idea in the paper was that of their graduate student Ralph Merkle.] is a method of causing two (or more) parties to agree on a single shared random integer without anyone else listening in being able to know what number they agreed upon.

The process requires identifying a "cyclic group" -- that is, a set of values (integers are preferred) and an operator on elements of that set such that $op(op(x, y), z) = op((x, z), y)$.
To be secure, the set should be large and the operation hard to undo (i.e., knowing both $x$ and $op(x,y)$ should not make it easy to determine $y$).

{.example ...} The following code implements a cyclic group (i.e., `f(f(x,y),z) == f(f(x,z),y)`{.c}, albeit not a secure one

```c
int f(int generator, int key) {
    return generator * key;
}
```

This is insecure for the same reason it was for asymmetric ciphers.
{/}

If two people want to create a shared secret, they

1. Agree (in public) on an initial random number $g$
2. Each (privately, without telling anyone at all) pick a second random number, their private key
3. Each applies the operator to $g$ and their key, sharing the result (in public) with the others
4. Each applies the operator to what they received and their key to attain their shared secret

person A knows       shared publicly            person B knows
-----------------   ----------------------  ------------------
                        $f$, $g$
$a$                                                        $b$
                    $f(g, a)$, $f(g, b)$
$f(f(g,b), a)$                                  $f(f(g,a), b)$

{.example ...}
Suppose you and I are communicating, using the (insecure) $f$ from the previous example.

1. One of us picks `0x3308006d` as $g$ and we both agree to use it
2. I pick `0x71563a35` as my key, compute `f(g, 0x71563a35) = 0xa25ec891`, and share `0xa25ec891`
2. You pick `0xd380203f` as your key, compute `f(g, 0xd380203f) = 0x9c85bad3`, and share `0x9c85bad3`
3. I use what you shared (`0x9c85bad3`) and my secret (`0x71563a35`) to compute `f(0x9c85bad3, 0x71563a35) = 0x99e57baf`
3. You use what I shared (`0xa25ec891`) and your secret (`0xd380203f`) to compute `f(0xa25ec891, 0xd380203f) = 0x99e57baf`
4. You and I now both know `0x99e57baf`, but all anyone listening in knows are `0x3308006d`, `0xa25ec891`, and `0x9c85bad3`.
    
    Because we picked and insecure operator $f$, this public information does allow others to figure out our secret keys via the extended Euclid's algorithm and thus learn our shared secret.
    If we had picked a better $f$ (an elliptic curve, perhaps) this would not have been feasible.
{/}


## Cryptographically-secure random numbers

Many security activities depend at some point on a random number being used.
These need to be random in the sense that there is no ability to guess them,
even if you know a great deal about the code that created them.

Most software "random number generators" are actually *pseudo*-random number generators,
meaning they are created by a deterministic subroutine with some internal state;
if you know the value of that state, you can predict the sequence of "random" numbers perfectly,
and observing a sequence of generated numbers is sufficient to re-construct the state.

Cryptographically-secure random number generators instead rely on some form of **entropy harvesting** to collect unpredictable bits from sources invisible to outsiders.
For example, as I type this writeup low-order bit of the microsecond at which I press each key may appear to be "pure entropy"---that is, without pattern or meaning.
We could likewise harvest the low-order bits of each mouse movement, CPU temperature reading, etc.
We don't actually know that such measurements are random; there might be a pattern we haven't noticed, and if so even secure algorithms may be breakable by attackers that know those patterns.
However, it's almost certainly more secure than using something entirely predictable
like the output of a pseudo-random generator seeded with the time of day.

Modern operating systems typically harvest likely sources of entropy and maintain an entropy pool.
Since entropy cannot be generated on demand, there is always risk that the entropy pool will not contain enough bytes for a needed cryptographic purpose.

{.example ...}
As explained in `man 4 urandom`{.bash}, Linux maintains an entropy pool in `/dev/random`.
It is designed to be read-once: if you read a byte from `/dev/random` there is one less byte there than there was before, and it is relatively easy to empty it entirely.

To avoid having cryptographic tools wait for more entropy to be generated,
Linux also provides `/dev/urandom`, a pseudo-random number generator
that uses harvested entropy to prevent itself becoming predictable.

As noted on the manual page, "The `/dev/random` interface is considered a legacy interface, and `/dev/urandom` is preferred and sufficient in all use cases, with the exception of applications which require randomness during early boot time." Explaining how the cryptographic security of `/dev/urandom` was established is beyond the scope of this course.
{/}


# Using the tools

## Signatures

Task
:   Verify that a document has been approved in its current form by a particular individual.

Solution
:   Have them encrypt a [hash](#hashes) of the document with their [private key](#asymmetric-ciphers);
    the encrypted hash is called their *signature*.
    
    To check the signature, 
    
    1. get their public key (possibly with a [certificate](#should-i-trust-you) if their key is not one you already know) 
    2. use it to decrypt the signature to get some value $x$
    3. hash the document yourself to get another value $y$
    4. if $x = y$, you have confidence that they signed the document


## Should I trust you?

Task
:   Convince someone you don't know that you are who you say you are.

Solution
:   1. Have someone who knows you both [sign](#signatures) a document saying "this person's public key is $X$".
    2. Give that document, called a *certificate*, to the skeptic who doubts your identity.
    3. The skeptic then generates a random number, encrypts it with your [public key](#asymmetric-ciphers), and tells you the encrypted result
    4. You decrypt it with your [private key](#asymmetric-ciphers) and tell the skeptic "it's really me: your number was $X$"
    
    The internet today uses a few well-known *certificate authorities*
    who build entire businesses on validating website identity and signing certificates.

A key insight about digital certificates
is that presenting the certificate is just half of the authentication process;
it must be followed with a challenge to verify you can use your private key,
and that private key is never given away.
It's a bit like (but much more secure than) an ID card with a picture of you on it:
you have to have your card (which others could steal)
*and* your face (which you never intentionally give to anyone)
to use it.

There are alternative ways of establishing trust without a centralized set of certificate authorities,
but the approach described here is dominant at the time of writing^[2019].

## Storing passwords

Task
:   I want the computer to recognize my password, but even the root account not to be able to learn what my password is.

Solution
:   Store only a [hash](#hashes) of the password.
    
    Note that a simple implementation of this makes possible exploits like [rainbow tables](https://en.wikipedia.org/wiki/Rainbow_table) where the hashes of a large number of plausible passwords are precomputed to compare against the hashes stored on a computer.
    One partial solution to this is to *salt* the password before hashing:
    we generate a random value, store it with the password, and include it and the password in the hash.

Passwords on the internet much less secure than this.
We hope servers store only hashes (though verifying this is hard),
but we still have to get the password to them to hash
so they typically are transmitted in their raw form.
Thus, if your browser is remembering your passwords,
it *cannot* be remembering only the hashes
because it needs to enter the actual passwords into web forms for you.
However, if you are good about using a different password for each site,
browser storage of them can be more secure than typing them manually
as it prevents you accidentally providing one site with another site's password.

A better approach to web passwords is to use a password manager.
This still has to store your passwords,
but it can encrypt them using a symmetric key derived from your "master" password,
which it stores only as a hash,
so that compromise of the password manager has limited loss.
It can also generate random virtually-unguessable passwords for each site
and continue to provide access to your passwords even if you are not on your usual browser.


## Let's talk privately

Task
:   Initialize private communication with a new acquaintance in a public place.

Solution
:   Since communication has patterns that can be used to bypass the security of public key cryptography, we need to establish a shared key for use with a [symmetric cipher](#symmetric-ciphers).
    Such a key is generally little more than a random number,
    so we can use [Diffie-Hellman] to generate it.
    
    Of course, first we need to agree on which symmetric cipher to use,
    and possibly share [certificates](#should-i-trust-you) to make sure we are who we say we are;
    and Diffie-Hellman involves several communication steps itself,
    so establishing this communication requires a multi-step back-and-forth protocol.
    One popular protocol for this is called [TLS](https://en.wikipedia.org/wiki/Transport_Layer_Security).


# Side channels

Having a secure protocol and entropy source is not sufficient to have a secure system.
Humans are often a weak point, picking bad passwords and sharing them too freely.
However, side channels are also a major possible weakness.

A **side channel** is something that can carry information, but was not the communication channel the designer of the system had in mind.
There are many side channels and may side channel attacks, but one example will suffice to show the complexity involved in anticipating and preventing them.

Suppose I want to learn the randomly-generated private key you used in [Diffie-Hellman].
I look through your code and consult the specs of your processor
and notice that it takes your code 2 microseconds to process each 0-bit and 3 microseconds to process each 1-bit.
There are a few other aspects that are hard for me to time perfectly,
like how long it takes me to see your messages,
but after watching a few hundred DH key exchanges I get a reasonable estimate on those too.

Now suppose for a particular 32-bit DH session I time your code and determine you have 13 1-bits in your key.
To brute-force guess your key, I only need to check the 347,373,600 32-bit numbers that have 13 1-bits, not all 4,294,967,296; a savings of approximately 12×.
