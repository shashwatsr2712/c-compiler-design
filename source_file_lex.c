int main(){
    int a,b,s,arr[10.2]; // Here's an incorrect array declaration
    int f;
    a=1;
    b=2;
    if(a<3){
        b=4;
    }
    int a; // Here's a duplicate declaration
    float c;
    int i;
    c=3.2;
    for(i=0;(3+c)<10 && b<c;i++){   // Here's a type error in expression 3+c and a mismatch in b<c
        b=b+1;
        int e;
    }
    float f; // Here's a case of conflicting types
    int d;
    d=c; // Here's a case of illegal assignment
    s=addNums(a,b);
    e=4; // Here's a case of using an undefined variable
    return 0;
}

int addNums(int x,int y){
    return x+y;
}