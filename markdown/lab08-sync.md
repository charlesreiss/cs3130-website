---
title: Synchonization Experience
...

The purpose of this lab is to provide you with some experiencing seeing how various synchronization primitives play out in practice. It will take the form of playing four versions of a simple competitive online game.

# Getting with a group

Lab will open with you signing into zoom (link will be added here).
The TAs will then split you into zoom breakout rooms of 3--5 people each.
Zoom's toolchain appears to limit us to making these assignments randomly.

Expecting that many of your are feeling somewhat isolated and in need of conversation, we encourage you to spend several minutes in small-talk with the other members of your group before other activities begin.

You will then go through the four game variants together.

# The Game

You'll be presented with 

- a dollar amount you are trying to reach. Each player has a different target amount, all within $20 of one another.
- a dollar amount that is "on the table"
- a set of buttons for adding or removing money in increments of $1, $5, $10, and $20

Your goal is to reach your target dollar amount before anyone else you are playing with does so.

# The Variants

1.  Variant 1 (<https://kytos.cs.virginia.edu/coa2/cashgame1.php>) 
    has atomic money moves and no other synchronization.
    It is possible (even likely) that other players' plays will occur between you looking at the table and your play being acted on.

1.  Variant 2 (<https://kytos.cs.virginia.edu/coa2/cashgame2.php>)
    uses locks to prevent plays if someone else has made a play between your looking and acting.

1.  Variant 3 (<https://kytos.cs.virginia.edu/coa2/cashgame3.php>)
    places arriving players in a queue and only lets them play in order.
    It is otherwise like Variant 1.

1.  Variant 4 (<https://kytos.cs.virginia.edu/coa2/cashgame4.php>)
    uses voting to make plays; only if a majority of players agree to an action does it occur.

Each variant allows arbitrary game names.
Your group should

1. make a google doc all group members can edit. All members should add their computing IDs and names to the document.
1. for each variant
    a. repeat until you agree on how the variant feels in play
        i. pick a name
        i. all enter that game
        i. play until someone wins (we encourage conversation during play)
    a. write a review for that variant in the doc
1. create a synthesis review, including which game is "best"
1. outline a fifth variant you think might be worth trying in the future

# Passing Off

TAs will roam the breakout rooms and observe play. They will also look over your review document.
