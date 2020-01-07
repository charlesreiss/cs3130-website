---
title: Buses (and networks)
...

In COA1, you learned about transistors and gates connected by wires.
These wires could have multiple destinations, but only one source.
The single-source design limitation is important for efficient transistor-level design,
but makes extending a system by adding new components challenging if not impossible.

# Channel characterization

This writeup provides a brief description of a several connection types,
organized from simplest to most flexible.

Some general terminology:

Channel
:   A medium capable to transmitting information.
    Wires, fiber optic cables, and radio frequency bands are examples of channels.

Signal
:   Information conveyed through a channel.
    Voltage changes, photons, and sound waves are examples of signals.

Message
:   Data conveyed via signals on a channel.
    `1` and `0`, songs, text, and pictures are examples of messages.

Sender
:   A party or device converting messages into a signal and placing it on a channel.

Receiver
:   A party or device noticing a signal on a channel and converting it back into messages.

## Simplex

In simplex communication, information only flows in one direction:
receivers are not also senders.
Wires between transistors and gates are simplex.
So are broadcast radio, baby monitors, surveillance cameras, and streaming video.

Simplex communications generally require separate designs
for the sender and receiver.
Most simplex communication systems allow a message to be delivered to many receivers via the same signal.

If multiple parties try to send information on a simplex channel at the same time,
a **collision** occurs.
This is almost always bad, varying from confusion (as when two radio stations overlap)
to destruction (as when two transistors connect to the same output wire, creating a short and burning the chip).
Because simplex systems have no way to send back-and-forth data,
collision is difficult to avoid if multiple sources exit.


## Half-duplex

In a half-duplex system,
every sender is also a receiver, but only one may place signal on the channel at a time.
Conversations can be seen as such a system: sound travels though shared air,
but if both speak at once neither understand the other.

Half-duplex systems typically require some type of protocol
for resolving who gets to use the channel when and for how long.
Because that protocol may mean that a signal may not be placed in the channel at the moment it is wanted,
there is often a need to queue messages until the channel is available for sending.

## Full-duplex

In a full-duplex system,
every sender is also a receiver, and every party may send at the same time
without messages being lost.
Voting by raised hands can be seen as such a system: one person's raised hand
does not prevent another person's lowered hand from being simultaneously visible,
and each can both send and receive information simultaneously.

Full-duplex channels are generally implemented as one simplex system channel
per potential sender/receiver pair.
In the raised hands example, the line-of-sight channel between a given sender's hand and receiver's eyes form that simplex channel.
Because of this, full-duplex systems do not scale to an arbitrary number of parties.

Even if extremely large full-duplex channels can be created,
their efficacy is limited by the processing needed to receive many messages.
If instead of raising hands, everyone had a digital screen and flipped through text-heavy slides,
the limiting factor would be how many messages each receiver could process,
not the number of screen-to-eye simplex channels we could create.

## Duplexing

Duplexing, also known as full-duplex emulation, is the process of using a protocol
over a half-duplex channel to simulate full-duplex communication.
It always involves delays in message delivery,
but generally are designed to make it look like the messages are traveling simultaneously
but at a slower pace than the channel would be able to support in half-duplex mode.

There are many methods of duplexing possible.
Simply alternating bytes from each sender is simple to describe,
but more complicated approaches can be more efficient in some situations.

# Network topology

The way that channels are used to connect many components
can be characterized by its topology.
There are many named topologies
and many more unnamed or idiosyncratic topologies;
three topologies will suffice for this overview.

The channels used to connect components in a network are traditionally called **links**.

## Fully-connected

If every pair of components has a link between then,
the network is said to be *fully connected*.
For $n$ components, that means $\sum_{i=1}^n i = n(n-1)/2$ separate links.

While full-connected networks provided arbitrary communication,
they are expensive to construct at scale.
Adding an additional component to a fully-connected network
requires adding another link to each existing component,
which is often impractical.
Fully-connected networks are uncommon except in small networks
with a limited, unchanging number of nodes.

## Star

If there is one central component that is connected to every other component,
the network is said to be a *star*
and the component all others connect to is called the *hub*.
For $n$ components, that means $n-1$ separate links.

Start networks are simple to design, implement and extend.
However, they are highly dependent on the hub,
being limited by its capabilities and going down if it does down.

## Bus

If every component is attached to the same half-duplex channel,
the network is said to have a "bus topology"
and that single channel is called "the bus".
For $n$ components, that means $1$ link.

Buses are relatively simple to construct,
but need to handle the potential for collision in some way.
Adding new components to a bus is straightforward,
but the likelihood of collision increases as more components are attached to the channel,
so buses are good for situations when a small-but-variable number of components are to be connected.
Buses can be very efficient if each component desires to communicate rarely
but to have the message travel at speed,
or when many receivers want access to the same message.

# "The Bus"

Inside a computer's casing it is common to have several special-purpose networks
to connect various components of the system.
Because early implementations had a bus topology, these are all commonly called buses,
and in many cases referred to simply as "the bus."

The following are all often called simply "the bus":

Frontside bus
:   Connects the CPU, RAM, drives, GPUs, input devices, etc.
    
    Commonly implemented as one or more bus-topology channels.
    
    The best known example is the PCI^[This stands for "peripheral component interconnect" but is almost universally called by its acronym, so much so that I'd expct using the full name to confuse people.],
    which is used both for a deprecated standard and the family of subsequent standards
    including the current^[as of 2019] PCI-e standards.

Backside bus
:   Connects the CPU and the L2 caches.

    In a multicore setting, topologies vary;
    the number of links may be constrained by space and heat dissipation as well as connectivity.

Memory bus
:   Connects the CPU and RAM (or one of the caches and RAM).
    Often also has a third connection,
    a special bus-connecting component called a "bridge"
    that allows the memory bus and frontside bus to communicate.
    
    Commonly implemented as a single bus-topology channel.

The following are sometimes called buses, but not generally just "the bus":

Universal Serial Bus    
:   A technology for connecting removable peripheral components to computers.
    Despite the "bus" name, this is a pure [tree-topology network](https://en.wikipedia.org/wiki/Tree_network):
    pair-wise links with multiple star-like hubs.

SCSI
:   Pronounced "scuzzie", the Small Computer System Interface (SCSI)
    typically is used to connect drives (hard drives, optical drives, etc)
    to the front-side bus.
    Topologically, this is a "linear bus", a type of variable-length bus.

SATA
:   The Serial ATA^[ATA stands for "AT Attachment". AT is short for "IBM Personal Computer AT," the product which popularized this attachment technology. In that computer's name, AT stood for "advanced technology".]
    typically is used to connect drives (hard drives, optical drives, etc)
    to the front-side bus.
    Topologically, this is a point-to-point network with port multipliers;
    basically this means the maximal number of connections (or "ports") has to be decided in advance,
    but devices can be added that connect several other devices to a single port.
