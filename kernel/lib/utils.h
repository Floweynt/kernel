inline bool is_digit(char ch)
{
  return (ch >= '0') && (ch <= '9');
}

template <typename T = unsigned int>
T atoi(const char** str)
{
    T i = 0;
    while (is_digit(**str))
        i = i * 10 + (T)(*((*str)++) - '0');
    return i;
}
