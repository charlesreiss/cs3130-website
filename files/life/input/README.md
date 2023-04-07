These are some example input files, converted from the common "RLE" format for Game of Life patterns.

Most of these are taken from the jslife collection of patterns, available at http://entropymine.com/jason/life/

This README.md and much of the rest of this package was modeled after a C++ version of this assignment compiled by Charles Reiss; see <https://www.cs.virginia.edu/~cr4bd/4414/S2019/life.html>

For all of these, if you wish to know more about the patterns themselves, I recommend getting copy of the
(more featureful) Game of Life implementation Golly and downloading the original patterns.

They are:
*  input/tiny --- a very small pattern that oscilates every 2 iterations.

*  input/make-a --- stablizes into a "spaceship" (pattern that "moves" over several generations)
   after 120 iterations
    
   For example, notice the result of

      ./life 120 input/make-a serial-result

   is similar to
    
      ./life 140 input/make-a serial-result

   but shifted up. The initial state (which you can see by passing 0 as an argument)
   has a several "gliders" (a particular kind of 3x3 spaceship) that are carefully
   arranged to interact to form this larger "spaceship" pattern.

*  input/o0045-gun --- most of this pattern oscillates every 45 generations, it emits a stream of
   "gliders" (3x3 patterns that move in a direction), which would grow infinitely large if we had
   an infinitely large life implementation.

*  input/o0075 --- this pattern oscillates every 75 generations.
   
*  input/puf-qb-c3 --- this pattern moves arbitrary far rightward at a rate of one cell every three generations
    (if given an infinite board, this input file is only ~2k wide) and produces "exhaust" of a forever oscillating
    pattern.

*  input/23334m --- via http://conwaylife.com/wiki/23334M , this pattern takes 23334 generations to stablize.
