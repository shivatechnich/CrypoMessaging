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
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REQ (request) socket and connect to interface
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:5556");

    // set up some static data to send
    const std::string data{"Send Message"};

    // send the request message for server
    std::cout << "Requesting Server for text" << "..." << std::endl;
    socket.send(zmq::buffer(data), zmq::send_flags::none);
    
    // wait for reply from server
    zmq::message_t reply{};
    socket.recv(reply, zmq::recv_flags::none);

    //std::cout << "Received " << reply.to_string();
	//Send successful message
	//socket.send(zmq::buffer(data),zmq::send_flags::none);
	
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
    string rep = reply.to_string();
    int pos = rep.find(" ");
    string cp;
    cp = rep.substr(pos+1);
    rep.erase(pos);
    string rec_pt_hash;
    rec_pt_hash = rep;
    //Decrypt
    std::istringstream hex_chars_stream(cp);
    std::vector<char> bytes;
    unsigned int c;
    while (hex_chars_stream >> std::hex >> c)
    {
        bytes.push_back(c);
    }
    int mess_len = bytes.size();
    char* ct = bytes.data();
    //cout << "Cipher Text:" << string(ct)<< endl;
    char pt[mess_len];
    char key[mess_len];

    //cout << "Message lenght : "<< mess_len << endl;
    //cout << "SharedKey :" << sk<< endl;
    int Bytes = mess_len/32;
    int i=1;
    //cout << "Bytes : " << Bytes << endl;
    // Seeding for key
    while(i<=Bytes){
        string tmp = sk + to_string(i);
        unsigned char* seed = hashSHA2(tmp);
        for(int j=0;j<32;j++){
            key[(i-1)*32 + j] = seed[j];
            //cout << "i="<<i<<",j:"<<j << ",:" << key[(i-1)*32 + j] << endl;
        }
        i++;
    }

    //cout << "Seed is done"<<endl;
    // Converting Cipher text to plain text
    string PT = "";
    for(int i=0;i<mess_len;i++){
        pt[i]= ct[i] ^ key[i];
        //cout << i << ":"<<"C="<<ct[i] << ",K="<< key[i] <<",P=" << pt[i] << endl;
        PT.push_back((char)pt[i]);
    }

    // check if sent plain text matches with recieved text
    // Creating hash of recieved text
    unsigned char* hash_result = hashSHA2(PT);
    string pt_hash="";
    stringstream ss;
    for (int i=0; i<int(sha256_desc.hashsize); i++)
    {
        ss<<hex<<(int)hash_result[i];
        pt_hash = ss.str();
    }
    //cout << "sent pt hash : " << rec_pt_hash << endl;
    //cout << "received pt hash : " << pt_hash << endl; 
    if ( rec_pt_hash == pt_hash ){
    	cout << "Hashes successfully match" << endl;
    	ofstream file("Bob_h.txt");
    	file << pt_hash;
    }

    //cout << "\nPlain Text : "<< PT <<endl;
    // Save Bob Plain Text in File
    ofstream file("BobPlaintext.txt");
    file << PT;

}

unsigned char *hashSHA2(const string &input)
{
    unsigned char *hash_res = new unsigned char[sha256_desc.hashsize];
    hash_state md;
    sha256_init(&md);
    sha256_process(&md, (const unsigned char *) input.c_str(), input.size());
    sha256_done(&md, hash_res);
    return hash_res;
}

