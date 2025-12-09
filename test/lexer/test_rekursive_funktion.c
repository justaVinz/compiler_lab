int quersumme(int n, int sum){
        if (n == 0)
                return sum;
        return quersumme(n/10, sum + n%10);
}
/*
Let's use our recursive function in a main function
//This comment shouldn't change anything


*/

int main(void){
        int n;
        n=3;
        printf("%d", quersumme(n, 0)); //we still need a print function to handle this \n\\\0 hihi
}