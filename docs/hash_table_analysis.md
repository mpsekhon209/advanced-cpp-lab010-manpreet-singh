# Hash Table Analysis Laboratory

## Core goals
Students must implement a hash table that stores key-value pairs using open addressing and quadratic probing.

## Required behaviors
Students must implement:


## Required analysis
Students should write a short comparison and complexity analysis of:


## Big-O expectations
Students should identify and explain the Big-Oh for:


Expected average-case: O(1) for insert, lookup, and update under a reasonable load factor; worst case can degrade to O(n) when many collisions occur or the table becomes badly loaded.
# Hash Table Analysis

## Design

The table stores string keys and integer values in a vector and uses open addressing. The hash function is 32-bit FNV-1a, reduced modulo the current capacity. On collision, the probe sequence checks `(hash + i^2) % capacity`. Capacity is kept below a 0.5 live-entry load factor and grows to the next prime greater than twice the old capacity. Prime capacities make the quadratic sequence cover a sufficiently large set of slots; the half-full threshold ensures an unused slot is available in that sequence.

Deletion marks an entry as a tombstone instead of making it empty. Searches continue across tombstones, while insertion remembers the first tombstone and reuses it only after scanning far enough to confirm the key is not already present. Rehashing reinserts live entries into a fresh table, removing tombstones and recalculating their indexes.

## Complexity

Let `n` be the number of live entries and `m` the capacity.

| Operation | Average | Worst case |
| --- | --- | --- |
| `insert` | O(1) amortized | O(n) probes, plus rehash |
| `contains` | O(1) | O(n) probes |
| `get` | O(1) | O(n) probes |
| `update` | O(1) | O(n) probes |
| `remove` | O(1) | O(n) probes |
| `rehash` | O(n) expected | O(n^2) with severe collisions |

The expected constant-time operations assume a well-distributed hash and a controlled load factor. A single rehash costs O(n), but geometric growth spreads that cost across many insertions, giving amortized O(1) insertion. Probing can degrade to O(n) when many keys collide. As load factor rises, open-addressed tables need longer probe sequences; tombstones can also make unsuccessful searches longer until a rehash removes them. This implementation grows before the live load factor exceeds 0.5.

## Collision Strategies and Trade-offs

Linear probing checks consecutive slots. It is simple and cache-friendly but creates primary clustering: long occupied runs attract more collisions. Quadratic probing spaces checks by squared offsets, reducing primary clustering, although keys with the same initial hash still follow the same sequence (secondary clustering). Double hashing uses a second hash to determine the step size and generally disperses colliding keys better, at the cost of another hash calculation and stricter requirements on its step size.

Open addressing stores entries in the table array itself, avoiding per-entry allocation and often improving cache locality. It requires careful tombstone handling and performance is sensitive to load factor. Chaining keeps a collection of entries at each bucket, tolerates high load factors and simple deletion, but requires extra pointers or container storage and has less predictable cache locality. With a poor hash function, either approach can degrade toward O(n) operations; with open addressing, clustering compounds that cost.
