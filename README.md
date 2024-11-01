# ESE5320 Final Project: Deduplication and Compression

**Members**

Ahmed Abdellah

Nhlanhla Mavuso 

Joaquin Revello

**Deduplication and Compression**

**Goal:** Developing a compressor that can receive data in real time at modern ethernet speeds and compress it into memory using deduplication and compression. We specificallt look at Content-Defined Chunking to break the input into chunks, SHA-256 (or SHA3-384) hasehs to screen for duplicate chunks, and LZW compression to compress non-duplicate chunks. 
