//Roll No.: 2018201018
//Name: Ravi Jakhania

//#include "assignment1.h"
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <termios.h>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <ftw.h>
#include <sys/ioctl.h>
#include <openssl/sha.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include <sys/stat.h> 
#include <thread> 
#include <vector>
#include<map>
#include <algorithm>
#include <mutex> 
using namespace std;
#define TRUE   1

struct stat stat_buf;
string clientIP_Port,tracker1IP_Port,tracker2IP_Port,logfile,client_IP,client_Port,tracker1_IP,tracker1_Port,tracker2_IP,tracker2_Port,filesize;
char* current_Path = getenv ("PWD");
string currentpath=current_Path;
int curser=0,commandmodeline=1,curser_column=15,flag=1;
map <string,string> file_path;
map <string,string> chunks;
std::mutex mtx;
string findname(string str1) //finding current directory
{
    int i=str1.length()-1;
    string str2;
    while(str1[i]!='/')
        --i;

    str2=str1.substr(i+1,str1.length()-i-1); 
    return str2;
}
string findpath(string tempstr) //finding parent path
{
    int i=tempstr.length()-1;
    while(tempstr[i]!='/')
        i--;
    //tempstr[i]='\0';
    return tempstr.substr(0,i);
}
string makefullpath(string str1,string currentpath) //making absolute path
{
  //  cout<<currentpath<<"\n";
    string str2;
    if(str1[0]=='~')
        return str1.substr(1);
    else if(str1[0]=='/')
    {
        //str2.append("~");
        //str2.append(str1);
        return str1;
    }
    else if(str1[0]=='.' && str1[1]!='.')
    {
        //str2.append("~");
        str2.append(currentpath);
        str2.append(str1.begin()+1,str1.end());
        return str2;   
    }
    else if(str1[0]=='.' && str1[1]=='.')
    {
        currentpath=findpath(currentpath);
       // cout<<currentpath<<":";
       // cout<<str1.substr(2);
        return makefullpath(str1.substr(3),currentpath);
    }
    else
    {
        //str2.append("~");
        str2.append(currentpath);
        str2.append("/");
        str2.append(str1);
        return str2;
    }
}
void statusbar(string tempstr) //printing status bar and path
{   
    cout<<"\033[1;1m"<<tempstr<<"\033[0m";
    cout<<"\n";
}
int connection(string port,string ip_address)
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
        serv_addr.sin_port = htons(stoi(port)); 
                       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
                   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

   return sock;
}
void getting_chunks(string SH2,vector <int> &final_chunks_getting,string seeder_port,string seeder_ip)
{
    string tempstr=SH2;                                          //SH2
                            //cout<<i;
        string temp;
        char chn[1024];
                            //cout<<"connecting:";
        int sock=connection(seeder_port,seeder_ip);              
        tempstr.insert(0,"chunks:");
                            //cout<<tempstr;
                           // cout<<"sending";
        send(sock,tempstr.c_str(),1024,0);
                            //cout<<"readnig";
        read( sock , chn, 1024);
                           // cout<<"after reading";
        temp=chn;
                          //  cout<<"getting chunks"<<temp<<"end"<<"\n";
        int k;
                            
        while(temp.length()!=0)
        {

                               // cout<<temp<<"\n";
            size_t found = temp.find(':');
            if(found!=std::string::npos)
            {
                final_chunks_getting.push_back(stoi(temp.substr(0,found)));
                                 //   cout<<"happening\n";
                temp=temp.substr(found+1);
                               //     cout<<temp<<"\n";
                                    
            }
            else
            {
                final_chunks_getting.push_back(stoi(temp));
                break;
            }
                                
        }

        for(int j=0;j<final_chunks_getting.size();j++)
            cout<<final_chunks_getting[j]<<" ";

        sort(final_chunks_getting.begin(),final_chunks_getting.end());

        close(sock);
}
void download_chunks(string IP,string Port,vector <int> &client_chunk,string destination_filepath,string hash,fstream& fout)
{
    for(int i=0;i<client_chunk.size();i++)
    {   
        
        int sock=connection(Port,IP);
        string message="download:";
        message.append(hash);
        message.append(":");
        message.append(to_string(client_chunk[i]));
        cout<<message<<"\n";
       
        send(sock,message.c_str(),1024,0);
        
        cout<<"chunks:"<<client_chunk[i]<<":"<<flush;
        
        int size,no_of_byes_read=0;
        int read_bytes=read(sock,&size,sizeof(size));
        
        mtx.lock();        
        fout.seekp (client_chunk[i]*512*1024, fout.beg);
        
        cout<<fout.tellp()<<":"<<flush;
        //mtx.unlock();
        cout<<size<<":"<<flush;
       
        
        

        while (no_of_byes_read<size) 
        {
            cout<<"reading";
            char buffer[512*1024];
            int val=read(sock,buffer,512*1024);
            cout<<val<<"\n";
            no_of_byes_read+=val;
          //  mtx.lock();
            fout.write(buffer,val);
            
        }
        mtx.unlock();
        close(sock);
        
    }
}
void upload_chunks(string SHA_chunk_string,int socket)
{
    //sha:chunks
    string path;
    size_t found = SHA_chunk_string.find(':');
    path=SHA_chunk_string.substr(0,found);

    SHA_chunk_string=SHA_chunk_string.substr(found+1); //
    
    string path_of_file=file_path[path];
    
    std::fstream fin;
    fin.open(path_of_file, ios_base::in|ios_base::binary);

    cout<<"chunks to be send"<<SHA_chunk_string;

    char buffer1[512*1024];
    int size;
    fin.seekg (stoi(SHA_chunk_string)*512*1024, fin.beg);
    cout<<fin.tellg()<<":";
                                    
    fin.read(buffer1,1024*512);
                                    
    size=fin.gcount();
    cout<<fin.gcount()<<"\n";
                                    
                                    
    send(socket,&size,sizeof(size),0);
    
    send(socket,buffer1,size,0);
     fin.close();
}
void sharing(vector <string> v)
{

    string local_file_path=makefullpath(v[1],currentpath);
    string filename_with_extension=v[2];
    cout<<filename_with_extension;
    cout<<local_file_path<<":"<<filename_with_extension<<":";
    stat(local_file_path.c_str(),&stat_buf);
                        
    int size=stat_buf.st_size;
    int tempsize=size;
    
    filesize=to_string(size);
    unsigned char digest[20];
    char mdString[SHA_DIGEST_LENGTH*2];
                        
    string SH1, SH2;
    std::fstream fin,fout;
                        
    fin.open(local_file_path, ios_base::in|ios_base::binary);
    fout.open(filename_with_extension, ios_base::out);

    fout << tracker1_IP.c_str() << ":";
    fout << tracker1_Port.c_str() << "\n";
    fout << tracker2_IP.c_str() << ":";
    fout << tracker2_Port.c_str() << "\n";
    fout << local_file_path.c_str() << "\n";
    fout << filesize.c_str() << "\n";


    while (tempsize>0) 
    {   

        if(tempsize>1024*512)
        {
            char buffer[1024*512];
            fin.read(buffer,1024*512);
            SHA1((unsigned char*)&buffer, strlen(buffer), (unsigned char*)&digest);

            for (int i = 0; i <20; i++)
                sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

            SH1=mdString;
            SH2.append(SH1.substr(0,20));                  
        }
        else
        {
            char buffer[tempsize];
            fin.read(buffer,tempsize);
            SHA1((unsigned char*)&buffer, strlen(buffer), (unsigned char*)&digest);

            for (int i = 0; i <20; i++)
                sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

            SH1=mdString;
            SH2.append(SH1.substr(0,20)); 
        }
        tempsize-=1024*512;
    }

    fout << SH2.c_str();
    
    SHA1((unsigned char*)SH2.c_str(), strlen(SH2.c_str()), (unsigned char*)&digest);

    for (int i = 0; i <20; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    SH1=mdString;
    SH1=SH1.substr(0,20);

    cout<<SH1;
    file_path[SH1]=local_file_path;                                //storing file path in map with key as SHA.
                      //  cout<<"in map:"<<file_path[SH1];
    tempsize=size/(1024*512);
    if(size%(1024*512)!=0)
        tempsize++;
                    //    cout<<"tmp:"<<tempsize<<"\n";
    string temp;
    for(int i=0;i<tempsize;i++)
    {
        temp.append(to_string(i));
        if(i<tempsize-1)
            temp.append(":");
    }

    chunks[SH1]=temp;
                        
    cout<<"chunks:"<<chunks[SH1];

    SH2.clear();
    SH2="share|";
    SH2.append(findname(local_file_path));
    SH2.append("|");
    SH2.append(SH1);
    SH2.append("|");
    SH2.append(client_IP);
    SH2.append("|");
    SH2.append(client_Port);

                      //  cout<<SH2;
                        //cout<<v[1]<<":"<<v[2];
    int size=strlen(SH2.c_str());
    int sock=connection(tracker1_Port,tracker1_IP);
    send(sock,&size,sizeof(size),0);
    send(sock,SH2.c_str(),size,0);
    printf("Message sent\n"); 
    close(sock);
    fin.close();
    fout.close();
}

void getting(vector <string> v)
{
    unsigned char digest[20];
    char mdString[SHA_DIGEST_LENGTH*2];
    char tempbuffer[1024];
    string tempstr;
    string SH1,SH2;
    
    std::fstream fin,fout;

    string mtorrent_file_path=makefullpath(v[1],currentpath);
    string destinstion_path=makefullpath(v[2],currentpath);


    fin.open(mtorrent_file_path,ios_base::in|ios_base::binary);

    for (int i=0; i<5;i++)
    {
        getline(fin,SH1);
    }
    
    fin.close();
                      //  cout<<SH1;
    SHA1((unsigned char*)SH1.c_str(), strlen(SH1.c_str()), (unsigned char*)&digest); 

    for (int i = 0; i <20; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
                        
                        
    SH1=mdString;
                        //SH1.append(mdString);
    SH1=SH1.substr(0,20);
    SH2=SH1;
                       

    SH1.insert(0,"get|");
               
    int sock=connection(tracker1_Port,tracker1_IP);
    send(sock,SH1.c_str(),1024,0);
                     //   printf("Message sent\n"); 

    read( sock , tempbuffer, 1024);
                     //   cout<<"reading seeder list: "<<tempbuffer;
    close(sock);

    int no_of_seeder=0;
    vector <string> seeder_ip;
    vector <string> seeder_port;
    string seeder;
    seeder=tempbuffer;
    cout<<"\n"<<seeder;

    no_of_seeder=seeder[0]-'0'; 
                     //   cout<<"seeder"<<no_of_seeder;
    seeder=seeder.substr(2);
    cout<<"\n"<<seeder;
    for(int i=0;i<no_of_seeder*2;i++)
    {
        size_t found = seeder.find(':');
        if(i%2==0)
            seeder_ip.push_back(seeder.substr(0,found));
        else
            seeder_port.push_back(seeder.substr(0,found));
        if(i!=no_of_seeder*2-1)
            seeder=seeder.substr(found+1);
    }

    cout<<seeder_ip.size();
    std::vector<int> final_chunks_getting[no_of_seeder];           //ggggggggggggggg
    std::vector<int> final_chunks_downloading[no_of_seeder];           
    std::thread mythread[no_of_seeder];  
    for(int i=0;i<no_of_seeder;i++)                                //i
    {
        mythread[i]=std::thread(getting_chunks,SH2,std::ref(final_chunks_getting[i]),seeder_port[i],seeder_ip[i]); //std::thread t1(sharing, std::ref(v));
        //getting_chunks(SH2,final_chunks_getting[i],seeder_port[i],seeder_ip[i]);
    }
    for(int i=0;i<no_of_seeder;i++)                                //i
    {
        mythread[i].join();
        //getting_chunks(SH2,final_chunks_getting[i],seeder_port[i],seeder_ip[i]);
    }

    string size;

    fin.open(mtorrent_file_path,ios_base::in|ios_base::binary);

    for (int i=0; i<4;i++)
    {
        getline(fin,size);
    }
    fin.close();

    fout.open(destinstion_path,ios_base::out);
                        // fout.seekp(stoi(size)-1);
                        // fout << 'a';
                        // fout.close();
                        
    int no_of_chunks=stoi(size)/(512*1024);
    if(stoi(size)%(512*1024)!=0)
        no_of_chunks++;

    cout<<"chunks from mtorrent:"<<no_of_chunks<<"\n";
    int visited_chunks[no_of_chunks]={0,0};
    int temp_num_of_chunks=no_of_chunks;
    int seeder_pointer[no_of_seeder]={0,0};

    for(int i=0;i<no_of_seeder;i++)
    {
        for(int j=0; j<final_chunks_getting[i].size();j++)
            cout<<final_chunks_getting[i][j]<<" ";
        cout<<"\n";
    }
                     
    while(temp_num_of_chunks>0)
    {
        for(int i=0;i<no_of_seeder;i++)
        {
            int j=seeder_pointer[i];
            while(j<=final_chunks_getting[i].size()-1 && visited_chunks[final_chunks_getting[i][j]]==1)
            {
                seeder_pointer[i]++;
                j=seeder_pointer[i];
            }
            
            if(j<=final_chunks_getting[i].size()-1)
            {
                visited_chunks[final_chunks_getting[i][j]]=1;   
                final_chunks_downloading[i].push_back(final_chunks_getting[i][j]);
                cout<<final_chunks_getting[i][j]<<"\n";
                seeder_pointer[i]++;
                temp_num_of_chunks--;
            }
            
            if(temp_num_of_chunks==0)
                i=no_of_seeder;
        }    
    }
    cout<<"final chunks\n";
    for(int i=0;i<no_of_seeder;i++)
    {
        for(int j=0; j<final_chunks_downloading[i].size();j++)
            cout<<final_chunks_downloading[i][j]<<" ";
        cout<<"\n";
    }

    std::thread my1thread [no_of_seeder];
    for(int i=0;i<no_of_seeder;i++)
    {
        cout<<SH2<<"\n";
        my1thread[i]=std::thread(download_chunks,seeder_ip[i],seeder_port[i],std::ref(final_chunks_downloading[i]),destinstion_path,SH2,std::ref(fout));
      //  download_chunks(seeder_ip[i],seeder_port[i],final_chunks_downloading[i],destinstion_path,SH2,fout);
                     // calling new thread;
        //d1.detach();
    }
    for(int i=0;i<no_of_seeder;i++)
    {
        my1thread[i].join();
    }
    fout.close();

    v.clear();
    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;   
    cout<<"\e[2K"<<flush;
    statusbar("Enter Command:");
    curser=0,commandmodeline=1,curser_column=15;
    cout<<"\033["<<commandmodeline<<";"<<curser_column<<"H"<<flush;
}


void act_as_server()
{
   // cout<<"hello from thread\n";
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[100] ,  
          max_clients = 100 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   
         
    char buffer[1025];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
         
    //a message  
    char *message = "ECHO Daemon v1.0 \r\n";   
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons(stoi(client_Port));   
         
    //bind the socket to localhost port 1313  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
 //   printf("Listener on port %d \n", stoi(client_Port));   
         
    //try to specify maximum of 100 pending connections for the master socket  
    if (listen(master_socket, 100) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
 //   puts("Waiting for connections ...");   
         
    while(TRUE)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
           
            //send new connection greeting message  

            valread=read( new_socket , buffer, 1024);
            cout<<"hello"<<client_IP<<":message from client in thread:"<<buffer<<"\n";
            string command;
            string message=buffer;
            //std::string delimiter = "|";

            size_t found = message.find(':');
            command=message.substr(0,found);

            if(command=="chunks")
            {
                message=message.substr(found+1);
                send(new_socket, chunks[message].c_str(), strlen(chunks[message].c_str()), 0);
            }
            else if(command=="download")
            {
                message=message.substr(found+1);
                cout<<"request for download:";
                upload_chunks(message,new_socket);
            }
           
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen); 
                    printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';   
                    send(sd , buffer , strlen(buffer) , 0 );   
                }   
            }   
        }   
    }
}


