# Using KLEE for computing per-EC prob


## Install Klee

Klee can be easily installed, follow instruction [here](http://klee.github.io/).
```
mkdir build
cd build
cmake -DENABLE_SOLVER_STP=ON -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-6.0 -DENABLE_UNIT_TESTS=OFF -DENABLE_SYSTEM_TESTS=OFF ..
make
```

## The simple Blink example

`blink.c` is a very simple version of Blink, it uses a small hash table to maintain
per-connection state (normal or retransmitting). Once it detects > threshold
flows are retransmitting, it raises an alarm and reset all state.

To run this example, first install klee and run:
```
# compile
clang-6.0 -I <path_to_klee_include_folder> -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone blink.c
# or
clang-6.0 -I ../../include -emit-llvm -c -g -O0 -Xclang -disable-O0-optnone blink.c

# symbex
klee --use-query-log=all:kquery --write-kqueries --write-cvcs --write-smt2s blink.bc
```

Klee can figure out all possible paths - either raising alarms or not.

Doing: hack Klee and integrate LattE to compute per-path prob.

TODO: use ASSERT-P4's translation tool

## Hacking Blink to support per-path prob

`test_prob.c` is used for testing probablity symbex.

## Questions and Problems

1) This simple Blink scales very poorly - when pkt num > 5 it cannot finish within
mintues. One possible reason is reversing `hash` is too time-consuming.

TODO: integrate havocing? CASTAN's code is available in https://github.com/nal-epfl/castan.

By reading the ISSTA paper, their technique also does not scale to >4 iterations.
This seems to suggest that probabilistic symbex suffers from state exploision
issue!

TODO: Can we develop an approximate method for computing probabilities?
TODO: See ASE'14 paper: exact and approximate Prob. SE.
TODO: Also see the ICSE'16 paper (perfplotter).

2) In assence, hash tables are just an array, and to access this array the **index**
has to be symbolic (derived from flow ID), i.e., access `a[sym_index]`, where `a`
is an array, and `sym_index` is `hash(flow_id)`.
How does the SEE handle this? The memory location is not deterministic!

TODO: The KLEE paper doesn't talk about this, maybe read the EXE paper.

TODO: If this is some special kinds of contraints, we don know if LattE can
handle it.
