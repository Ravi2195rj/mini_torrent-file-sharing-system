// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h> 


struct stat stat_buf;
//#include <sys/syscall.h>
#define PORT 1313
   
using namespace std;
// int main(int argc, char const *argv[]) 
// { 
//     struct sockaddr_in address; 
//     int sock = 0, valread; 
//     struct sockaddr_in serv_addr; 
//     char hello[50] = "Hello from client"; 
//     char buffer[1024] = {0}; 
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
//     { 
//         printf("\n Socket creation error \n"); 
//         return -1; 
//     } 
   
//     memset(&serv_addr, '0', sizeof(serv_addr)); 
   
//     serv_addr.sin_family = AF_INET; 
//     serv_addr.sin_port = htons(PORT); 
       
//     // Convert IPv4 and IPv6 addresses from text to binary form 
//     if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
//     { 
//         printf("\nInvalid address/ Address not supported \n"); 
//         return -1; 
//     } 
   
//     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
//     { 
//         printf("\nConnection Failed \n"); 
//         return -1; 
//     } 
//     send(sock , hello , strlen(hello) , 0 ); 
//     printf("Hello message sent\n"); 
//     valread = read( sock , buffer, 1024); 
//     printf("%s\n",buffer ); 
//     return 0; 
// } 
int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char hello[50] = "Hello from client"; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    // char buffer1[1024]; //let's say read by 1024 char block

    // std::fstream fin;
    // fin.open("./OS_Assignment_2.pdf", ios_base::in|ios_base::binary);
    // stat("./OS_Assignment_2.pdf",&stat_buf);

    // int size=stat_buf.st_size;
    // cout<<size;
    // while (size>0) 
    // {

    //     if(size>1024)
    //     {
    //         int a=1024;
    //         send(sock,&a,sizeof(a),0);
    //         fin.read(buffer1,1024);
    //         send(sock,buffer1,1024,0);
    //     }
    //     else
    //     {

    //        send(sock,&size,sizeof(size),0);
            
    //         fin.read(buffer1,size);
    //         send(sock,buffer1,size,0);

    //     }
    //     size-=1024;          
    // }     
    //         // SHA1((unsigned char*)&buffer, strlen(buffer), (unsigned char*)&digest); 
            
    //         // for (int i = 0; i <20; i++)
    //         //     sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    //         // //cout<<mdString<<"\n";

    //         // SH1=mdString;
    //         // cout<<SH1<<"\n";
    //         // SH2.append(SH1.substr(0,20));

    //         //std::cout << buffer;

    //         //if ()
    //             //break;
            
    
   printf("Hello message sent\n"); 
   valread=read( sock , buffer, 1024); 

 //  printf("%d\n",valread);    
  // valread=read( sock , buffer, 1024); 
 //  printf("%d\n",valread);    

  // printf("%s\n",buffer ); 
    return 0; 
} 