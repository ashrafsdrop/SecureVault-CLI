#include <iostream>
#include <cstring>
#include "aes.h"

// Function to encrypt data using AES-128 ECB mode
void aes_encrypt(const uint8_t *key, const uint8_t *input, uint8_t *output) {
    // AES context
    struct AES_ctx ctx;

    // Initialize the context with the key
    AES_init_ctx(&ctx, key);

    // Encrypt the data
    uint8_t temp[16];
    memcpy(temp, input, 16); // Copy input to a temporary buffer
    AES_ECB_encrypt(&ctx, temp);
    memcpy(output, temp, 16); // Copy the encrypted data to output

    // Print encrypted data
    std::cout << "Encrypted data: ";
    for (int i = 0; i < 16; i++) {
        std::cout << std::hex << (int)output[i] << " ";
    }
    std::cout << std::endl;
}

// Function to decrypt data using AES-128 ECB mode
void aes_decrypt(const uint8_t *key, const uint8_t *input, uint8_t *output) {
    // AES context
    struct AES_ctx ctx;

    // Initialize the context with the key
    AES_init_ctx(&ctx, key);

    // Decrypt the data
    uint8_t temp[16];
    memcpy(temp, input, 16); // Copy input to a temporary buffer
    AES_ECB_decrypt(&ctx, temp);
    memcpy(output, temp, 16); // Copy the decrypted data to output

    // Print decrypted data
    std::cout << "Decrypted data: " << output << std::endl;
}

int main() {
    // Key and input data (16 bytes for AES-128)
    uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint8_t input[16] = "Hello, AES Test"; // String truncated to 16 bytes
    uint8_t output[16];
    uint8_t decrypted[16];

    // Encrypt the data
    aes_encrypt(key, input, output);

    // Decrypt the data
    aes_decrypt(key, output, decrypted);

    return 0;
}
