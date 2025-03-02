#!/bin/bash
set -e

# Clean up any previous test
rm -rf test_repo
mkdir -p test_repo
cd test_repo

echo "======================================================"
echo "1. Initializing repository with Valgrind..."
valgrind --leak-check=full --verbose ../build/linux/syncro.bin init
echo "======================================================"

# You need to create LICENSE.md before adding it
echo "2. Creating LICENSE.md file..."
cat > LICENSE.md << 'EOF'
This is a simple test file to verify compression and decompression.
We will use this file to test if the content is preserved correctly
through the compression and decompression process.

Some special characters: !@#$%^&*()_+~`-=[]{}|;':",./<>?
Some numbers: 1234567890
EOF

echo "======================================================"
echo "3. Adding LICENSE.md to tracking with Valgrind..."
valgrind --leak-check=full ../build/linux/syncro.bin add LICENSE.md
echo "======================================================"

echo "======================================================"
echo "4. Committing LICENSE.md with Valgrind..."
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ../build/linux/syncro.bin commit -m "Add LICENSE file"
echo "======================================================"

# Save a copy before potential corruption
echo "5. Creating a copy of LICENSE.md for comparison..."
cp LICENSE.md LICENSE.md.original

echo "======================================================"
echo "6. Testing vdiff on LICENSE.md with Valgrind..."
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ../build/linux/syncro.bin vdiff LICENSE.md
echo "======================================================"

echo "7. Running diff to verify content matches..."
diff LICENSE.md LICENSE.md.original
if [ $? -eq 0 ]; then
    echo "SUCCESS: Files match! Compression/decompression is working correctly."
else
    echo "FAILURE: Files don't match. There's an issue with compression/decompression."
    exit 1
fi

# echo "======================================================"
# echo "8. Testing memory handling with large file..."
# # Create a larger file to test memory handling
# dd if=/dev/urandom of=large_file.bin bs=1M count=5
# cp large_file.bin large_file.bin.original

# echo "======================================================"
# echo "9. Add and commit large file with Valgrind..."
# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ../build/linux/syncro.bin add large_file.bin
# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ../build/linux/syncro.bin commit -m "Add large file"
# echo "======================================================"

# echo "10. Testing vdiff on large file with Valgrind..."
# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ../build/linux/syncro.bin vdiff large_file.bin
# echo "======================================================"

# echo "11. Running diff to verify large file content matches..."
# diff large_file.bin large_file.bin.original
# if [ $? -eq 0 ]; then
#     echo "SUCCESS: Large files match! Compression/decompression handles large files correctly."
# else
#     echo "FAILURE: Large files don't match. There's an issue with compression/decompression for large files."
#     exit 1
# fi

# echo "======================================================"
# echo "All tests passed successfully!"
# echo "No memory leaks or errors detected by Valgrind."
# echo "======================================================" 