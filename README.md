# UFO C implementation

This is a plain-vanilla C implementation of UFO. Currently it's written using the C11 standard.

The target domain of UFO is concurrent, networked, and distributed computing. It's not quite there yet.

## Building the UFO interpreter

```
$ python3 pmake.py
```

## Running example programs

```
$ bin/ufo
▌▐ ▛▘▛▜ UFO version 5-rc-1
▙▟ ▛ ▙▟ http://github.com/ufo-language
type :? for help

UFO> :l examples/hello.ufo
Hello, World!
UFO> 
```

## Why C?

Because what I find interesting is **not** what you can build using a bloated language that tries to give you everything, but with a small and clean language that gives you only what you need. And also there's a small SBC I would love to port it to.

## Why not C++?

Because C++ has become a Frankenstein's monster of a programming language and it really needs to die a horrible death. *With fire.*

That, and I've done it already in C++. I've also done it in Java, Python, Ada, Modula-3, Erlang, Scheme, and a few others.

## Caveat programmator

This version of UFO is not yet compliant with the docs, and probably also diverges a bit from it. I can't decide if I want to change the docs to fit the language, or change the language to fit the docs. I guess they're both sort of converging as I fill out the language.

## Current status

There's a REPL that lets you enter many of the data structures and a few of the expressions, and there are a few namespaces with primitive functions. Type `ns:global()` to see all the namespaces and their primitives (the output is really messy), or `hash:keys(ns:global())` to see just the namespace names. Type the name of a namespace to see the primitives defined in it.

## Features

* Automatic memory management (mark/sweep GC)
* Recursive evaluator that also provides proper tail call elimination (impressed? *you should be*)
* Expressions:
  * variable
  * if/then/else
  * do/end (sequence)
  * let (dynamic scope)
  * let/in (local scope)
  * abstraction & application (lexical scoping, recursion, tail call elimination (a.k.a. tail recursion))
  * exception throwing (exception catching not yet implemented)
  * async/end, cobegin/end, nondet/end for concurrency
* Data types:
  * integer, real, boolean, string
  * symbolic constant
  * array
  * singly-linked list (for realsies, not a "we call it a list but it's actually an array" type of data structure) (shakes fist at Guido)
  * finite map / hash table
  * set
  * queue
  * tuple (immutable array) (I might deprecate this type)
  * a few others
* Namespaces for primitive functions
* Plenty of undocumented features! (i.e., some are bugs, some are real features)

Also I'm thinking about combining the linked list & queue types into a doubly-linked list type. I can think of reasons to do it, and reasons not to do it. The fact that other languages do it that way doesn't automatically make it the right way to do it.

## CS quiz

What's so useful about an immuatable array (or any immutable data type, for that matter)? No fair peeking at my source code before you answer it. Hint: I said "CS quiz" and not "software engineering quiz".

## Development

* Written using Emacs. I won't get into a holy war over editors. The best editor is the one you prefer to use.
* No 3rd party dependencies. I wrote the lexer, parser, and unit test library.
  * I'll eventually add 3rd party libraries when I get to stuff like JSON and YAML. Now way I'm writing a YAML parser. (Update: but I did write a JSON parser.)
* Compiled using GCC.
* Built using a pmake.py program that I wrote. It does not use any sort of config file. It scans the source tree and automatically detects dependencies that need to be recompiled. It's faster and simpler than CMake.
* Unit tested using custom unit testing library written in C.
* Memory tested using valgrind.
* Running in Ubuntu 20 in VirtualBox on a Mac Mini.

## The future of UFO

I don't kid myself into thinking that there is any sort of future for UFO. This is a toy language that I work on for fun. I don't kid myself into thinking that anyone anywhere would want to use UFO for any reason.
