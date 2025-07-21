
int main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int* ptrP = &a;
    int* ptrQ = &b;
    c = *ptrP;
    ptrP = ptrQ;
    ptrP = &c;
}