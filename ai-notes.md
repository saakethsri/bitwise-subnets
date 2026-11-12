# AI Exploration & Reflection

I used an AI chatbot to learn the networking side of this topic. I already knew C,
binary, and bitwise operators from COMP 211, but I only knew subnet masks from
memorizing them for my CompTIA A+ certification. I didn't know why they work.

Full chat: https://claude.ai/share/ead684bd-4ce4-40c6-a4a5-fdd2f34607b0

## Prompts I used

### 1. Starting point

> I'm taking COMP 211 right now, so I know C, binary representation, and basic
> bitwise operators (&, |, ~, <<, >>), but I don't know much about networking.
> How are IPv4 addresses and subnet masks actually represented under the hood?

**What I learned:** An IP address is just a 32-bit number. The dotted form
(192.168.1.37) is only for people to read. A subnet mask is another 32-bit
number: 1s over the network part, 0s over the host part. `/26` means the mask
starts with 26 ones.

### 2. Why unsigned?

> Why do networking programs in C typically store an IPv4 address in an
> unsigned int instead of a regular signed int or an array of four chars? What
> goes wrong with bit shifting if it's signed?

**What I learned:** Any address from 128.0.0.0 up has a 1 in the top bit. In a
signed `int`, that bit is the sign bit, so:
- `192 << 24` overflows a signed int, which is undefined behavior in C.
- Right-shifting a negative number fills in 1s instead of 0s.
- Comparisons put high addresses before low ones.

Unsigned avoids all of these. Real code uses `uint32_t` instead of plain
`unsigned int`, because C only guarantees `unsigned int` is at least 16 bits,
while `uint32_t` is exactly 32.

### 3. Packing and unpacking

> If an IP address is like 192.168.1.37, how do I pack those four separate
> numbers into a single 32-bit unsigned int in C, and then unpack them back out
> to print them?

**What I learned:** Shift each part into its own 8-bit slot and OR them
together. To get a part back out, shift it down and `& 0xFF`. OR works for
combining because each part only has 1s in its own slot, so nothing overlaps.

### 4. Broadcast address

> I get that ip & mask gives the network address because 1 & x = x and
> 0 & x = 0. How would I use bitwise operators to go the opposite direction and
> find the broadcast address (where all the host bits are set to 1)?

**What I learned:** `ip | ~mask`. AND and OR are mirror images:
- `x & 0` forces 0, `x & 1` keeps x
- `x | 0` keeps x, `x | 1` forces 1

`~mask` has 1s exactly over the host bits, so OR sets only those to 1.

### 5. Something that confused me

> Wait, when I tried ~mask in my head, I realized it flips all 32 bits,
> including the network bits. Why doesn't ip | ~mask ruin the network part of
> the IP address?

**What I learned:** `~` flips the bits of the *mask*, not the IP. The network
positions in `~mask` become 0, and `x | 0 = x`, so the IP's network bits pass
through unchanged. A mask works like a selector: AND clears bits where it is 0,
and OR sets bits where it is 1.

### 6. Slash notation and my example

> What does slash notation like /24 or /23 mean in binary? Can you walk me
> through an example where two IP addresses look like they're on different
> networks, like 192.168.0.200 and 192.168.1.5, and show how changing the mask
> from /24 to /23 changes the answer?

**What I learned:** The two addresses differ only in the last bit of the third
part (0 = `00000000`, 1 = `00000001`). A /24 mask checks that bit, so they're
on different networks. A /23 mask (`11111110` in the third part) does not, so
AND wipes it out and both become 192.168.0.0. Same network.

### 7. Checking my understanding

> Let me check if I understand how my computer decides where to send a packet:
> It takes its own IP and the destination IP, runs a bitwise AND with its
> subnet mask on both, and compares the two results. If they match, it sends
> the packet directly on the local network; if not, it sends it to the router.
> Is that right, or am I missing a step?

**What I learned:** That's the core idea. Two extra details:
- When the packet goes to the router, the destination IP doesn't change. Only
  the hardware (MAC) address on the outside of the packet changes.
- Real computers keep a routing table with several entries. Each entry uses the
  same AND-and-compare check, and the most specific match wins.

### 8. Explaining it to classmates

> I need to explain why a /23 subnet mask (255.255.254.0) groups 192.168.0.200
> and 192.168.1.5 together to classmates who haven't taken networking. What's
> the clearest way to visualize or explain why that single bit flip in 254
> (11111110) matters?

**What I learned:**
- A ZIP code analogy: compare fewer digits and you get bigger regions.
- A /23 mask rounds the third part down to an even number, so 0 and 1 both
  become 0.
- A good follow-up question: under /23, are 192.168.1.5 and 192.168.2.5 on the
  same network? No. 1 is `001` and 2 is `010`, so they differ in a bit /23
  still checks.

## Reflection

The AI helped most when I asked follow-up questions about things that didn't
make sense to me (prompt 5), or when I explained my own understanding and asked
if it was right (prompt 7). Those answers fixed gaps I didn't know I had.

It also connected what I already knew. For A+, I memorized that /24 means
255.255.255.0 and gives 254 hosts. Now I know why. A mask is just a bit
pattern, and the network address, broadcast address, and host count all come
from AND, OR, NOT, and powers of 2.

The AI isn't always right. In one answer it wrote `0xAC10_0000`, which is not
valid C, and it had to correct itself in the next reply. That reminded me to
check its answers myself instead of just trusting them.
