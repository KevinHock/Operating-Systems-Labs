int compare_keys (const char *string1, int len1, const char *string2, int len2, int *pKeylen) {
    int keylen, offset1, offset2;
    keylen = len1 < len2 ? len1 : len2;
    //one of these offsets is going to be zero
    offset1 = len1 - keylen;
    offset2 = len2 - keylen;
    assert (keylen > 0); 
    if (pKeylen)
      *pKeylen = keylen;
    return strncmp(&string1[offset1], &string2[offset2], keylen);
}

