# Hachiman-Data-Encoder
Huffman-based text and image compression.

## Features
### Text Compression
- Builds a Huffman tree from character frequencies.
- Generates optimal prefix codes.
- Encodes and decodes arbitrary input text.
- Displays a frequency table with characters, counts, and Huffman codes.
- Computes compression ratio.

### Image Compression
- Loads images using stb_image (PNG, JPG, etc.).
- Applies predictive preprocessing:
  `processed = current_pixel - previous_pixel + 255`
- Builds a Huffman tree for processed pixel values (0–510 range).
- Encodes the image into a compressed bitstring.
- Decodes using inverse predictive coding.
- Saves reconstructed image using stb_image_write.

### Step by step visualization
- Gui enables user to view each step in huffman tree formation
- it uses widgets to display the huffnodes in the heap
- the heap is displayed after every successive operation on it

### Encryption/Decryption
- After compresssion, the compressed bits are encrypted
- Encryption is performed via AES
- We have used openssl for using AES
- before decompression, the encrypted bits are decrypted

---

## DSA Concepts used
- Heaps
- Huffman trees
- Arrays
- Pointers
- User defined ADTs (huffnode class)
- Greedy approach (In making the huffman tree)

---

## Summary of compression funcs

### Text
1. Count frequency of characters.
2. Create min-heap of nodes.
3. Repeatedly merge the two smallest nodes.
4. Generate codes by traversing the final tree.
5. Replace characters with codes to encode.
6. Decode bits by walking the tree.

### Image
1. Load raw pixel data.
2. Preprocess each pixel using predictive coding:
   `delta = pixel - previous_pixel + 255`
3. Build Huffman tree over delta frequencies.
4. Encode delta stream.
5. Decode to reconstruct deltas.
6. Rebuild the original pixels with:
   `pixel = delta - 255 + previous_pixel`

---
## Results
- An average compression ratio of ~50 to 60% is observed
- Algorithmic complexity ~O(n) or O(k) in most functions
- where k is either 255 or 510

---

## Build Instructions

### Requirements
- C++17 compiler
- Qt 5 or Qt 6
- stb_image / stb_image_write
- CMake or qmake
- openssl (For AES)
