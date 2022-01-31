#include <tomcrypt.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <fstream>
using namespace std;
unsigned char *hashSHA2(const string &); //prototype of the SHA256
int main()
{
    fstream newfile;
    string st="";
    char stream[];
    newfile.open("SharedSecretKey.txt",ios::in);
    if (newfile.is_open()){ //checking whether the file is open
        string tp;
        while(getline(newfile, tp)){ //read data from file object and put it into string.
            st += tp; //print the data of the string
        }
        newfile >> stream;
      newfile.close();
    }
    cout << st;
    cout << stream;
    /*
    cout <<"Testing SHA256 in LibTopmCrypt... "<< endl;
    string input ="We are testing LibTomCrypt";
    unsigned char *hash_result = hashSHA2(input);
    string hash ="";
    string ss;
    for (int i = 0; i < int(sha256_desc.hashsize); i++)
    {
        ss << hex << (int)hash_result[i];
        hash = ss;
    }
    cout <<"This is the hash : "<< hash << endl;
    */
}
//SHA-256 function
unsigned char *hashSHA2(const string &input)
{
    unsigned char *hash_res = new unsigned char[sha256_desc.hashsize];
    hash_state md;
    sha256_init(&md);
    sha256_process(&md, (const unsigned char *) input.c_str(), input.size());
    sha256_done(&md, hash_res);
    return hash_res;
}