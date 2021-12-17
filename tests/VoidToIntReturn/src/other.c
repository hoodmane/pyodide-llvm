void do_the_thing(int a, int b);

int f(int x){
    do_the_thing(x, x);
    return 77;
}