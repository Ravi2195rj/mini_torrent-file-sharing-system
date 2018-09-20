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

using namespace std;
#define TRUE   1
struct stat stat_buf;
string clientIP_Port,tracker1IP_Port,tracker2IP_Port,logfile,client_IP,client_Port,tracker1_IP,tracker1_Port,tracker2_IP,tracker2_Port,filesize;
char* current_Path = getenv ("PWD");
string currentpath=current_Path;
int curser=0,commandmodeline=1,curser_column=15,flag=1;
map <string,string> file_path;
map <string,vector <string>> chunks;

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
void statusbar(string tempstr) //printing status bar and path
{   
    cout<<"\033[1;1m"<<tempstr<<"\033[0m";
    cout<<"\n";
}
    
stack <string> stack1,stack2;
vector <string> Path,Name;

char buffer[1000],path[1000]=".",pop[1000];
// int curser_row=2,curser_column=1,file_num=2,total_files,level=0,search_flag=0,total_rows,total_columns,listend,commandmodeline;
// string dst_root;

// DIR *point_to_directory;
// struct dirent **point_to_file;
// struct stat file_stat;
// struct winsize win;


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
                    // cout<<"\033["<<commandmodeline<<";15H"<<flush;
                    // cout<<command<<flush;
                    // command[curser-1]='\0';
                    //curser--;
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
                if(v[0]=="share")
                {


                        string local_file_path=makefullpath(v[1],currentpath);
                        string filename_with_extension=v[2];
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
                        


                        //SH2.clear();
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
                        
                        for(int i=0;i<tempsize;i++)
                            chunks[SH1].push_back(to_string(i));
                        // for(int i=0;i<tempsize;i++)
                        //     cout<<chunks[SH1][i]<<" ";
                        //cout<<"chunks:"<<chunks[SH1];

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
                   int sock=connection(tracker1_Port,tracker1_IP);
                   send(sock,SH2.c_str(),1024,0);
                   printf("Message sent\n"); 
                   close(sock);

                       v.clear();
                    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;   
                    cout<<"\e[2K"<<flush;
                    statusbar("Enter Command:");

                    curser=0,commandmodeline=1,curser_column=15;
                    cout<<"\033["<<commandmodeline<<";"<<curser_column<<"H"<<flush;
                //    valread=read( sock , buffer, 1024);

                }
                if(v[0]=="get")
                {

                        unsigned char digest[20];
                        char mdString[SHA_DIGEST_LENGTH*2];
                        char tempbuffer[1024];
                        string tempstr;

                        string SH1,SH2="get|";
                        std::fstream fin,fout;

                        string mtorrent_file_path=makefullpath(v[1],currentpath);
                        string destinstion_path=makefullpath(v[2],currentpath);
                        //cout<<local_file_path<<":"<<filename_with_extension<<":";
                        
                        fin.open(mtorrent_file_path,ios_base::in|ios_base::binary);

                        for (int i=0; i<5;i++)
                        {
                            getline(fin,SH1);
                        }

                      //  cout<<SH1;
                        SHA1((unsigned char*)SH1.c_str(), strlen(SH1.c_str()), (unsigned char*)&digest); 

                        for (int i = 0; i <20; i++)
                            sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
                        
                        
                        SH1=mdString;
                        //SH1.append(mdString);
                        SH1=SH1.substr(0,20);

                       

                        SH1.insert(0,"get|");
                        //SH1.insert(0,"get|");
                        //SH2.append(SH1);
                   //     cout<<SH1<<"\n";
                        int sock=connection(tracker1_Port,tracker1_IP);
                        send(sock,SH1.c_str(),1024,0);
                        printf("Message sent\n"); 

                        read( sock , tempbuffer, 1024);
                        cout<<"reading seeder list: "<<tempbuffer;
                        close(sock);

                        int no_of_seeder=0;
                        vector <string> seeder_ip;
                        vector <string> seeder_port;
                        string seeder;
                        seeder=tempbuffer;
                        cout<<"\n"<<seeder;

                        no_of_seeder=seeder[0]-'0'; 
                        cout<<no_of_seeder;
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
                        for(i=0;i<seeder_ip.size();i++)
                        {
                            cout<<"connecting:";
                            sock=connection(seeder_port[i],seeder_ip[i]);
                        }

                            
                        // size_t found = clientIP_Port.find(':');
                        // client_IP=clientIP_Port.substr(0,found);
                        // client_Port=clientIP_Port.substr(found+1);
                        // sock=connection();
                        // send(sock,SH1.c_str(),1024,0);
                        // printf("Message sent\n");


                        v.clear();
                        cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;   
                        cout<<"\e[2K"<<flush;
                        statusbar("Enter Command:");
                        curser=0,commandmodeline=1,curser_column=15;
                        cout<<"\033["<<commandmodeline<<";"<<curser_column<<"H"<<flush;
                        }

                 }


                



         } 
         buffer[0]=buffer[1]=buffer[2]=0;
           
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