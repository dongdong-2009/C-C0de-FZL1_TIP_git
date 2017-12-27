#include "Bexception.h"

std::ostream& operator<<(std::ostream& os, BaseException& ex)
{
  return os<<ex.get_msg();
}
