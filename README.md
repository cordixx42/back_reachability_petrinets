# Backwards Reachability in Petri Nets Using Weakly-Acyclic DFAs
This repository contains the implementation part of my Bachelor Thesis on Solving the Backwards Reachability Problem in Petri nets using Finite Automata. The main functionalities can be found in ```sourcecode/src/impl```. 

**Abstract** \
*The coverability problem of Petri nets plays a crucial role in analyzing and arguing about
distributed and concurrent systems over various application areas, including parallel programs,
biological models, and business processes. Nevertheless, its EXPSPACE-complete nature
presents a challenge when confronted with practical scenarios.
This thesis presents a novel approach to the coverability problem, which utilizes a class
of formal language called weakly acyclic. We design a data structure for minimal weakly
acyclic DFAs, this can be seen as a generalization of binary decision diagrams. Subsequently,
we integrate this data structure into the Backwards Reachability Algorithm, an existing
high-level procedure for solving the coverability problem.
We perform benchmarks on real-life instances, which show that our implementation is
competitive with prevailing coverability tools and that the representation with weakly acyclic
languages provides a compact finite description of configurations in Petri nets.*
