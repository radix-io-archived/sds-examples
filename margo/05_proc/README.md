# Variable-length encoding

There are many examples of how to use `MERCURY_GEN_PROC` for structures containing built-in types.  What if your data structure varies, though?

In this example, the client reads in a list of words. For each word in the list, it sends the word to the server and gets back a hash.  Instead of relying on a fixed size "word buffer", this example will encode the client side information and decode the server response.

See `hg_proc_hash_in_t` for the variable lenght processing.

## usage

Start the server:

    05_margo_hash_server

Start the client:

    05_margo_hash_client <mercury_addr> <list_of_words>