void findIP(string clientIP_Port,string tracker1IP_Port,string tracker2IP_Port)
{
    size_t found = clientIP_Port.find(':');
    client_IP=clientIP_Port.substr(0,found);
    client_Port=clientIP_Port.substr(found+1);

    size_t found1 = tracker1IP_Port.find(':');
    tracker1_IP=tracker1IP_Port.substr(0,found1);
    tracker1_Port=tracker1IP_Port.substr(found1+1);
    
    size_t found2 = tracker2IP_Port.find(':');
    tracker2_IP=tracker2IP_Port.substr(0,found2);
    tracker2_Port=tracker2IP_Port.substr(found2+1);
}


void gotoNonCanon() //going to non canon
{
    struct termios initial_settings, new_settings;
    FILE *input;
    FILE *output;

    input = fopen("/dev/tty", "r");
    output = fopen("/dev/tty", "w");
    tcgetattr(fileno(input),&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;

    if(tcsetattr(fileno(input), TCSANOW, &new_settings) != 0) 
    {
        fprintf(stderr,"could not set attributes\n");
    }
  
//  tcsetattr(fileno(input),TCSANOW,&initial_settings);
}


int main(int argc, char *argv[])
{
    gotoNonCanon(); //Going to Non Canon mode.
    
    cout<<"\033[3J";
    cout<<"\033[H\033[J";
   
    statusbar("Enter Command:");
    cout<<" ";
    // char* current_Path;
    // current_Path = getenv ("PWD");
    // string currentpath=current_Path;
    //cout<<current_Path<<flush;
    
    clientIP_Port=argv[1];
    tracker1IP_Port=argv[2];
    tracker2IP_Port=argv[3];
    
    //logfile=argv[4];
    
    findIP(clientIP_Port,tracker1IP_Port,tracker2IP_Port);

    // cout<<clientIP_Port<<"\n"<<flush;
    // cout<<tracker1IP_Port<<"\n"<<flush;
    // cout<<tracker2IP_Port<<"\n"<<flush;
    
    
    // cout<<client_IP<<"\n"<<flush;
    // cout<<client_Port<<"\n"<<flush;
    // cout<<tracker1_IP<<"\n"<<flush;
    // cout<<tracker1_Port<<"\n"<<flush;
    // cout<<tracker2_IP<<"\n"<<flush;
    // cout<<tracker2_Port<<"\n"<<flush;

    thread th1(act_as_server); 

    cout<<"\033["<<commandmodeline<<";"<<curser_column<<"H"<<flush;
    while(flag)
    { 
        char command[500];
        char buffer[500];
        read(0,buffer,128);
        if(buffer[0]==27 && buffer[1]!=91) // Esc key , Going back to normal mode
        {
            flag=0;
        }

        else if(buffer[0]!=27 && buffer[0]!=127 && buffer[0]!=10)  //Entering Input or commands.
        {
            command[curser++]=buffer[0];
            cout<<buffer[0]<<flush;
            curser_column++;
        }
        else if(buffer[0]==127 || buffer[0]==8) // Pressing Back space to Erase text.
        {
            if(curser_column>15)
            {
                command[--curser]=' ';
                command[curser+1]='\0';
                cout<<"\033["<<commandmodeline<<";15H"<<flush;
                cout<<command<<flush;
                command[curser]='\0';
                cout<<"\033["<<commandmodeline<<";"<<--curser_column<<"H"<<flush;
            }
                  
        }
        else if(buffer[0]==10) // Pressing Enter key to execute commands
        {
            std:: vector<string> v;
            command[curser]='\0';
            char s[1000];
            int j=0,i;

            for(i=0;command[i]!='\0';i++) // Differentiate Operation and Arguments.
            {
                if(command[i]==' ')
                {
                    if(command[i-1]==92)
                        s[j++]=command[i];
                    else
                    {
                        s[j]='\0';
                        v.push_back(s); 
                        j=0;    
                    }
                }
                else if(command[i]!=92)
                {
                    s[j++]=command[i];
                }
            }

            s[j]=command[i];
            v.push_back(s);
            cout<<"\n"<<flush;
            cout<<"\e[2K"<<flush;
            cout<<v[1]<<" "<<v[2]<<"\n";
            if(v[0]=="share")
            {
               // sharing(v);
                std::thread t1(sharing,v);
		        t1.detach();
             //    valread=read( sock , buffer, 1024);
            }
             
            if(v[0]=="get")
            {
               // getting(v);
                std::thread t2(getting,v);
                t2.detach();
            }
            v.clear();
    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;   
    cout<<"\e[2K"<<flush;
    statusbar("Enter Command:");

    curser=0,commandmodeline=1,curser_column=15;
    cout<<"\033["<<commandmodeline<<";"<<curser_column<<"H"<<flush;
    // fin.close();
    // fout.close();

        }

        buffer[0]=buffer[1]=buffer[2]=0;
    } 
           
    return 0;
}


                    //string s;
                        //s << (5);

                       // cout<<SHA1;
                //         stat(local_file_path.c_str(),&stat_buf);
                        
                //         int size=stat_buf.st_size;
                //         int tempsize=size;
                        
                //         filesize=to_string(size);
                        
                //         unsigned char digest[20];
                //         char mdString[SHA_DIGEST_LENGTH*2];
                        
                //         string SH1, SH2;
                //         std::fstream fin,fout;
                        
                //         fin.open(local_file_path, ios_base::in|ios_base::binary);
                //         fout.open(filename_with_extension, ios_base::out);

                //         fout << tracker1_IP.c_str() << ":";
                //         fout << tracker1_Port.c_str() << "\n";
                //         fout << tracker2_IP.c_str() << ":";
                //         fout << tracker2_Port.c_str() << "\n";
                //         fout << local_file_path.c_str() << "\n";
                //         fout << filesize.c_str() << "\n";


                //         while (tempsize>0) 
                //             {   

                //                 if(tempsize>1024*512)
                //                 {
                //                     char buffer[1024*512];
                //                     fin.read(buffer,1024*512);
                //                     SHA1((unsigned char*)&buffer, strlen(buffer), (unsigned char*)&digest); 
                //                     for (int i = 0; i <20; i++)
                //                         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
                //                     SH1=mdString;
                //                     SH2.append(SH1.substr(0,20));                  
                //                 }
                //                 else
                //                 {
                //                     char buffer[tempsize];
                //                     fin.read(buffer,tempsize);
                //                     SHA1((unsigned char*)&buffer, strlen(buffer), (unsigned char*)&digest); 
                //                     for (int i = 0; i <20; i++)
                //                         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
                //                     SH1=mdString;
                //                     SH2.append(SH1.substr(0,20)); 
                //                 }
                //                 tempsize-=1024*512;
                //             }

                //         fout << SH2.c_str();
                //         //SH2.clear();
                //         SHA1((unsigned char*)SH2.c_str(), strlen(SH2.c_str()), (unsigned char*)&digest); 
                //         for (int i = 0; i <20; i++)
                //             sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
                //         SH1=mdString;
                //         //cout<<SH1;
                //         SH2.clear();
                //         SH2="share|";
                //         SH2.append(findname(local_file_path));
                //         SH2.append("|");
                //         SH2.append(SH1);
                //         SH2.append("|");
                //         SH2.append(client_IP);
                //         SH2.append("|");
                //         SH2.append(client_Port);

                //         cout<<SH2;
                //         //cout<<v[1]<<":"<<v[2];
                //    int sock=connection_with_tracker();
                //    send(sock,SH2.c_str(),1024,0);
                //    printf("Message sent\n"); 
                // //    valread=read( sock , buffer, 1024);
