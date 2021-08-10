__asm__("ljmpl $0x0008, $main\n");

int main ()
{
    *(char*)(0xb8000) = 't';
    *(char*)(0xb8001) = 7;
    // dont return!
    while(1) {}
}
