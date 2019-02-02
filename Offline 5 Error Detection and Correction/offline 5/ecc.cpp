#include<bits/stdc++.h>
using namespace std;
int m;
string pol;
float p;
int errorBits[1000];
string newdb[1000];

string replaceBit(string str, int start, int interval){
    int len = str.length();
    int n = 0;
    for(int i = start; i<len;i+=(2*interval)){
        for(int j = 0;j<interval;j++){
            if(i+j>=len){
                break;
            }
            if(i+j==start){
                continue;
            }
            if(str[i+j]=='1'){
                n++;
            }
        }
    }
    if(n%2){
        return "1";
    }
    return "0";
}

string produceRedundantBits(string str){
    string temp = str;
    int len = temp.length();
    int k = 0;
    
    for(int i = 1;i<=len;i=(1<<k)){
        k++;
        temp.insert(i-1,"0");
    }
    k = 0;
    for(int i = 1;i<=len;i=(1<<k)){
        k++;
        string rb = replaceBit(temp,i-1,i);
        temp.replace(i-1,1,rb);
    }
    return temp;
}

void printDatablock(string db[], int size){
    for(int i = 0;i<size;i++){
        string temp = db[i];
        int period = 1;
        int k = 0;
        for(int j = 0;j<temp.length();j++){
            if(j == period-1){
                cout << "\033[1;32m"<<temp[j]<<"\033[0m";
                period = (1<<(k+1));
                k++;     
                continue;   
            }
            cout<<temp[j];
        }
        cout<<endl;
    }
}

string columnWiseSerialization(string db[], int size){
    string temp = "";
    int len = db[0].length();
    for(int i = 0;i<len;i++){
        for(int j = 0;j<size;j++){
            temp+=db[j][i]; 
        }
    }
    cout<<temp<<endl;
    return temp;
}

string XOR(string a, string b){
    int len = b.length();
    string rem = "";
    for(int i = 1;i<len;i++){
        if(a[i] == b[i]){
            rem+="0";
        }
        else{
            rem+="1";
        }
    }
    return rem;
}

string binaryDiv(string divident, string divisor){
    int len = divisor.length();
    string temp = divident.substr(0,len);
    
    int dlen = divident.length();
    while(len < dlen){
        if(temp[0] == '1'){
            temp = XOR(divisor, temp) + divident[len];
        }
        else{
            string dstr(len,'0');
            temp = XOR(dstr, temp) + divident[len];
        }
        len++;
    }

    if (temp[0] == '1'){ 
        temp = XOR(divisor, temp);
    } 
    else{
        string dstr(len,'0');
        temp = XOR(dstr, temp);
    }
    return temp; 
}

string calculateCRC(string data, string str, int r){
    
    int len = str.length();
    string rem = binaryDiv(str,pol);
    data+=rem;

    for(int j = 0;j<len;j++){
        if(j > len-r-1){
            cout << "\033[1;36m"<<data[j]<<"\033[0m";
            continue;   
        }
        cout<<data[j];
    }
    cout<<endl;
    return data;
}

string CRC(string data){
    int r = pol.length();
    r--;
    string temp(r,'0');
    string str = data;
    str+=temp;
    return calculateCRC(data,str,r);
}

void checkCRC(string rs){
    string rem = binaryDiv(rs,pol);
    int len = pol.length();
    string str(len-1, '0');
    if(rem == str){
        printf("No error detected\n");
    } 
    else{
        printf("Error detected\n");
    }   
}

void toggle(char &ch){
    if(ch == '1'){
        ch = '0';
    }
    else{
        ch = '1';
    }
}

string addErrorBits(string data){
    int len = data.length();
    memset(errorBits,0,sizeof(errorBits));
    int range = 100+1;
    float probOfErrorbits = p*100;
    int k = 0;
    srand(time(0));
    for(int i = 0;i<len;i++){
        float tf = rand()%range;
        if(tf <= probOfErrorbits){
            toggle(data[i]);
            errorBits[i] = 1;
            cout << "\033[1;31m"<<data[i]<<"\033[0m";
            continue;
        }
        cout << data[i];
    }
    cout<<endl;
    return data;
}

