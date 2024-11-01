#define WIN_SIZE 16
#define PRIME 3
#define MODULUS 256
#define TARGET 0
#define NUM_CHUNKS 1000
#define MAX_CHUNK_SIZE 8192
#define INITIAL_DICT_CAPACITY 256

uint64_t compute_prime_power_window();
uint64_t initialize_hash(unsigned char* input, unsigned int window_size);
uint64_t update_hash(uint64_t hash, unsigned char outgoing, unsigned char incoming);
std::vector<std::vector<unsigned char>> cdc(unsigned char* buff, unsigned int buff_size);
