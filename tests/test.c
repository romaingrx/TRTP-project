void test_varuint_decode_encode(){
    ssize_t err;
    size_t bytes = 2;
    uint16_t retval;
    uint8_t *data_decode = malloc(bytes);
    data_decode[0] = 0b10001110;
    data_decode[1] = 0b100011;
    err  = varuint_decode(data_decode, bytes, &retval);
    uint8_t *data_encode = malloc(bytes);
    err = varuint_encode(retval, data_encode, bytes);
    printf("Data encodé en Network Byte Order avant le décodage :\n");
    printf("      data_decode[0] = %u | data_decode[1] = %u \n",data_decode[0], data_decode[1]);
    printf("Valeur stockée dans le data_decode : %u \n",retval);
    printf("Data encodé en Network Byte Order après l'encodage: \n");
    printf("      data_encode[0] = %u | data_encode[1] = %u \n",data_encode[0], data_encode[1]);
    free(data_decode); free(data_encode);
}