void deSerialization(string data, int size){
    int len = pol.length();
    len--;
    string temp = data.substr(0,data.length()-len);
    //cout<<temp<<endl;
    int col = temp.length()/size;
    
    for(int i = 0;i<size;i++){
        int interval = i;
        string temp = "";
        for(int j = 0;j<col;j++){
            temp += data[interval];
            if(errorBits[interval] == 1){
                cout << "\033[1;31m"<<data[interval]<<"\033[0m";
            }
            else
            {
                cout<<data[interval];
            }
            
            interval+=size; 
        }
        newdb[i] = temp;
        cout<<endl;
    }
}

string checkParity(string str, int start, int interval){
    int len = str.length();
    int n = 0;
    for(int i = start; i<len;i+=(2*interval)){
        for(int j = 0;j<interval;j++){
            if(i+j>=len){
                break;
            }
            if(str[i+j]=='1'){
                n++;
            }
        }
    }
    if(n%2){
        return "1";
    }
    return "0";
}

bool isPowerOfTwo (int x) 
{ 
  return x && (!(x&(x-1))); 
} 

void checkError(int size){

    for(int j = 0;j<size;j++){
        string temp = newdb[j];
        int len = m*8;
        int k = 0;
        int pos = 0;
        for(int i = 1;i<=len;i=(1<<k)){
            k++;
            string rb = checkParity(temp,i-1,i);
            if(rb == "1"){
                pos+=i;
            }
        }
        if(pos!=0){
            toggle(newdb[j][pos-1]);
        }
    }
    for(int j = 0;j<size;j++){
        string temp = newdb[j];
        int len = m*8;
        int tlen = temp.length();
        int k = 0;
        int pos = 0;
        string built = "";
        for(int in = 0;in<tlen;in++){
            if(isPowerOfTwo(in+1)){
                continue;
            }
            built += temp[in];
        }
        newdb[j] = built;
        cout<<newdb[j]<<endl;
    }

}


void convertToCharacter(int size){
    for(int i = 0;i<size;i++){
        string data = newdb[i];
        stringstream sstream(data);
        string output;
        while(sstream.good())
        {
            std::bitset<8> bits;
            sstream >> bits;
            char c = char(bits.to_ulong());
            output += c;
        }

        cout << output;
    }
    cout<<endl;
}

int main(){
    freopen("input.txt","r",stdin);
    string str;
    printf("Enter the data string: ");
    getline(cin,str);
    cout<<str<<endl;
    printf("Enter number of data bytes in a row <m>: ");
    cin>>m;
    cout<<m<<endl;
    printf("Enter probability <p>: ");
    cin>>p;
    cout<<p<<endl;
    printf("Enter generator polynomial: ");
    cin>>pol;
    cout<<pol<<endl;
    printf("\nData string after padding: ");
    
    if(str.length()%m){
        int k = floor(str.length()/m);
        k = ((k+1)*m) - str.length();
        string temp(k,'~');
        str+=temp;
    }
    cout<<str<<endl;
    
    int size = str.length()/m;
    string datablock[size];

    int idx = 0,tsize = size,t = 0;
    while(tsize--){
        string temp = "";

        for(int i = t;i<m+t;i++){
            temp+=bitset<8>(int(str[i])).to_string();
        }
        datablock[idx++] = temp;
        t+=m;
    }

    printf("\nData block <ascii code of m characters per row>: \n");

    for(int i = 0;i<size;i++){
        cout<<datablock[i]<<endl;
    }

    printf("\nData block after adding checkbits:\n");

    for(int i = 0;i<size;i++){
        datablock[i] = produceRedundantBits(datablock[i]);
    }
    
    printDatablock(datablock,size);
    printf("\nData bits after column-wise serialization:\n");

    string serialized = columnWiseSerialization(datablock,size);

    printf("\nData bits after appending CRC checksum <sent frame>:\n");


    string crcstring = CRC(serialized);

    printf("\nReceived frame:\n");

    string receivedData = addErrorBits(crcstring);

    printf("\nResult of CRC checksum matchings: ");
    
    checkCRC(receivedData);
    
    printf("\nData block after removing CRC checksum bits:\n");

    deSerialization(receivedData, size);

    printf("\nData block after removing check bits:\n");

    checkError(size);

    printf("\nOutput frame: ");

    convertToCharacter(size);

    return 0;
}