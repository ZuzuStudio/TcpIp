#ifndef TCPEXCEPTION_H
#define TCPEXCEPTION_H

#include <exception>

namespace hzw
{
   class ExceptionSocket: public std::exception
   {
      public:
         const char *what() const throw()
         {
            return "hzw::sokcet";
         }
   };

   class ExceptionConnect: public std::exception
   {
      public:
         const char *what() const throw()
         {
            return "hzw::connect";
         }
   };

   class ExceptionRecive: public std::exception
   {
      public:
         const char *what() const throw()
         {
            return "hzw::recv";
         }
   };

   class ExceptionSend: public std::exception
   {
      public:
         const char *what() const throw()
         {
            return "hzw::send";
         }
   };
}

#endif // TCPEXCEPTION_H

