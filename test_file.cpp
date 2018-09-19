#include<bits/stdc++.h>
using namespace std;


int main(){
    FILE* in = fopen("./input.txt", "w");
    srand(time(NULL));
    for (long long i = 0; i <524288  ; i++)
    	fprintf(in, "%d\n", rand()); 
    fclose(in);
    return 0;
}

