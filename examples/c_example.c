#include "libbech32.h"
#include <string.h>
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>

void encodeAndDecode() {

    // hrp and data to encode
    char hrp[] = "example";
    unsigned char dp[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // create output for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(sizeof(hrp), sizeof(dp));

    // expected bech32 string output
    char expected[] = "example1qpzry9x8ge8sqgv";

    // encode
    assert(bech32_encode(bstring, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstring->string) == 0);

    // allocate memory for decoded data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstring->string);

    // decode
    assert(bech32_decode(decodedResult, bstring->string) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, "example") == 0);
    assert(decodedResult->dp[0] == 0);
    assert(decodedResult->dp[8] == 8);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    // free memory
    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void decodeAndEncode() {

    // bech32 string with extra invalid characters
    char bstr[] = " example1:qpz!r--y9#x8&%&%ge-8-sqgv ";
    // expected bech32 string output
    char expected[] = "example1qpzry9x8ge8sqgv";

    // make sure to strip invalid characters before allocating storage and trying to decode
    assert(bech32_stripUnknownChars(bstr, sizeof(bstr), bstr, sizeof(bstr)) == E_BECH32_SUCCESS);

    // allocate memory for decoded data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    // decode
    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, "example") == 0);
    assert(decodedResult->dp[0] == 0);
    assert(decodedResult->dp[8] == 8);

    // create output array for bech32 string
    bech32_bstring *bstring = bech32_create_bstring_from_DecodedResult(decodedResult);

    // encode
    assert(bech32_encode(bstring, decodedResult->hrp, decodedResult->dp, decodedResult->dplen) == E_BECH32_SUCCESS);

    // encoding of "cleaned" decoded data should match expected string
    assert(strcmp(expected, bstring->string) == 0);

    // free memory
    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void badEncoding() {

    // hrp and data to encode
    char hrp[] = "example";
    unsigned char dp[] = {0, 1, 2, 3, 4, 5, 6, 7, 33};

    // create output for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(sizeof(hrp), sizeof(dp));

    // encode
    assert(bech32_encode(bstring, hrp, dp, sizeof(dp)) == E_BECH32_UNKNOWN_ERROR);

    // free memory
    bech32_free_bstring(bstring);
}

void badDecoding_corruptData() {

    // bech32 string
    char bstr[] = "example1qpzry9x8ge8sqgv";
    // simulate corrupted data--checksum verification will fail
    bstr[10] = 'x';

    // allocate memory for decoded data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    // decode
    assert(bech32_decode(decodedResult, bstr) == E_BECH32_INVALID_CHECKSUM);

    // free memory
    bech32_free_DecodedResult(decodedResult);
}

void badDecoding_corruptChecksum() {

    // bech32 string
    char bstr[] = "example1qpzry9x8ge8sqgv";
    // simulate corrupted checksum--verification will fail
    bstr[19] = 'q';

    // allocate memory for decoded data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    // decode
    assert(bech32_decode(decodedResult, bstr) == E_BECH32_INVALID_CHECKSUM);

    // free memory
    bech32_free_DecodedResult(decodedResult);
}

int main() {
    encodeAndDecode();
    decodeAndEncode();
    badEncoding();
    badDecoding_corruptData();
    badDecoding_corruptChecksum();
}
