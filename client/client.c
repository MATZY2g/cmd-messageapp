#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#define KEY_BYTES 32
#define MAX_MSG 1024

int hexval(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

int from_hex(const char *hex, uint8_t *out, size_t outlen)
{
    size_t n = strlen(hex);
    if (n != outlen * 2)
        return -1;
    for (size_t i = 0; i < outlen; i++)
    {
        int h = hexval(hex[2 * i]);
        int l = hexval(hex[2 * i + 1]);
        if (h < 0 || l < 0)
            return -1;
        out[i] = (h << 4) | l;
    }
    return 0;
}

char *to_hex(const uint8_t *in, size_t len)
{
    static const char *tbl = "0123456789abcdef";
    char *s = malloc(len * 2 + 1);
    if (!s)
        return NULL;
    for (size_t i = 0; i < len; i++)
    {
        s[2 * i] = tbl[in[i] >> 4];
        s[2 * i + 1] = tbl[in[i] & 0xF];
    }
    s[len * 2] = '\0';
    return s;
}

void xor_stream(const uint8_t *key, const uint8_t *in, uint8_t *out, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        out[i] = in[i] ^ key[i % KEY_BYTES];
    }
}

void load_or_gen_key(uint8_t *key)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/.termmsg_key.hex", getenv("HOME"));
    FILE *f = fopen(path, "r");
    if (f)
    {
        char hex[KEY_BYTES * 2 + 1] = {0};
        fread(hex, 1, KEY_BYTES * 2, f);
        fclose(f);
        from_hex(hex, key, KEY_BYTES);
        printf("Hi! Your key loaded from %s\n", path);
    }
    else
    {
        f = fopen(path, "w");
        int fd = open("/dev/urandom", O_RDONLY);
        read(fd, key, KEY_BYTES);
        close(fd);
        char *hex = to_hex(key, KEY_BYTES);
        fwrite(hex, 1, KEY_BYTES * 2, f);
        fclose(f);
        free(hex);
        printf("Generated new key and saved to %s\n", path);
    }
}

int main()
{
    uint8_t mykey[KEY_BYTES];
    load_or_gen_key(mykey);

    while (1)
    {
        printf("\n=== Terminal Messenger Demo ===\n");
        printf("1. Send a message\n");
        printf("2. Receive messages (demo)\n");
        printf("3. Exit\n");
        printf("Choose an option: ");

        int choice = 0;
        scanf("%d", &choice);
        getchar();

        if (choice == 1)
        {
            char recip_hex[KEY_BYTES * 2 + 1] = {0};
            printf("Enter recipient key (hex, 64 chars): ");
            scanf("%64s", recip_hex);
            getchar();
            uint8_t recip_key[KEY_BYTES];
            if (from_hex(recip_hex, recip_key, KEY_BYTES) != 0)
            {
                printf("Invalid recipient key!\n");
                continue;
            }

            char msg[MAX_MSG] = {0};
            printf("Enter your message: ");
            fgets(msg, sizeof(msg), stdin);
            size_t msglen = strcspn(msg, "\n");
            msg[msglen] = '\0';

            uint8_t ciph[MAX_MSG];
            xor_stream(recip_key, (uint8_t *)msg, ciph, msglen);
            char *hex_msg = to_hex(ciph, msglen);

            printf("\n[Encrypted message ready to send]: %s\n", hex_msg);
            printf("[Demo mode: not sending anywhere yet]\n");
            free(hex_msg);
        }
        else if (choice == 2)
        {
            printf("[Demo receive] No real server yet.\n");
        }
        else if (choice == 3)
        {
            printf("Exiting...\n");
            break;
        }
        else
        {
            printf("Invalid option!\n");
        }
    }
    return 0;
}
