#include <tomcrypt.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <chrono>
#include <thread>
#include <zmq.hpp>
using namespace std;
unsigned char *hashSHA2(const string &); //prototype of the SHA256
int main()
{
    using namespace std::chrono_literals;
    fstream newfile;
    string pt="";
    //For Plain text
    newfile.open("HW1PlaintextTest.txt",ios::in);
    if (newfile.is_open()){ //checking whether the file is open
        string tp;
        while(getline(newfile, tp)){ //read data from file object and put it into string.
            pt += tp; //print the data of the string
        }
      newfile.close();
    }
    //For secret key
    string sk="";
    newfile.open("SharedSecretKey.txt",ios::in);
    if (newfile.is_open()){ //checking whether the file is open
        string tp;
        while(getline(newfile, tp)){ //read data from file object and put it into string.
            sk += tp; //print the data of the string
        }
      newfile.close();
    }
    int pt_len = pt.length();
    if( pt_len%32 != 0 ){
    	cout << "Input is not a multiple of 32 " << endl;
	return 0;
    } 
    char ct[pt_len];
    char key[pt_len];
    for(int i=1;i<=pt_len/32;i++){
    	string tmp = sk + to_string(i);
	unsigned char* seed = hashSHA2(tmp);
	for(int j=0;j<32;j++){
	    key[(i-1)*32 + j] = seed[j];
	}
    }
    string cp="";
    string cp_hex="";
    cout << "\nHex data \n";	
    for(int i=0;i<pt_len;i++){
	stringstream cp_tmp;
    	char tmp;
	ct[i]= pt[i] ^ key[i];
	cout << i << ":"<<"C="<<ct[i] << ",K="<< key[i] <<",P=" << pt[i] << endl;
	cp_tmp<<hex<<(int)ct[i];
	cp += cp_tmp.str() + " ";
	cp_hex += cp_tmp.str();
    }
    cp.pop_back();
    cout << "Ciper Text:" << cp << std::endl;
    unsigned char* hash_result = hashSHA2(pt);
    string hash="";
    stringstream ss;
    for (int i=0; i<int(sha256_desc.hashsize); i++)
    {
	ss<<hex<<(int)hash_result[i];
        hash = ss.str();
    }
    cout<<"This is the hash of Plain Text:"<<hash<<endl;
    cout<<"length of cipher text: "<< strlen(ct) << endl;
    cp = hash + " " + cp;
    cout << "Cipher Text + hash :" << cp << endl;

    //Save cipher hex in file
    ofstream file("TheCiphertext.txt");
    file << cp_hex;

    //send to client
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REP (reply) socket and bind to interface
    zmq::socket_t socket{context, zmq::socket_type::rep};
    socket.bind("tcp://*:5556");
    // prepare some static data for responses
    const std::string data{"Sending Cipher text"};

    zmq::message_t request;

    // receive a request from client
    socket.recv(request, zmq::recv_flags::none);
    std::cout << "Received " << request.to_string() << std::endl;

    // simulate work
    std::this_thread::sleep_for(4s);

    // send the reply to the client
    socket.send(zmq::buffer(string(cp)), zmq::send_flags::none);

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
