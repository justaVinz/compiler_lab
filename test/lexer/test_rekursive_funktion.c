int quersumme(int n, int sum){
        if (n == 0)
                return sum;
        return quersumme(n/10, sum + n%10);
}

int main(){
        int n;
        n=3;
        printf("%d", quersumme(n, 0));
}