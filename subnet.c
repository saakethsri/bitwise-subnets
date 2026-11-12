#include <stdio.h>

// Combine the 4 parts of an IP address (like 192.168.1.37)
// into one 32-bit number. Each part is 8 bits (0 to 255).
unsigned int make_ip(unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
    return (a << 24) | (b << 16) | (c << 8) | d;
}

// Print a 32-bit number in binary, with a space after every 8 bits.
void print_binary(unsigned int x) {
    for (int i = 31; i >= 0; i--) {
        unsigned int bit = (x >> i) & 1;
        printf("%u", bit);
        if (i % 8 == 0 && i != 0) {
            printf(" ");
        }
    }
    printf("\n");
}

// Print a 32-bit number as a normal IP address, like 192.168.1.0
// (the reverse of make_ip: pull each 8-bit part back out).
void print_ip(unsigned int x) {
    unsigned int a = (x >> 24) & 255;
    unsigned int b = (x >> 16) & 255;
    unsigned int c = (x >> 8) & 255;
    unsigned int d = x & 255;
    printf("%u.%u.%u.%u", a, b, c, d);
}

// Two IPs are on the same network if they have the same network address.
// Returns 1 (yes) or 0 (no).
int same_network(unsigned int ip1, unsigned int ip2, unsigned int mask) {
    return (ip1 & mask) == (ip2 & mask);
}

// Print whether two IPs are on the same network, and show why.
void check(unsigned int ip1, unsigned int ip2, unsigned int mask) {
    print_ip(ip1);
    printf(" and ");
    print_ip(ip2);
    printf(" with mask ");
    print_ip(mask);
    printf("\n");

    printf("  network of first:  ");
    print_ip(ip1 & mask);
    printf("\n  network of second: ");
    print_ip(ip2 & mask);
    printf("\n");

    if (same_network(ip1, ip2, mask)) {
        printf("  -> SAME network\n\n");
    } else {
        printf("  -> DIFFERENT networks\n\n");
    }
}

int main(void) {
    unsigned int ip = make_ip(192, 168, 1, 37);
    unsigned int mask = make_ip(255, 255, 255, 0);

    // The key step: AND keeps the network bits and zeroes out the rest.
    unsigned int network = ip & mask;

    // ~ flips every bit of the mask, then OR sets all the host bits to 1.
    unsigned int broadcast = ip | ~mask;

    printf("IP:         ");
    print_binary(ip);
    printf("Mask:       ");
    print_binary(mask);
    printf("~Mask:      ");
    print_binary(~mask);
    printf("Network:    ");
    print_binary(network);
    printf("Broadcast:  ");
    print_binary(broadcast);

    printf("\nNetwork address:   ");
    print_ip(network);
    printf("\nBroadcast address: ");
    print_ip(broadcast);
    printf("\n");

    // Same-network checks
    printf("\n--- Same network? ---\n\n");

    unsigned int mask24 = make_ip(255, 255, 255, 0);   // /24: 24 ones
    unsigned int mask23 = make_ip(255, 255, 254, 0);   // /23: 23 ones

    // Easy case: only the last part is different
    check(make_ip(192, 168, 1, 37), make_ip(192, 168, 1, 99), mask24);

    // Tricky case: same two IPs, different masks
    check(make_ip(192, 168, 0, 200), make_ip(192, 168, 1, 5), mask24);
    check(make_ip(192, 168, 0, 200), make_ip(192, 168, 1, 5), mask23);

    return 0;
}
