#include <sys/socket.h>
#include <arpa/inet.h> //нужен для структуры sockaddr_in; а также для фунуций htons и inet_addr
//#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <hzw/queue.h>
#include "tcpexception.h"

using namespace std;
using namespace hzw;

enum State {FINISH, START, NORMAL, CONTROL, DISCONNECT, ENDING};

char catchChar();

int main(int argc, char **argv)
{
   int sendFile, recvFile;

   try
   {
      int mySocket;

      mySocket = socket(AF_INET, SOCK_STREAM, 0);

      if(mySocket < 0)
         throw ExceptionSocket();



      sendFile = open("send.eml", O_WRONLY | O_CREAT | O_TRUNC, 0664);
      recvFile = open("recv.eml", O_RDONLY);

      /*

      int bad;
      struct sockaddr_in myAddress;
      myAddress.sin_family = AF_INET;
      myAddress.sin_port = htons(7500);

      if(argc == 1)
         myAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
      else
         myAddress.sin_addr.s_addr = inet_addr(argv[1]);

      bad = connect(mySocket, (struct sockaddr *) &myAddress, sizeof(myAddress));

      if(bad)
      {
         cerr << "ошибка вызова connect\n";
         return 1;
      }*/

      Queue<char> qInput, qSend, qRecipe, qOutput, qChecked;
      bool silentWindow[2] = {true, true};
      char ch;
      char controlWord[20] = "";
      State state = NORMAL;
      int downcounter = 50;

      do
      {
         system("sleep 0.01");
         //console input

         if(NORMAL == state || CONTROL == state)
         {
            ch = catchChar();

            if(ch != EOF)
            {
               qInput.enqueue(ch);
            }
         }

         //qInput
         if(NORMAL == state)
         {
            int combine = (qInput.isEmpty() ? 0 : 1) + (qChecked.isEmpty() ? 0 : 2);

            switch(combine)
            {
               case 3:
               case 2:
                  qSend.enqueue(qChecked.onFront());
                  qChecked.dequeue();
                  break;
               case 1:

                  if('/' == qInput.onFront())
                     state = CONTROL;// TO control state
                  else
                  {
                     qSend.enqueue(qInput.onFront());
                     qInput.dequeue();
                  }

                  break;
               case 0:
                  break;
            }
         }

         //qChecked
         if(CONTROL == state)
            if(!qInput.isEmpty())
            {
               char smallString[2] = {'\0', '\0'};
               smallString[0] = qInput.onFront();
               qChecked.enqueue(qInput.onFront());
               qInput.dequeue();
               strcat(controlWord, smallString);

               if(!strcmp(smallString, "/exit"))
               {
                  state = DISCONNECT;
                  qChecked.clear();
                  strcpy(controlWord, "");
               }
               else
               {
                  if(strlen(controlWord) > 7)
                  {
                     state = NORMAL;
                     strcpy(controlWord, "");
                  }
               }
            }

         //qSend normall
         if(NORMAL == state || ENDING == state)
            if(!qSend.isEmpty())
            {
               ch = qSend.onFront();
               qSend.dequeue();

               if(write(sendFile, (const void *) &ch, 1) <= 0)
                  throw ExceptionSend();

               //if(send(mySocket, (const void *) &ch, 1, 0) <= 0)
               //   throw ExceptionSend();


            }
            else
            {
               ch = '\x10';

               if(write(sendFile, (const void *) &ch, 1) <= 0)
                  throw ExceptionSend();

               //if(send(mySocket, (const void *) &ch, 1, 0) <= 0)
               //   throw ExceptionSend();
            }

         if(DISCONNECT == state)
         {
            ch = '\x04';

            if(write(sendFile, (const void *) &ch, 1) <= 0)
               throw ExceptionSend();

            //if(send(mySocket, (const void *) &ch, 1, 0) <= 0)
            //   throw ExceptionSend();
            state = ENDING;
         }

         //normal recive
         if(NORMAL == state)
         {
            if(read(recvFile, (void *) &ch, 1) <= 0)
               throw ExceptionRecive();

            if('\x04' == ch)
               state = ENDING;
         }

         //ending close
         if(ENDING == state)
            if(qSend.isEmpty() && qOutput.isEmpty())
               if(downcounter)
                  --downcounter;
               else
                  state = FINISH;
            else
               downcounter = 50;
      }
      while(state);
   }
   catch(exception &e)
   {
      perror("произошло следующее исключение:\n");
      perror(e.what());
      return 1;
   }

   /*
   int len = 5;
   bad = send(mySocket, "kuku\0", len, 0);

   if(bad <= 0)
   {
      cerr << "ошибка вызова send\n";
      return 1;
   }

   char buf[ len ] ;

   bad = recv(mySocket, buf, len, 0) ;

   if(bad <= 0)
   {
      cerr << "ошибка вызова recv\n";
      return 1;
   }
   else
      cout << buf << endl;
      */

   return 0;

}

char catchChar()
{
   char ch;
   int oldConsoleFlag;
   termios oldTermios, newTermios;
   tcgetattr(STDIN_FILENO, &oldTermios);
   newTermios = oldTermios;
   newTermios.c_lflag &= ~ICANON;
   tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
   oldConsoleFlag = fcntl(STDIN_FILENO, F_GETFL, 0);
   fcntl(STDIN_FILENO, F_SETFL, oldConsoleFlag | O_NONBLOCK);
   ch = getchar();
   tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
   fcntl(STDIN_FILENO, F_SETFL, oldConsoleFlag);
   return ch;
}









