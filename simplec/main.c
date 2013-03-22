#include <sys/socket.h>
#include <arpa/inet.h> //нужен для структуры sockaddr_in; а также для фунуций htons и inet_addr
#include <stdio.h>


int main ()
{
   int mySocket;

   mySocket = socket (AF_INET, SOCK_STREAM, 0);

   if (mySocket < 0)
   {
      printf ("ошибка вызова socket\n");
      return 1;
   }

   int ok;
   struct sockaddr_in myAddress;
   myAddress.sin_family = AF_INET;
   myAddress.sin_port = htons (7500);
   myAddress.sin_addr.s_addr = inet_addr ("127.0.0.1");

   ok = connect (mySocket, (struct sockaddr *) &myAddress, sizeof (myAddress));

   if (ok)
   {
      printf ("ошибка вызова connect\n");
      return 1;
   }

   /*
   printf("local ip %s\n",inet_ntoa(myAddress.sin_addr));
   printf("local port %d\n",htons(myAddress.sin_port));
   printf("local domain(family) %s\n\n",(myAddress.sin_family==AF_INET?"AF_INET":"others"));
   //*/

   ok = send (mySocket, "kuku\0", 5, 0);

   if (ok <= 0)
   {
      printf ("ошибка вызова send\n");
      return 1;
   }

   char buf[ 5 ] ;

   ok = recv (mySocket, buf, 5, 0) ;

   if (ok <= 0)
   {
      printf ("ошибка вызова recv\n");
      return 1;
   }
   else
      printf ("%s\n", buf);

   return 0;

}

