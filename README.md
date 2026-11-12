# Bitwise Operators in Action: IP Addresses and Subnet Masks

COMP 211 "Applications of Bitwise Operators" presentation by Saaketh Srikakolapu.

## The question

Every time your laptop sends something over the internet, it has to answer one
question: **is the device I'm talking to on my own network, or somewhere else?**

If it's on the same network, the laptop sends it directly. If not, it sends it to
the router. Computers answer this with **one bitwise AND**.

## Background

### An IP address is just a 32-bit number

An IPv4 address like `192.168.1.37` looks like four numbers, but each one is
8 bits (0 to 255). Put them side by side and you get one 32-bit number:

```
192      168      1        37
11000000 10101000 00000001 00100101
```

In C, we build it with shifts and OR, sliding each part into its own 8-bit slot:

```c
unsigned int make_ip(unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
    return (a << 24) | (b << 16) | (c << 8) | d;
}

unsigned int ip = make_ip(192, 168, 1, 37);
```

We use `unsigned int` because it is exactly 32 bits and has no sign bit. The top
bit of an IP is part of the address, not a minus sign.

### A subnet mask splits the address into two parts

A subnet mask is also 32 bits: a row of 1s followed by a row of 0s.

```
255.255.255.0  =  11111111 11111111 11111111 00000000
                  |------- network part ------| host |
```

- **1s** cover the **network part**: which network you're on.
- **0s** cover the **host part**: which device on that network.

The short way to write a mask is to count the 1s: `255.255.255.0` is **/24**.

## What I explored

I wrote a C program, [`subnet.c`](subnet.c), that does what a computer does with
a mask:

1. Find the **network address** with `ip & mask`
2. Find the **broadcast address** with `ip | ~mask`
3. Check if **two IPs are on the same network**

## Key discoveries

### 1. AND with the mask gives the network address

AND gives 1 only when both bits are 1. So the mask works like a stencil:

- where the mask is **1**, the IP's bit is kept (`bit & 1 = bit`)
- where the mask is **0**, the bit is wiped to 0 (`bit & 0 = 0`)

```
IP:       11000000 10101000 00000001 00100101   192.168.1.37
Mask:     11111111 11111111 11111111 00000000   255.255.255.0
          ----------------------------------- AND
Network:  11000000 10101000 00000001 00000000   192.168.1.0
```

One operation, and the host part is gone.

### 2. Network and broadcast are mirror images

The **broadcast address** ("send to everyone on this network") is the same idea,
flipped. `~mask` flips every bit, so its 1s cover the host part. Then OR forces
those bits to 1:

```
IP:         11000000 10101000 00000001 00100101   192.168.1.37
~Mask:      00000000 00000000 00000000 11111111
            ----------------------------------- OR
Broadcast:  11000000 10101000 00000001 11111111   192.168.1.255
```

| | Operation | Host bits become | Result |
|---|---|---|---|
| Network address | `ip & mask` | all 0s | `192.168.1.0` (first address) |
| Broadcast address | `ip \| ~mask` | all 1s | `192.168.1.255` (last address) |

Every device on the network fits between these two. Because those two addresses
are reserved, the number of usable hosts is **2^(host bits) − 2**. For /24 that's
2⁸ − 2 = **254**.

### 3. Changing the mask by one bit changes the answer

Two IPs are on the same network if they have the same network address:

```c
(ip1 & mask) == (ip2 & mask)
```

**Are `192.168.0.200` and `192.168.1.5` on the same network?** It depends on the
mask. Look at the third part:

```
0           = 00000000
1           = 00000001
/24 mask    = 11111111   keeps the last bit  ->  0 vs 1  ->  DIFFERENT
/23 mask    = 11111110   wipes the last bit  ->  0 vs 0  ->  SAME
```

`255.255.254.0` (**/23**) has one fewer 1, so one more bit belongs to the host
part. That doubles the network size:

| | /24 | /23 |
|---|---|---|
| Mask | `255.255.255.0` | `255.255.254.0` |
| Host bits | 8 | 9 |
| Usable hosts | 2⁸ − 2 = 254 | 2⁹ − 2 = 510 |
| Network for `192.168.0.200` | `192.168.0.0` – `192.168.0.255` | `192.168.0.0` – `192.168.1.255` |

**A smaller slash number means a bigger network.**

## Takeaways

- An IP address is just a 32-bit number, so bitwise operators fit it naturally.
- A mask is a stencil: **AND** keeps the network part, **OR with `~mask`** fills
  the host part.
- Routers and computers check "same network?" millions of times a second, and
  it's one AND and one compare, which is very fast.
- Moving one bit between the network and host parts doubles or halves the
  network.

## How to run the code

Inside the learncli container, from this folder:

```bash
make
./subnet
```

Expected output:

```
IP:         11000000 10101000 00000001 00100101
Mask:       11111111 11111111 11111111 00000000
~Mask:      00000000 00000000 00000000 11111111
Network:    11000000 10101000 00000001 00000000
Broadcast:  11000000 10101000 00000001 11111111

Network address:   192.168.1.0
Broadcast address: 192.168.1.255

--- Same network? ---

192.168.1.37 and 192.168.1.99 with mask 255.255.255.0
  network of first:  192.168.1.0
  network of second: 192.168.1.0
  -> SAME network

192.168.0.200 and 192.168.1.5 with mask 255.255.255.0
  network of first:  192.168.0.0
  network of second: 192.168.1.0
  -> DIFFERENT networks

192.168.0.200 and 192.168.1.5 with mask 255.255.254.0
  network of first:  192.168.0.0
  network of second: 192.168.0.0
  -> SAME network
```

To try your own addresses, change the `make_ip(...)` values in `main`.

## Repo contents

| File | What it is |
|------|------------|
| [`subnet.c`](subnet.c) | C program: network address, broadcast address, same-network check |
| [`Makefile`](Makefile) | Builds the program |
| [`slides/`](slides/) | Presentation slides |
| `ai-notes.md` | Prompts I used with AI and what I learned (coming soon) |

## References

- [RFC 791: Internet Protocol](https://www.rfc-editor.org/rfc/rfc791): defines IPv4 addresses as 32-bit numbers
- [RFC 950: Internet Standard Subnetting Procedure](https://www.rfc-editor.org/rfc/rfc950): introduces subnet masks
- [RFC 4632: Classless Inter-domain Routing (CIDR)](https://www.rfc-editor.org/rfc/rfc4632): the `/24` style notation
- [RFC 1918: Address Allocation for Private Internets](https://www.rfc-editor.org/rfc/rfc1918): why home networks use `192.168.x.x`
- [cppreference: C bitwise operators](https://en.cppreference.com/w/c/language/operator_arithmetic): `&`, `|`, `~`, `<<`, `>>`
