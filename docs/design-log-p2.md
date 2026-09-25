# ECE 309 Project 2 Design Log

## Conversation Growth Strategy

For `Conversation`, I used a dynamically allocated raw array with a growth factor of 2, starting with a capacity of 2 when the first element is inserted. When `size_` reaches `capacity_`, the implementation allocates a new array with twice the previous capacity, moves the `Message` objects into the new array, deletes the old array, and updates the pointer and capacity. Doubling keeps reallocations infrequent while providing space for future appends.

A single append that causes reallocation is O(n), because every existing element has to be moved. However, append is amortized O(1). The capacities grow as 2, 4, 8, 16, and so on. The total number of elements moved during all reallocations through n insertions is bounded by a geometric series such as 2 + 4 + 8 + ... + n, which is less than 2n. Therefore, the total reallocation work across n appends is O(n). Spread over n operations, the average cost is O(1) per append. This is why doubling is preferable to increasing capacity by a fixed amount, which would require O(n^2) total copying.

## Rule of Five and Memory Safety

`Conversation` owns its dynamically allocated `Message` array, making ownership clear. The destructor releases the array with `delete[]`. The copy constructor creates a separate allocation and copies every stored message, so the copied conversation does not share the original's buffer. Copy assignment uses the copy-and-swap approach: it first creates a temporary deep copy and then swaps the data, size, and capacity with the current object. This also handles self-assignment safely.

The move constructor transfers the existing pointer, size, and capacity instead of copying each message. The move assignment operator first releases the destination's current buffer, then takes ownership of the source buffer. Both move operations reset the source pointer to `nullptr` and its size and capacity to zero, leaving it valid and empty. `at()` checks its index against `size_` and throws `std::out_of_range` when the index is invalid.

`Message` also has the required default constructor, which creates an empty System message. When a System message is appended to a conversation, the existing messages are shifted right so the System message remains at index zero.

## SentinelScanner Bounded Memory

`SentinelScanner` stores the sentinel itself, a bounded `pending_` string, and a `found_` flag. Each call to `feed()` combines the pending characters with the new chunk and searches for the sentinel. If the sentinel is found, only the text before it is returned as safe output, and the scanner records that the sentinel has been found. Later calls do not emit additional text.

If the sentinel is not found, the scanner keeps only the final `m - 1` characters, where `m` is the sentinel length. Those characters are the only portion that could potentially become the beginning of a sentinel when the next chunk arrives. Everything before them is guaranteed to be safe and can be emitted immediately. Therefore, `pending_` never exceeds `m - 1` characters, regardless of the total amount of streamed input. This gives bounded auxiliary memory with respect to stream length.

## Hindsight

In hindsight, I would consider using a Knuth–Morris–Pratt style search for the sentinel. The current implementation satisfies the required bounded-memory behavior and handles arbitrary chunk boundaries, including a sentinel arriving one character at a time. However, KMP could reduce repeated comparisons for adversarial input containing many partial matches. I chose the current approach because it is straightforward and demonstrates the pending-buffer requirement.

## Testing

My tests cover the required behaviors. I test empty conversations, default and parameterized messages, normal appending, System-message ordering, bounds checking, growth through 100 messages, copy construction, copy assignment, move construction, and move assignment. The scanner tests cover clean text, a complete sentinel, every possible split point of the sentinel-containing string, one-character-at-a-time streaming, false alarms, and flushing pending text. These tests target the important boundary conditions emphasized by the project specification.
