#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/// (insecure) public-key decryption function
uint32_t decrypt(uint32_t msg) {
    uint64_t result = 1;
    uint64_t m = msg % 0x694d12c5;
    uint32_t e = 17;
    while (e) {
        if (e&1) { result *= m; result %= 0x694d12c5; }
        e >>= 1;
        m *= m; m %= 0x694d12c5;
    }
    return result;
}

/// one-byte-based insecure hash function
uint32_t hash0(const char *s) { return s[0]; }
/// four-byte-based insecure hash function
uint32_t hash1(const char *s) { 
    uint32_t ans = 0;
    ans |= s[0];
    ans |= ((uint32_t)s[1])<<8;
    ans |= ((uint32_t)s[2])<<16;
    ans |= ((uint32_t)s[3])<<24;
    return ans;
}
/// xor-based insecure hash function
uint32_t hash2(const char *s) { 
    uint32_t ans = 0;
    while(*s) { ans ^= s[0]; s+=1; }
    return ans;
}
/// addition-based insecure hash function
uint32_t hash3(const char *s) { 
    uint32_t ans = 0;
    while(*s) { ans += s[0]; s+=1; }
    return ans;
}
/// multibyte xor-based insecure hash function
uint32_t hash4(const char *s) { 
    uint32_t ans = 0;
    while(s[0] && s[1] && s[2]) { 
        ans ^= s[0];
        ans ^= ((uint32_t)s[1])<<8;
        ans ^= ((uint32_t)s[2])<<16;
        ans ^= ((uint32_t)s[3])<<24;
        s += 4;
    }
    return ans;
}
/// rotation-based insecure hash function
uint32_t hash5(const char *s) { 
    uint32_t ans = 0;
    while(s[0]) { 
        ans = (ans<<3) | (ans>>29);
        ans ^= s[0];
        s += 1;
    }
    return ans;
}
/// data-driven rotation-based insecure hash function
uint32_t hash6(const char *s) { 
    uint32_t ans = 0;
    while(s[0]) {
        ans = (ans<<(s[0]&0x1f)) | (ans>>(32-(s[0]&0x1f)));
        ans ^= s[0];
        s += 1;
    }
    return ans;
}

/**
 * Submits a message to be checked by the server.
 * Returns 0 if sent successfully, a negative number if there's an error.
 * 
 * Since this uses UDP, there is some small chance a message will be dropped;
 * if that happens, this function will freeze until you kill it with Ctrl+C
 */
int submitSolution(int algo, const char *msg, uint32_t signature) {
    if (algo < 0 || algo > 6) return -1;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -2;
    
    struct sockaddr_in ip;
    memset(&ip, 0, sizeof(struct sockaddr_in));
    ip.sin_family = AF_INET;
    ip.sin_addr.s_addr = inet_addr("128.143.67.106");
    ip.sin_port = htons(25013);
    
    int mlen = strlen(msg);
    char *buf = malloc(mlen+5);
    buf[0] = signature;
    buf[1] = signature>>8;
    buf[2] = signature>>16;
    buf[3] = signature>>24;
    buf[4] = algo;
    memcpy(buf+5, msg, mlen);
    if (sendto(sock, buf, mlen+5, 0, (struct sockaddr*)&ip, sizeof(struct sockaddr_in)) != mlen+5) { free(buf); close(sock); return -4; }
    free(buf);

    char reply[256];
    ssize_t len = recvfrom(sock, reply, 256, 0, NULL, NULL);
    if (len <= 0) return -5;
    write(1, "Reply: ", 7);
    write(1, reply, len);
    puts("");

    close(sock);
    return 0;
}


// tester function
int main() {
    printf("%u\n", decrypt(0x20816110)); // should print 2501
    printf("return code: %d\n", submitSolution(0, "mst3k passes", 0x03294abe));
}
