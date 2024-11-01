void deduplicate_chunks(chunks) {
   hash_table = initialize_hash_table();
   for (int i = 0; i < NUM_CHUNKS; i++) {
	chunk_hash = compute_hash(chunks[i]);
	....
   }
}
