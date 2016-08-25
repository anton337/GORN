# GORN  GORN - is a distributed Hash Map. The system uses the Map-Reduce methodology. The mappers split the input data based by performing a distributed merge sort on the hash key prefixes. Reducers perform low level sorts on mapper output, and store the sorted key-value pairs into distributed bins. To ensure that the system is scalable, the Chord network topology is used to connect the network.   Definition of Terms:    Hash Map – a data structure that associates elements to bins. The index of these bins is calculated using a hashing function. An appropriate choice of hashing function can asymptotically improve lookup efficiency relative to alternative data structures, such as search trees. The hashing function must minimize then number of bin collisions, and effectively resolve collisions when they occur. Two popular strategies for collision resolution exist, namely chaining and probing. Chaining associates a linked list with each bin, and appends colliding elements into this list. The probing collision strategy searches for nearby empty bins to store the element there. Hash Maps can achieve O(1) lookup complexity at the expense of space requirements.     Search Tree – a data structure that facilitates efficient lookup. Common types of search trees are the Red-Black search trees and AVL search trees. These data structures achieve logarithmic lookup complexity O(log n). Search Trees manage memory more efficiently than hash maps, however for large data sets, hash maps can provide significantly more efficient lookup speeds.     Chord – network topology that connects every node to log(n) adjacent nodes, such that the travel distance from any node in the network to any other is log(n) number of steps. Suppose, we order each node in a circle, then traveling clockwise, one node will be connected to its neighbor, as well as its second neighbor, as well as its fourth neighbor, etc…. This connection topology forms the binary Chord.  Implementation Details:    This implementation of a data structure attempts to improve the amortized efficiency of element storage and search operations by performing them in bulk. That is, if we store thousands of elements at a time, we are hoping to improve the average cost of storing them one element at a time. This system uses elements of hash maps as well as search trees to achieve acceptable storage and search efficiency. For example, each node in the Chord network contains a bin of data. To store data, we must send it from an arbitrary node accessed by the client to the node that has a bin assigned to the data. Chord traversal is O(log n), and sending data from its source to its destination along a Chord is in some ways analogous to a search tree operation. Each bin, however is a hash map with relatively few hash bins. Collisions are resolved using the chaining strategy, where colliding elements are stored in a sorted queue. The sorted queues can be relatively large, however search and insertions operations on sorted queues are performed in bulk using a merge sort strategy. Sorted queues are assigned a size that is sufficiently large to take advantage of memory cashing, but sufficiently small to fit in RAM. Specifically, we try to keep sorted queues under 2^14 bytes (approx. 16 Kb). If this size is exceeded, the queue is split using a 2-radix strategy.   Web Crawler:    The source of data that is stored in the distributed hash map, is a web crawler. This is essentially a collection of independent bots that send network requests to web urls, parse the responses for links, and store the links uniquely in the hash map. To avoid duplication of effort, only links that do not already exist in the hash map are parsed. The hash map itself represents the explored volume of the internet. A different data structure is used to store links that are yet to beexplored (the horizon).   The Distributed Producer Consumer Queue:    A producer-consumer queue is a thread safe data structure that allows for concurrent storage and retrieval of data. The distributed producer-consumer balances the data across a cluster, such that multiple consumers distributed across the network can acquire data in parallel. Also, producers across the network have access to push data into the queue. This is achieved by partitioning the data into a collection of files. The files are redistributed across the network in an attempt to balancedata availability. The underlying assumption is that the order in which data is pushed into the queue is not preserved when the data is retrieved. 