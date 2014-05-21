There is a misleading [video](https://www.youtube.com/watch?v=YQs6IC-vgmo) that seems show up on reddit every so often. In it Bjarne Stroustrup extolls the virtues of vectors while denouncing linked lists. He shows that for a given task vectors are much faster than linked lists even though in theory linked lists should do very well. The problem with his approach is that he takes an operation that linked lists are very good at (insert/delete) and then substitutes a different operation to benchmark (linear search + insert/delete). He then goes on to talk about caches and memory use. There is an implication that vector somehow overcomes the superior runtime complexity of list due to its own superior constant factors. This is completely false, both have linear complexity here. If this implication was not meant to be made then why mention that list is really good at insertion and deletion?

It's also misleading to talk about caches because you usually dont have to give up cache efficiency to acheive better runtime complexity. Lists trade away constant time indexing to get constant time insert/delete. They then give up cache efficiency for iterator/reference stability.  If both of these properties are needed for your problem, then lists may be for you. If not, there are probably better datastructures available. But let's not let vector off the hook so easily, while it may have great constant factors, it's *also* terrible for this problem due to its linear per operation behaviour.

To show that runtime complexity and cache efficiency are orthoganal, I have selected a problem similar to what Bjarne used and benchmarked it against 4 different data structures. I use std::vector and std::list from the standard library and also 2 non-standard classes. First I use [avl_array](http://avl-array.sourceforge.net/), which is an avl tree that offers logarithmic insert at an arbitrary index as well stable iterators but poor cache efficiency. Next I use a b+tree class that I wrote which offers logarithmic insertion at an arbitrary index as well as very good cache efficiency but can not provide stable iterators. Below is a table showing some of the properties of each data structure.

                | **cache innefficient** | **cache efficient** |
----------------|:----------------------:|:-------------------:|
**linear**      | list                   | vector              |
**logarithmic** | avl_array              | btree_array         |


The task at hand is this: Insert the numbers 0 - N into random positions in a sequence. Below are the timings for different values of N.

              | **list**      | **vector**    | **avl_array**   | **btree_array** |
--------------|--------------:|--------------:|----------------:|----------------:|
**10**        | 0.000941370   | 0.000934011   | 0.000938341     | 0.000940762     |
**100**       | 0.001084006   | 0.001096881   | 0.001098301     | 0.001182809     |
**1000**      | 0.001769533   | 0.001056606   | 0.001232885     | 0.000994284     |
**10000**     | 0.379775849   | 0.009943421   | 0.005019796     | 0.003015072     |
**100000**    | 51.701910351  | 0.970821829   | 0.060822126     | 0.015336007     |
**1000000**   | *out of time* | 155.485687811 | 1.271527184     | 0.187348144     |
**10000000**  | *out of time* | *out of time* | 22.117999131    | 3.400824379     |
**100000000** | *out of time* | *out of time* | *out of memory* | 57.620256751    |

You can see that avl_array quickly overtakes vector despite its poor cache behaviour and btree_array scales without sacraficing cache efficiency.

If you would like to run the benchmarks yourself, clone this repository and type "make run"
