// #include "stdio.h"

void f(int);


void do_the_thing(int a, int b){
    if(a > 0){
        a -= b;
    } else {
        a += b;
    }
    f(a);
}



int test(){
    do_the_thing(2,6);
    return 777;
}
