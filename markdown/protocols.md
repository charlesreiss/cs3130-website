---
title: Networking Protocols
...

This document is designed to be a brief overview of several common network communication protocols, suitable for a day or two overview of a set of topics that we also teach entire courses about.
As such it glosses over or completely ignores many important topics.

# Packets with headers

In general, any piece of communication to be handled by intermediaries
needs two pieces of information:
the contents of the communication
and the routing information needed to reach the intended destination.

## Paper

For paper communication, these would be the letter and the envelope,
each made of paper and each with information written on them
but one (the letter) containing the contents
and the other (the envelope) containing the routing information.
We know which one is which by how they are arranged:
the envelope is outside the letter.

If we wanted to send an entire book we could invest in a different sending protocol
such as a box instead of an envelope,
but we could also take a few pages at a time and put each in an envelope.
As long as the pages are numbered, the recipient could reassemble the book
no matter what order the envelopes arrived in.
If we had a book with unnumbered pages, we could add the ordering to the envelope instead to still permit reassembling the book.

## Digital

Common network communications use many of the same ideas as paper post.

Data
:   The contents you want to send is the message or data.

Packet / Segment / Frame
:   Many protocols split the message into packets of some maximum (or in a few cases fixed) size,
    sending each separately with some sort of reassembly packet order numbers.
    
    The name for the chopped-up data depends on the [layer](#layers).

Header / Footer
:   To get information where it needs to go, a header with routing information needs to be provided first.
    It is followed by the packet of data to be transmitted.
    Some protocols add a footer as well to mark the end of the packet.

## Layers

Paper post is typically transmitted part of the way
by a letter inside an envelope inside a crate inside a truck;
and another part by a letter inside an envelope inside a mail pouch;
etc.
Each of these outer containers has routing information, like the envelope does:
the carrier route of the pouch, the post office destination of the crate, etc.

Digital communication also puts containers inside containers
and the set of containers used are called "layers"
There are different numbers of layers possible,
but [the OSI model](https://en.wikipedia.org/wiki/OSI_model#Description_of_OSI_layers) is often seen as the default set
even though [the Internet Protocol Suite](https://en.wikipedia.org/wiki/Internet_protocol_suite#Key_architectural_principles) is better known.
Note that OSI numbers its layers, where containers have smaller numbers than their contents.

{.example ...}
Consider using a `SOCK_STREAM` `socket`, as you did in [COA1](/luther/COA1/F2019/lab09-sockets.html).
When you `write` a message, the result is

- The message you sent over the socket, wrapped in
- a TCP header with the port number to get it to the right program, wrapped in
- an IP header with the IP address to get it to the right computer, wrapped in
- a IEEE 802.11 header and footer to get it over the Wireless LAN to the Internet

That last layer will be stripped off after traversing the WLAN, to be replaced by a different outer "envelope" to guide it over the next link on its way to its destination.
{/}

Note that because of the header/footer design,
every layer can splice the data provided by the other layers freely.
Thus, a message could be split into 4 TCP segments;
those each split into 2 IP packets to make 8 total packets.

# TCP/IP and friends

While there are many, many protocols out there,
three are pervasive enough to be worth ensuring every CS major has at least some understanding of them:
TCP, IP, and UDP.
These sit in the middle of the network layer stack:
they transmit higher-level protocols like HTTP, SMTP, SSH, etc;
and they are transmitted by lower-level protocols like wi-fi, ethernet, etc.

|Layer|Protocols|Use|Notes|
|-----|---------|---|-----|
|Application|HTTP, SSH, SMTP, [etc.](https://en.wikipedia.org/wiki/Category:Application_layer_protocols)|programmer-visible messages||
|Transport|**TCP**, **UDP**|reach correct program|there are [other transport layer protocols](https://en.wikipedia.org/wiki/Category:Transport_layer_protocols) too|
|Internet|**IP**, which has two versions: IPv4 and IPv6|reach correct computer|there are [other internet layer protocols](https://en.wikipedia.org/wiki/Category:Transport_layer_protocols) too|
|Link|MAC, Wi-Fi, [etc.](https://en.wikipedia.org/wiki/Category:Link_protocols)|traveling over wires and the air||


## IP

IP, or Internet Protocol, has two versions in use (IPv4 and IPv6).
There are some important technical updates in version 6,
and several other nuanced ideas this write-up ignores.

IP (both versions) works on the following principles:

- Every computer has a unique numeric "IP Address".

    IPv4 typically writes its 32-bit addresses as 4 8-bit decimal values with periods in between them, as in `255.127.63.31` for `0xFF7F3F1F`.

    IPv6 typically writes its 128-bit addresses as 8 16-bit hexadecimal values separated with colons in square brackets, with a run of all-zero values omitted, as in `[c0a:2::2020]` for `0x0c0a0002000000000000000000002020`.

    The fact that there are more than 2^32^ computers is part of the motivation to switch from IPv4 to IPv6.

- IP headers contain the IP address of both the originating and end-destination computer.

- Messages are delivered via "best-effort delivery".
    Each computer involved in delivering a packet
    sends it to the computer it has a link to
    that it believes is most likely to get the message where it is going,
    but there is no guarantee that it will arrive at all
    or that the sender will know if it arrived.

## UDP

UDP, or User Datagram Protocol, is a light-weight transport protocol.
It adds 16-bit source and destination port numbers
that the OS of the involved computers can use
to ensure it gets to the correct program;
and a checksum to detect errors that might arise during transmission.

UDP adds no other features on top of the underling IP:
messages are not guaranteed to arrive at all, nor in any particular order;
cannot be dynamically split into different sizes;
etc.
As such, it adds little if any delay on top of IP
and is used for speed-sensitive messages like clock synchronization
and streaming media.
Software that uses UDP generally has to be designed on the assumption
that some packets that are sent will never arrive.

## TCP

TCP, or Transmission Control Protocol, is a reliable, ordered, connection-oriented transport protocol.
It creates this over IP by a somewhat complicated state machine;
a core idea in this is that each received message must be acknowledged by the recipient and will be re-sent by the sender otherwise.

![Simplified TCP state diagram for how to establish and terminate a connection (from [Wikimedia](https://en.wikipedia.org/wiki/Transmission_Control_Protocol#/media/File:Tcp_state_diagram_fixed_new.svg))](https://upload.wikimedia.org/wikipedia/commons/f/f6/Tcp_state_diagram_fixed_new.svg)

{.example ...}
Suppose two computers, $P$ and $Q$, have established a TCP connection with one another.
$P$ might send $Q$ the message "This is a triumph!" as follows,
using (contents, sequence number) to represent a TCP segment:

------------------------------------------------------------------
    $P$                     IP               $Q$
------------------------- ------------ ---------------------------
sends ("This", 1)   

                            →

                                        receives ("This", 1)

                                        sends (ACK, 2)

                            ←

receives (ACK, 2)

sends (" is a ", 2)

                            (dropped)

sends ("triumph", 3)

                            →

                                        receives ("triumph", 3)

                                        expected message number 2, 
                                        not received

                                        resends (ACK, 2)

                            ←

receives (ACK, 2),
which means segment 2
never arrived...

resends (" is a ", 2)

                            →

resends ("triumph", 3)
as it came after
segment 2

                            →

                                        receives ("triumph", 3)

                                        receives (" is a ", 2)

                                        sends (ACK, 4)

                            ←

receives (ACK, 4)

sends (FIN) to close 
begin closing connection
------------------------------------------------------------------

In general, either side may re-send a message if they have not received the expected response, and neither needs to wait for the other's message to send the next.
This way if messages seem to be arriving well, more messages may be send in groups
while if acknowledgments are not arriving.
{/}

TCP is significantly slower than UDP,
requiring overhead to establish and shut down a connection,
but provides reliable delivery over unreliable IP.
The TCP over IP combination is so prevalent that "TCP/IP"
is sometimes used as a term for the entire Internet protocol suite.

# URLs

We are accustomed to navigating the web with Universal Resource Locators (URLs).
URL are a special type of URI, and have [many components](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Generic_syntax), but three will suffice for this introduction.

- A *scheme* like HTTP or HTTPS; this identifies the protocol used at the application layer.

- A *hostname*, which (conceptually) identifies a particular computer.

- A *path*, which is given to the computer to identify a specific resource.

Note that the hostname here performs a similar function as an IP address, but different audiences.
IP addresses are organized to help computers locate one another.
Hostnames are organized to help humans locate computers.

{.example ...}
In the URL <https://www.cs.virginia.edu/COA2>,

- The *scheme* is `https`
- The *hostname* is `www.cs.virginia.edu`
- the *path* is `/COA2`
{/}

## The value of a mapping

When you visit a URL, your browser first needs to convert the hostname in the URL
to the IP address of a computer so that it can use TCP/IP to route your request
to the appropriate server.
It does this by consulting a special mapping data structure,
which is maintained online and can be updated by the party owning the hostname.
That means it first queries the Internet for the IP address of the hostname
before querying it again with the website request itself.

Having such a distributed mapping between what users type and what computers do
allows many conveniences, such as the ability to change servers
without the need to have everyone learn the new server's IP addresses.
However, it also comes at a price:
someone has to decide who gets to change the IP address of `www.cs.virginia.edu`,
which means someone has to know who owns each address
and prevent others from taking it.
This work requires resources and human judgment calls,
meaning it requires money,
meaning it costs money to register a hostname.

## From files to DNS

In the earliest days of the ARPANET, the mapping between hostnames and IP addresses was a single file, `HOSTS.TXT`.
Each line contained a hostname and an IP address^[More than this, really; it also had several line types (`NET`, `HOST`, and `GATEWAY`) and some information about the type of computer it described].
Initially this was stored on one computer
and users learned its contents by calling up an operator via telephone
and asking the operator for the IP address of a given host.
Later these telephone calls were moved to a digital protocol,
`WHOIS`, but it remained initially on one computer
owned by one company, SRI.

As this directory grew in importance, SRI needed to decide who could have which hostnames.
[Elizabeth Feinler](https://en.wikipedia.org/wiki/Elizabeth_J._Feinler)
and her team managed WHOIS and created the idea of "domain names"
to help organize requests.
A domain name is a hierarchical sequence of strings separated with periods,
with the last string (called the "top-level domain") being the most important.
SRI decided that top-level domains would be allocated based to type of business,
with `.com` for commercial entities, `.edu` for educational, `.gov` for government, etc.

{.example ...}
In the hostname `www.cs.virginia.edu`,

- `edu` is the top-level domain
- `virginia.edu` is a subdomain of `edu`
- `cs.virginia.edu` is a subdomain of `virginia.edu`
- `www.cs.virginia.edu` is a subdomain of `www.virginia.edu`
- `edu`, `virginia`, `cs`, and `www` called *labels*

{/}

As the Internet grew and a single file on a single server became unwieldy,
various proposals were created for how to distribute the load.
After many iterations, this has grown into the current Domain Name System (DNS).

## DNS

DNS is a fairly complicated set of concepts,
but the iconic and most important core is the address resolution mechanism.

DNS address resolution uses two basic ideas:

1. Requests start at a top-level DNS server,
    who replies with the DNS server of the top-level domain;
    that DNS server is then asked about the next part of the domain,
    and so on until the full domain is handled.
    
1. Replies are cached (stored locally) so that each query may be sent just once.

{.example ...}
Generally when I ask my browser to visit `www.cs.virginia.edu`,
it just contacts `128.143.67.11` without any DNS queries because it has it in its cache.

But if there was no cache, the request would do something like the following:

--------------------------------------------------------------------------------
Request                 To               Reply
-------------------     ---------------  ---------------------------------------
`.`                     my ISP's DNS     a list of 13 known top-level-domain DNS servers;
                        server           we randomly select `202.12.27.33`.

`edu.`                  `202.12.27.33`   a list of 13 .edu-domain DNS servers;
                                         we randomly select `192.54.112.30`.

`virginia.edu.`         `192.54.112.30`  a list of 4 .virginia.edu-domain DNS servers;
                                         we randomly select `128.143.22.119`.

`cs.virginia.edu.`      `128.143.22.119` a list of 2 cs.virginia.edu-domain DNS servers;
                                         we randomly select `128.143.136.15`.

`www.cs.virginia.edu.`  `128.143.136.15` The IP address `128.143.67.11`
--------------------------------------------------------------------------------
{/}

There are many additional components to the DNS protocol,
including digital signatures to validate that they are not spoofed,
messages for registering new DNS entries and changing old ones,
[etc.](https://en.wikipedia.org/wiki/List_of_DNS_record_types)

As DNS has grown in complexity, DNS servers have grown into ever more-complicated database engines.
However, the core DNS lookup process has not changed since the first DNS specifications^[ [RFC 882](https://tools.ietf.org/html/rfc882) and [RFC 883](https://tools.ietf.org/html/rfc883)] were released in 1983.
