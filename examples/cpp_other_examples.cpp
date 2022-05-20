#include "bech32.h"
#include <iostream>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>


/**
 * Illustrates encoding a "human readable part" and a "data part" to a bech32 string, then
 * decoding that bech32 string and comparing the results.
 */
void encodeAndDecode() {

    // human-readable part
    std::string hrp = "example";
    // data values can be 0-31
    std::vector<unsigned char> data = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // expected bech32 string output
    char expected[] = "example1qpzry9x8ge8sqgv";

    // encode
    std::string bstr = bech32::encode(hrp, data);
    assert(expected == bstr);

    // decode
    bech32::DecodedResult decodedResult = bech32::decode(bstr);

    assert(hrp == decodedResult.hrp);
    assert(data == decodedResult.dp);
    assert(bech32::Encoding::Bech32m == decodedResult.encoding);
}

/**
 * Illustrates decoding a possibly "dirty" bech32 string into its "human readable part" and
 * "data part". Then encodes that data to another bech32 string and compares the results.
 */
void decodeAndEncode() {

    // bech32 string with extra invalid characters
    std::string bstr = " example1:qpz!r--y9#x8&%&%ge-8-sqgv ";
    std::string expected = "example1qpzry9x8ge8sqgv";
    // decode - make sure to strip invalid characters before trying to decode
    bech32::DecodedResult decodedResult = bech32::decode(bech32::stripUnknownChars(bstr));

    // verify decoding
    assert(!decodedResult.hrp.empty() && !decodedResult.dp.empty());
    assert(bech32::Encoding::Bech32m == decodedResult.encoding);

    // encode
    bstr = bech32::encode(decodedResult.hrp, decodedResult.dp);

    // encoding of "cleaned" decoded data should match expected string
    assert(bstr == expected);
}

/**
 * Illustrates exception thrown when attempting to encode bad data--in this case, "33" is not in the
 * range of allowed data values (0-31).
 */
void badEncoding() {

    // hrp and data to encode
    std::string hrp = "example";
    std::vector<unsigned char> data = {0, 1, 2, 3, 4, 5, 6, 7, 33};

    // encode
    try {
        std::string bstr = bech32::encode(hrp, data);
    }
    catch (std::exception &e) {
        assert(std::string(e.what()) == "data value is out of range");
    }
}

/**
 * Illustrates failure when attempting to decode bad data--in this case, the "z" character from
 * the bech32 string has been corrupted and changed to a "x". This will cause the checksum
 * verification to fail and an "Invalid" encoding to be returned.
 */
void badDecoding_corruptData() {

    // valid bech32 string
    std::string bstr = "example1qpzry9x8ge8sqgv";
    // simulate corrupted data--checksum verification will fail
    bstr[10] = 'x';

    // decode
    bech32::DecodedResult decodedResult = bech32::decode(bstr);

    // verify decoding failed
    assert(decodedResult.hrp.empty() && decodedResult.dp.empty());
    assert(bech32::Encoding::Invalid == decodedResult.encoding);
}

/**
 * Illustrates failure when attempting to decode bad data--in this case, the "s" character from
 * the bech32 string's checksum has been corrupted and changed to a "q". This will cause the
 * checksum verification to fail and an "Invalid" encoding to be returned.
 */
void badDecoding_corruptChecksum() {

    // valid bech32 string
    std::string bstr = "example1qpzry9x8ge8sqgv";
    // simulate corrupted checksum--verification will fail
    bstr[19] = 'q';

    // decode
    bech32::DecodedResult decodedResult = bech32::decode(bstr);

    // verify decoding failed
    assert(decodedResult.hrp.empty() && decodedResult.dp.empty());
    assert(bech32::Encoding::Invalid == decodedResult.encoding);
}

int main() {
    encodeAndDecode();
    decodeAndEncode();
    badEncoding();
    badDecoding_corruptData();
    badDecoding_corruptChecksum();
}
