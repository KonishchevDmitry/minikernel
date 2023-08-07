static volatile unsigned char* VIDEO = (unsigned char*) 0xB8000;

void test() {
    VIDEO[0] = 'A';
    VIDEO[1] = 'A';
}