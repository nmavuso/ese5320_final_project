int appHost(unsigned char* buffer, unsigned int length, FILE* outfc,
            cl::Kernel &krnl_lzw, cl::CommandQueue &q,
            cl::Buffer &input_buf, cl::Buffer &output_buf, cl::Buffer &output_size_buf,
            char *input, int *output_hw, int *output_size_hw) {
    std::cout << "Encoding Working Started" << std::endl;

    // Step 1: Process Ethernet Input
    std::cout << "Processing Ethernet Input..." << std::endl;
    unsigned int buffer_size = length;
    std::cout << "Buffer size: " << buffer_size << std::endl;

    // Step 2: Chunking the Ethernet input
    std::cout << "Chunking Ethernet Input..." << std::endl;
    Chunk chunks[NUM_PACKETS];
    int num_chunks = 0;
    cdc(buffer, buffer_size, chunks, &num_chunks);
    std::cout << "Number of chunks created: " << num_chunks << std::endl;

    // Step 3: Deduplication and Encoding
    std::cout << "Starting deduplication and encoding..." << std::endl;
    for (int i = 0; i < num_chunks; ++i) {
        std::cout << "Processing chunk " << i + 1 << "..." << std::endl;
        unsigned char* chunk_data = chunks[i].data;
        int chunk_size = chunks[i].size;
        std::cout << "Chunk size: " << chunk_size << std::endl;

        // Deduplicate chunk
        HashTable hash_table;
        initialize_hash_table(&hash_table);
        int is_new_chunk = deduplicate_chunks(chunk_data, chunk_size, &hash_table);
        std::cout << "Deduplication result for chunk " << i + 1 << ": " << is_new_chunk << std::endl;

        if (is_new_chunk == 1) {
            std::cout << "Encoding chunk " << i + 1 << " using FPGA..." << std::endl;

            // Load the current chunk into the input buffer
            strncpy(input, (const char *)chunks[i].data, MAX_INPUT_SIZE);

            // Migrate input buffer to FPGA
            q.enqueueMigrateMemObjects({input_buf}, 0 /* Host to device */, NULL, NULL);
            q.finish();

            // Launch the FPGA kernel
            q.enqueueTask(krnl_lzw, NULL, NULL);
            q.finish();

            // Migrate output buffer back to host
            q.enqueueMigrateMemObjects({output_buf, output_size_buf}, CL_MIGRATE_MEM_OBJECT_HOST);
            q.finish();

            // // Retrieve encoded data
            // int encoded_size = *output_size_hw;

            std::cout << "Getting Encoded Data: " << std::endl;

            // // Write encoded data to the output file
            // fwrite(&encoded_size, sizeof(int), 1, outfc);  // Write encoded size
            // fwrite(output_hw, sizeof(int), encoded_size, outfc);  // Write encoded data

            // Print encoded data for debugging
            std::cout << "Encoded Chunk FPGA: ";
            for (int j = 0; j < encoded_size; ++j) {
                std::cout << output_hw[j] << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "Chunk " << i + 1 << " is a duplicate and was not re-encoded." << std::endl;
        }
    }

    std::cout << "Encoding Complete" << std::endl;
    return 0;
}
